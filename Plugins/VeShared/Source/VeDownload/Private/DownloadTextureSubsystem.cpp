// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "DownloadTextureSubsystem.h"

const FName FDownloadTextureSubsystem::Name = FName("TextureLoadSubsystem");

void FDownloadTextureSubsystem::Initialize() {}

void FDownloadTextureSubsystem::Shutdown() {
	StopTick();
	ClearCachedTextures();
}

bool FDownloadTextureSubsystem::LoadTexture(const TSharedPtr<FTextureLoadRequest> InRequest) {

#pragma region Check input data
	if (!InRequest) {
		LogErrorF("invalid request");
		return false;
	}

	if (InRequest->Url.IsEmpty()) {
		LogErrorF("invalid request url");
		return false;
	}

	if (!InRequest->GetOnComplete().IsBound()) {
		LogWarningF("request OnComplete delegate is not bound");
		return false;
	}

#pragma endregion

#pragma region Memory Cache

	// Try to get the texture from the cache.
	if (const auto CachedTexturePtr = GetCachedTexture(InRequest->Url)) {
		// todo: understand if texture is to be deleted on the next pass or not
		if (CachedTexturePtr->TexturePtr.IsValid()) {
			if (InRequest->GetOnComplete().IsBound()) {
				FTextureLoadResult Result(InRequest->Url, CachedTexturePtr->TexturePtr.Get());
				Result.bResolved = true;
				Result.bMemoryCached = true;

				// Notify request 
				InRequest->GetOnComplete().Execute(Result);
			}

			return true;
		}
	}

#pragma endregion

#pragma region Pending Tasks
	for (const TSharedPtr<FTextureLoadTask>& Task : ProcessingTasks) {
		if (*Task->GetRequest() == *InRequest) {
			Task->GetOnComplete().AddSP(InRequest.ToSharedRef(), &FTextureLoadRequest::OnTaskCompleted);
			// todo: add texture reference
			return true;
		}
	}

	for (const TSharedPtr<FTextureLoadTask>& Task : PendingTasks) {
		if (*Task->GetRequest() == *InRequest) {
			Task->GetOnComplete().AddSP(InRequest.ToSharedRef(), &FTextureLoadRequest::OnTaskCompleted);
			// todo: add texture reference
			return true;
		}
	}
#pragma endregion

#pragma region New Task
	{
		// Create a new texture loading task.
		const TSharedPtr<FTextureLoadTask> Task = MakeShareable(new FTextureLoadTask(InRequest));

		Task->GetOnComplete().AddRaw(this, &FDownloadTextureSubsystem::OnTaskCompleted);

		// Schedule the task.
		PendingTasks.Add(Task);
	}

#pragma endregion

	// Start ticking to update queues and delete completed tasks.
	StartTick();

	return true;
}

void FDownloadTextureSubsystem::AddCachedTexture(const TSharedPtr<FManagedTexture> InTexture) {
	// ReSharper disable once CppLocalVariableWithNonTrivialDtorIsNeverUsed
	FScopeLock Lock(&CachedTexturesLock);
	CachedTextures.Add(InTexture->Url, InTexture);
}

void FDownloadTextureSubsystem::AddObjectReference(const FString& InUrl, IManagedTextureRequester* InReference) {
	if (const TSharedPtr<FManagedTexture>* ManagedTexture = CachedTextures.Find(InUrl)) {
		ManagedTexture->Get()->AddReference(InReference);
	} else {
		LogErrorF("no texture to add reference to");
	}
}

void FDownloadTextureSubsystem::RemoveAllObjectReferences(IManagedTextureRequester* InReference) {
	PendingRemoveAllReferencesList.Add(InReference);

	if (CleanupTickDelegateHandle.IsValid()) {
		FTSTicker::GetCoreTicker().RemoveTicker(CleanupTickDelegateHandle);
		CleanupTickDelegateHandle.Reset();
	}

	CleanupTickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([=](float DeltaSeconds) {
		ProcessRemoveObjectReferences();
		ProcessRemoveAllObjectReferences();

		FTSTicker::GetCoreTicker().RemoveTicker(CleanupTickDelegateHandle);
		CleanupTickDelegateHandle.Reset();
		return true;
	}), CleanupTickInterval);
}

void FDownloadTextureSubsystem::RemoveObjectReference(const FString& InUrl, IManagedTextureRequester* InReference) {
	PendingRemoveReferenceList.Add(InUrl, InReference);

	if (CleanupTickDelegateHandle.IsValid()) {
		FTSTicker::GetCoreTicker().RemoveTicker(CleanupTickDelegateHandle);
		CleanupTickDelegateHandle.Reset();
	}

	CleanupTickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([=](float DeltaSeconds) {
		ProcessRemoveObjectReferences();
		ProcessRemoveAllObjectReferences();

		FTSTicker::GetCoreTicker().RemoveTicker(CleanupTickDelegateHandle);
		CleanupTickDelegateHandle.Reset();
		return true;
	}), CleanupTickInterval);
}

TSharedPtr<FManagedTexture> FDownloadTextureSubsystem::GetCachedTexture(const FString& InUrl) {
	// ReSharper disable once CppLocalVariableWithNonTrivialDtorIsNeverUsed
	FScopeLock Lock(&CachedTexturesLock);
	const TSharedPtr<FManagedTexture>* TexturePtr = CachedTextures.Find(InUrl);
	if (TexturePtr) {
		return *TexturePtr;
	}
	return nullptr;
}

