// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "DownloadChunkSubsystem.h"

#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "VeShared.h"
#include "VeDownload.h"

const FName FDownloadChunkSubsystem::Name = FName("ChunkDownloadSubsystem");

FDownloadSubscriptionPtr FDownloadChunkSubsystem::DownloadFile(const FFileDownloadInfo& Info, uint32 Priority, bool bOverwrite) {
	auto id = Info.Id;
	{
		FRWScopeLock lock(DownloadsLock, FRWScopeLockType::SLT_Write);
		if (Downloads.Contains(id)) {
			auto& v = Downloads[id];
			// force to restart task
			v->bIsAvailable = true;
			// update priority
			v->Priority = Priority;
			// add subscription if nessesary
			return TryAddSubscription(id);
		}
		// add a task to downloads collection
		auto& t = Downloads.Emplace(id, MakeShared<FChunkDownloadRequestWithTask>(
			Info,
			Priority,
			bOverwrite,
			FFileDownloadTask(FFileDownloadTaskRequest{id, Info.Url, Info.Path, bOverwrite})
		));
		// Downloads.Emplace(id, FChunkDownloadRequestWithTask{
		// 					  Info,
		// 					  Priority,
		// 					  bOverwrite,
		// 					  FFileDownloadTask(FFileDownloadTaskRequest{id, Info.Url, Info.Path, bOverwrite}),
		// 				  });
		t->DownloadTask.OnDownloadProgress().BindRaw(this, &FDownloadChunkSubsystem::DownloadProgressReceived);
		t->DownloadTask.OnDownloadChunk().BindRaw(this, &FDownloadChunkSubsystem::DownloadChunkReceived);
		t->DownloadTask.OnDownloadComplete().BindRaw(this, &FDownloadChunkSubsystem::DownloadCompleteReceived);
		t->DownloadTask.OnDownloadHeader().BindRaw(this, &FDownloadChunkSubsystem::DownloadHeaderReceived);
		t->DownloadTask.OnDownloadLink().BindRaw(this, &FDownloadChunkSubsystem::DownloadLinkReceived);
	}

	// emit new requested download
	OnRequest.Broadcast(Info);

	// Start ticking to update queues and delete completed tasks.
	StartTick();

	return TryAddSubscription(id);
}

bool FDownloadChunkSubsystem::TryRemove(const FFileDownloadInfo& Info) {
	TryRemove(Info.Id);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.SetSandboxEnabled(false);
	return PlatformFile.DeleteFile(*Info.Path);
}

TSharedPtr<FFileDownloadResult> FDownloadChunkSubsystem::Result(const FGuid& Id) {
	FRWScopeLock lock(DownloadsLock, FRWScopeLockType::SLT_ReadOnly);
	if (Downloads.Contains(Id)) {
		FFileDownloadResult result;
		if (Downloads[Id]->DownloadTask.TryGetResult(result)) {
			return MakeShared<FFileDownloadResult>(result);
		}
	}
	return TSharedPtr<FFileDownloadResult>{};
}

TSharedPtr<FFileDownloadTaskProgress> FDownloadChunkSubsystem::Progress(const FGuid& Id) {
	FRWScopeLock lock(DownloadsLock, SLT_ReadOnly);
	if (Downloads.Contains(Id)) {
		FFileDownloadTaskProgress Progress;
		Progress.Id = Id;
		Progress.fProgress = Downloads[Id]->DownloadTask.GetProgress();
		Progress.iTotalSize = Downloads[Id]->DownloadTask.GetTotalSize();
		return MakeShared<FFileDownloadTaskProgress>(Progress);
	}
	return TSharedPtr<FFileDownloadTaskProgress>{};
}

bool FDownloadChunkSubsystem::TryRemove(const FGuid& Id) {
	FFileDownloadTask* DownloadTask = nullptr;
	{
		FRWScopeLock lock(DownloadsLock, FRWScopeLockType::SLT_Write);
		if (Downloads.Contains(Id)) {
			DownloadTask = &Downloads[Id]->DownloadTask;
		}
		// erase version information on the package
		Versions.RemoveVersion(Id);
	}
	if (DownloadTask != nullptr) {
		DownloadTask->Stop();
	}
	{
		FRWScopeLock lock(DownloadsLock, FRWScopeLockType::SLT_Write);
		if (DownloadTask != nullptr) {
			Downloads.Remove(Id);
		}
	}
	return true;
}

