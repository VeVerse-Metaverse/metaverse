// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeShared.h"
#include "ManagedTexture.h"
#include "TextureLoadRequest.h"
#include "TextureLoadTask.h"

class VEDOWNLOAD_API FDownloadTextureSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/** Used to request the asynchronous texture loading. Returns true if successfully queued or resolved the texture. */
	bool LoadTexture(TSharedPtr<FTextureLoadRequest> InRequest);

private:
#pragma region Tasks
	/** Queue of the async tasks to be processed. */
	TArray<TSharedPtr<FTextureLoadTask>> PendingTasks;

	/** Queue of the async tasks currently processed. */
	TArray<TSharedPtr<FTextureLoadTask>> ProcessingTasks;

	/** How many parallel download tasks are allowed. */
	const int32 MaxParallelTasks = 12;

	/** Tick interval. */
	const float TickInterval = 1.61803398875f / 3.1415926f;

	void OnTaskCompleted(const FTextureLoadResult& Result);

	/** Enqueue the next pending task for processing. */
	void StartNextTaskProcessing();
#pragma endregion

#pragma region Texture Cache
	/** Cached textures. Accessed using RequestTexture method. */
	TMap<FString, TSharedPtr<FManagedTexture>> CachedTextures;
	FCriticalSection CachedTexturesLock;

	TMap<FString, IManagedTextureRequester*> PendingRemoveReferenceList;
	TArray<IManagedTextureRequester*> PendingRemoveAllReferencesList;

	/** Adds a new texture to the cache. */
	void AddCachedTexture(TSharedPtr<FManagedTexture> InTexture);

public:
	DECLARE_EVENT_OneParam(FTextureSubsystem, FTextureDestroyed, const FString& /* Url */);

	FTextureDestroyed& GetOnTextureDestroyed() { return OnTextureDestroyed; }

	/** Registers the object as the texture user. */
	void AddObjectReference(const FString& InUrl, IManagedTextureRequester* InReference);

	/** Removes all references to the object deleting textures if required. */
	void RemoveAllObjectReferences(IManagedTextureRequester* InReference);

	/** Removes a single texture object reference deleting the texture if required. */
	void RemoveObjectReference(const FString& InUrl, IManagedTextureRequester* InReference);

private:
	FTextureDestroyed OnTextureDestroyed;

	/** Returns cached texture by URL if it exists or nullptr otherwise. */
	TSharedPtr<FManagedTexture> GetCachedTexture(const FString& InUrl);

	/** Clears all cached textures. */
	void ClearCachedTextures();

	/** Process scheduled object reference cleanup. */
	void ProcessRemoveObjectReferences();

	/** Process scheduled deleted object reference cleanup. */
	void ProcessRemoveAllObjectReferences();
#pragma endregion

#pragma region Tick
	/** Ticker delegate. */
	FTSTicker::FDelegateHandle TickDelegateHandle;

	/** Updates asynchronous tasks. */
	bool Tick(float DeltaSeconds);

	/** Start ticking to update async jobs. */
	void StartTick();

	/** Stop ticking when no active or pending async jobs left. */
	void StopTick();
#pragma endregion

#pragma region Cleanup
	/** Tick interval. */
	const float CleanupTickInterval = 1.61803398875f * 3.1415926f * 2.718281828459f;

	/** Ticker delegate. */
	FTSTicker::FDelegateHandle CleanupTickDelegateHandle;
#pragma endregion
};