void FDownloadTextureSubsystem::ClearCachedTextures() {
	// ReSharper disable once CppLocalVariableWithNonTrivialDtorIsNeverUsed
	FScopeLock Lock(&CachedTexturesLock);

	for (const auto& Pair : CachedTextures) {
		Pair.Value->ClearTexture();
	}

	CachedTextures.Empty();
}

void FDownloadTextureSubsystem::ProcessRemoveObjectReferences() {
	if (PendingRemoveReferenceList.Num() <= 0) {
		return;
	}

	for (auto It = PendingRemoveReferenceList.CreateIterator(); It; ++It) {
		const FString InUrl = It.Key();
		IManagedTextureRequester* InReference = It.Value();

		if (const TSharedPtr<FManagedTexture>* ManagedTexture2D = CachedTextures.Find(InUrl)) {
			if (ManagedTexture2D->Get()->RemoveReference(InReference)) {
				OnTextureDestroyed.Broadcast(InUrl);
			}
		} else {
			LogErrorF("no texture to remove reference from");
		}
	}

	PendingRemoveReferenceList.Empty();
}

void FDownloadTextureSubsystem::ProcessRemoveAllObjectReferences() {
	if (PendingRemoveAllReferencesList.Num() <= 0) {
		return;
	}

	for (const auto InReference : PendingRemoveAllReferencesList) {
		TArray<FString> KeysToRemove;

		if (CachedTextures.Num() > 0) {
			for (auto It = CachedTextures.CreateIterator(); It; ++It) {
				if (It.Value().Get()->RemoveReference(InReference)) {
					OnTextureDestroyed.Broadcast(It.Key());
					KeysToRemove.Add(It.Key());
				}
			}
		}

		if (KeysToRemove.Num() > 0) {
			for (auto Key : KeysToRemove) {
				if (auto* CachedTexturePtr = CachedTextures.Find(Key)) {
					(*CachedTexturePtr).Reset();
				}
				CachedTextures.Remove(Key);
			}
		}
	}

	PendingRemoveAllReferencesList.Empty();
}

void FDownloadTextureSubsystem::OnTaskCompleted(const FTextureLoadResult& Result) {
	const FManagedTexturePtr CachedTexture2D = MakeShared<FManagedTexture>(Result.Url, Result.Texture);
	AddCachedTexture(CachedTexture2D);
}

void FDownloadTextureSubsystem::StartNextTaskProcessing() {
	// Get the first task from the queue.
	const TSharedPtr<FTextureLoadTask> Task = PendingTasks[0];

	if (!Task) {
		// todo: should we remove this task from the queue?
		LogErrorF("failed to get the scheduled task");
		return;
	}

	if (!Task->GetRequest()) {
		// todo: should we remove this task from the queue?
		LogErrorF("failed to get request from the task");
		return;
	}

	// Try to get texture from the cache.
	const auto CachedTexturePtr = GetCachedTexture(Task->GetRequest()->Url);
	if (CachedTexturePtr && CachedTexturePtr->TexturePtr.IsValid()) {
		// Create a result object.
		FTextureLoadResult Result(Task->GetUrl(), CachedTexturePtr->TexturePtr.Get());
		Result.bResolved = true;
		Result.bMemoryCached = true;

		// Broadcast the result.
		Task->GetRequest()->OnTaskCompleted(Result);

		// Remove the task from the pending queue.
		PendingTasks.RemoveAt(0);

		return;
	}

#if 0
	const FString FilePath = FPaths::ProjectContentDir() / "Cache" / FPaths::GetBaseFilename(Task->GetRequest()->Url);
#endif

	Task->StartBackgroundTask();

	// Move the task from the pending to the processing tasks queue.
	ProcessingTasks.Add(Task);
	PendingTasks.RemoveAt(0);
}

bool FDownloadTextureSubsystem::Tick(float DeltaSeconds) {
	// Check if there are pending tasks available.
	while (PendingTasks.Num() > 0 && ProcessingTasks.Num() < MaxParallelTasks) {
		StartNextTaskProcessing();
	}

	// Remove completed tasks. Warning: All the delegates must be executed before the memory is freed, otherwise the memory corruption will occur on delegate call.
	for (auto It = ProcessingTasks.CreateIterator(); It; ++It) {
		if (!It) {
			LogErrorF("invalid processing task");
			continue;
		}

		if (It->Get()->IsComplete()) {
			It.RemoveCurrent();
		}
	}

	// Stop ticking if no more pending and processing tasks left. Return true so that we can restart the ticker later.
	if (PendingTasks.Num() == 0 && ProcessingTasks.Num() == 0) {
		StopTick();
		return true;
	}

	return true;
}

void FDownloadTextureSubsystem::StartTick() {
	if (!TickDelegateHandle.IsValid()) {
		TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FDownloadTextureSubsystem::Tick), TickInterval);
	}
}

void FDownloadTextureSubsystem::StopTick() {
	if (TickDelegateHandle.IsValid()) {
		FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
		TickDelegateHandle.Reset();
	}
}