bool FDownloadChunkSubsystem::TrySetPriority(const FGuid& Id, int32 Priority) {
	FRWScopeLock lock0(DownloadsLock, FRWScopeLockType::SLT_ReadOnly);
	FScopeLock lock1(&ActiveDownloadsLock);
	if (!Downloads.Contains(Id)) return false;

	Downloads[Id]->Priority = Priority;

	return true;
}

bool FDownloadChunkSubsystem::TryStop(const FGuid& Id) {
	FRWScopeLock lock0(DownloadsLock, FRWScopeLockType::SLT_ReadOnly);
	FScopeLock lock1(&ActiveDownloadsLock);
	if (!Downloads.Contains(Id)) return false;

	Downloads[Id]->DownloadTask.Stop();

	return true;
}

void FDownloadChunkSubsystem::GetFileHeaders(FString InUrl, TSharedRef<FOnFileHeaderRequestComplete> InCallback) {
	const auto Request = MakeShared<FFileHeaderRequest>();

	Request->Url = InUrl;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound(FFileHeaderResult(), false, TEXT("not processed"));
	}
}

FDownloadChunkSubsystem::~FDownloadChunkSubsystem() {
	FRWScopeLock Lock2(DownloadsLock, SLT_Write);
	CleanSubscriptions();
	// Stop all ongoing tasks
	for (auto& i : Downloads) {
		if (i.Value->DownloadTask.OnDownloadProgress().IsBound()) i.Value->DownloadTask.OnDownloadProgress().Unbind();
		if (i.Value->DownloadTask.OnDownloadChunk().IsBound()) i.Value->DownloadTask.OnDownloadChunk().Unbind();
		if (i.Value->DownloadTask.OnDownloadComplete().IsBound()) i.Value->DownloadTask.OnDownloadComplete().Unbind();
		i.Value->DownloadTask.Stop();
	}
	Downloads.Empty();
}

#pragma region VersionInfo
void FDownloadChunkSubsystem::VersionInfo::Update() {
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.SetSandboxEnabled(false);
	FString contents;
	bool bSuccess;
	{
		// do IO
		FScopeLock lock(&FileRWLock);
		bSuccess = FFileHelper::LoadFileToString(contents, &PlatformFile, *FileLocation());
	}
	if (bSuccess) {
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(contents);
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid()) {
			FRWScopeLock lock(VersionLock, FRWScopeLockType::SLT_Write);

			for (auto& i : JsonObject->Values) {
				FString value;
				FGuid guid;

				if (FGuid::Parse(i.Key, guid) && i.Value->TryGetString(value)) {
					if (Versions.Contains(guid)) {
						Versions[guid] = value;
					} else {
						Versions.Add(guid, value);
					}
				}
			}
		}
	}
	TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	{
		FRWScopeLock lock(VersionLock, FRWScopeLockType::SLT_ReadOnly);
		for (auto& i : Versions) {
			JsonObject->SetStringField(i.Key.ToString(), i.Value);
		}
	}

	contents.Empty();
	TArray<uint8> OutBytes;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&contents);
	if (FJsonSerializer::Serialize(JsonObject, JsonWriter.Get())) {
		const FTCHARToUTF8 StringConverter = FTCHARToUTF8(*contents);
		const char* Utf8RequestBodyJsonString = const_cast<ANSICHAR*>(StringConverter.Get());
		const int32 Utf8RequestBodyJsonStringLen = FCStringAnsi::Strlen(Utf8RequestBodyJsonString);

		OutBytes.SetNumUninitialized(Utf8RequestBodyJsonStringLen);
		for (int32 I = 0; I < Utf8RequestBodyJsonStringLen; I++) {
			OutBytes[I] = Utf8RequestBodyJsonString[I];
		}
		{
			// do IO
			FScopeLock lock(&FileRWLock);
			auto FileDirectory = FPaths::GetPath(FileLocation());
			FileDirectory = FPaths::ConvertRelativePathToFull(FileDirectory);
			if (!PlatformFile.DirectoryExists(*FileDirectory)) {
				if (!PlatformFile.CreateDirectoryTree(*FileDirectory)) {
					LogWarningF("failed to create directory");
				}
			}
			auto f = PlatformFile.OpenWrite(*FileLocation(), false, false);
			if (f) {
				f->Write(OutBytes.GetData(), OutBytes.Num());
				delete f;
			} else {
				LogWarningF("Unable to save pak versions");
			}
		}
	}
	IsUpdated = true;
}

bool FDownloadChunkSubsystem::VersionInfo::TrySetVersion(const FGuid& Id, const FString& Version) {
	// lazy initialization (concurrence
	if (!IsUpdated) Update();
	{
		FRWScopeLock lock(VersionLock, FRWScopeLockType::SLT_Write);
		if (Versions.Contains(Id)) {
			// check if version was changed
			if (Versions[Id] == Version) return false;
			Versions[Id] = Version;
		} else {
			Versions.Add(Id, Version);
		}
	}
	Update();
	return true;
}

/** Removes version controll */
void FDownloadChunkSubsystem::VersionInfo::RemoveVersion(const FGuid& Id) {
	if (!IsUpdated) Update();
	{
		FRWScopeLock lock(VersionLock, FRWScopeLockType::SLT_Write);
		Versions.Remove(Id);
	}
}
#pragma endregion

#pragma region Subscriptions
#pragma region FSubscription
FDownloadChunkSubsystem::FSubscription::FSubscription(const FGuid& Id, FDownloadChunkSubsystem* Subsystem)
	: Subsystem{Subsystem}, Id{Id} {
	OnComplete = MakeShared<FDownloadChunkSubsystemOnCompleteCallback>();
	OnProgress = MakeShared<FDownloadChunkSubsystemOnProgressCallback>();
}

bool FDownloadChunkSubsystem::FSubscription::operator ==(const FSubscription& other) const {
	return OnComplete == other.OnComplete && OnProgress == other.OnProgress;
}

void FDownloadChunkSubsystem::FSubscription::Destroy() {
	// possible race condition, not threadsafe method (calling from different threads is unlikely)
	if (Subsystem == nullptr) return;
	if (OnComplete.IsValid() && OnComplete->IsBound()) OnComplete->Unbind();
	if (OnProgress.IsValid() && OnProgress->IsBound()) OnProgress->Unbind();
	Subsystem = nullptr;
}

FDownloadChunkSubsystem::FSubscription::~FSubscription() {
	Destroy();
}
#pragma endregion

bool FDownloadChunkSubsystem::TryRemoveSubscription(const FSubscription& sub) {
	if (!Subscriptions.Contains(sub.Id)) return false;
	auto& subs = Subscriptions[sub.Id];
	int target = -1;
	for (int i = 0; i < subs.Num(); ++i) {
		if (sub == *subs[i]) target = i;
	}
	if (target == -1) return false;
	subs.RemoveAtSwap(target);
	return true;
}

void FDownloadChunkSubsystem::CleanSubscriptions() {
	FRWScopeLock lock(SubscriptionsLock, FRWScopeLockType::SLT_Write);
	Subscriptions.Empty();
}

FDownloadSubscriptionPtr FDownloadChunkSubsystem::TryAddSubscription(FGuid Id) {
	FRWScopeLock lock(SubscriptionsLock, FRWScopeLockType::SLT_Write);

	auto& subs = Subscriptions.FindOrAdd(Id);
	auto i = subs.Add(MakeShared<FSubscription>(Id, this));

	return MakeShared<FDownloadSubscription>(subs[i]);
}
#pragma endregion

void FDownloadChunkSubsystem::DownloadProgressReceived(const FFileDownloadTaskProgress& Progress) {
	TArray<TSharedPtr<FSubscription>> subs{};
	{
		FRWScopeLock lock(SubscriptionsLock, SLT_ReadOnly);
		if (Subscriptions.Contains(Progress.Id)) {
			subs = Subscriptions[Progress.Id];
		}
	}
	// iterating through subs (changes at subs are ignored)
	for (TSharedPtr<FSubscription> i : subs) {
		if (i) {
			i->OnProgress->ExecuteIfBound(Progress);
		}
	}

	OnProgress.Broadcast(Progress);
}

void FDownloadChunkSubsystem::SetAvailable(const FGuid& Id) {
	FRWScopeLock lock0(DownloadsLock, FRWScopeLockType::SLT_ReadOnly);
	FScopeLock lock1(&ActiveDownloadsLock);
	Downloads[Id]->bIsAvailable = true;
}

void FDownloadChunkSubsystem::DownloadChunkReceived(const FFileDownloadTaskProgress& Progress) {
	FPlatformAtomics::InterlockedDecrement(&iActiveDownloads);
	// restore task's availibility
	SetAvailable(Progress.Id);
	Tick(0.f);
}

void FDownloadChunkSubsystem::DownloadCompleteReceived(const FFileDownloadTaskResult& Result) {
	TArray<TSharedPtr<FSubscription>> subs{};
	{
		FRWScopeLock lock(SubscriptionsLock, FRWScopeLockType::SLT_Write);

		if (Subscriptions.Contains(Result.Id)) {
			subs = Subscriptions[Result.Id];
		}
	}
	// iterating through subs (changes at subs are ignored, new added subs force 
	// task to restart, so technically they are bound to next task result)
	for (auto& i : subs) {
		if (i) {
			if (i->OnComplete->IsBound()) {
				i->OnComplete->Execute(Result);
			}
		}
	}

	OnComplete.Broadcast(Result);
	{
		// remove all subscribers, all received what they needed
		FRWScopeLock lock2(SubscriptionsLock, FRWScopeLockType::SLT_Write);

		// remove destroyed subscriptions
		for (auto& i : subs) {
			if (i->IsDestroyed()) TryRemoveSubscription(*i);
		}

		if (Subscriptions.Contains(Result.Id)) {
			auto& v = Subscriptions[Result.Id];
			if (v.Num() == 0) Subscriptions.Remove(Result.Id);
		}
	}
}

void FDownloadChunkSubsystem::DownloadLinkReceived(const FFileDownloadLink& Link) {
	if (Link.bSuccessful) {
		VeLogVerbose("download link received");
		return;
	}

	VeLogError("failed to get a file download link: %s", *Link.Error);
}

bool FDownloadChunkSubsystem::DownloadHeaderReceived(const FFileDownloadTaskHeader& Header) {
	if (Header.bOverwrite) {
		// if overwriting was forced, ignore the result of version update
		Versions.TrySetVersion(Header.Id, Header.Version);
		return true;
	}
	// If version is changed force to restart download
	return Versions.TrySetVersion(Header.Id, Header.Version);
}

void FDownloadChunkSubsystem::Initialize() {}

void FDownloadChunkSubsystem::Shutdown() {
	StopTick();
}

bool FDownloadChunkSubsystem::Tick(float _DeltaTime) {
	while (iActiveDownloads < MaxParallelTasks) {
		// pick not completed task with maximum priority
		FRWScopeLock lock0(DownloadsLock, FRWScopeLockType::SLT_ReadOnly);
		FChunkDownloadRequestWithTask* mx = nullptr;
		{
			FScopeLock lock1(&ActiveDownloadsLock);
			for (auto& i : Downloads) {
				// check if 
				if (!i.Value->bIsAvailable) continue;
				if (mx == nullptr) {
					mx = &i.Value.Get();
					continue;
				}
				if (mx->Priority == FDownloadChunkSubsystem::HighestPriority) break;
				if (mx->Priority < i.Value->Priority) {
					mx = &i.Value.Get();
				}
			}
			if (mx == nullptr) {
				// all tasks are done
				return true;
			}
			// has some job to do
			mx->bIsAvailable = false;

			// activate task
			FPlatformAtomics::InterlockedIncrement(&iActiveDownloads);
		}
		if (mx->DownloadTask.RequestNextSegment()) {
			// task was wrongly activated, try again
			FPlatformAtomics::InterlockedDecrement(&iActiveDownloads);

			// download completed, may remove task 
			// (should i? because list of downloads is small, and will be cleaned on restart)
			/*
			{
				FRWScopeLock lock2(DownloadsLock, FRWScopeLockType::SLT_Write);
				Downloads.Remove(mx->Request.Id);
			}
			*/

			continue;
		}
		// job's started
		break;
	}

	return true;
}

void FDownloadChunkSubsystem::StartTick() {
	if (!TickDelegateHandle.IsValid()) {
		TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FDownloadChunkSubsystem::Tick), TickInterval);
	}
}

void FDownloadChunkSubsystem::StopTick() {
	if (TickDelegateHandle.IsValid()) {
		FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
		TickDelegateHandle.Reset();
	}
}
