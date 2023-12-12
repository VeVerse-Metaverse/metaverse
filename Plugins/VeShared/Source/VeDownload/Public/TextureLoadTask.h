// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
#include "TextureLoadAsyncTask.h"

/** Used to queue texture load requests. */
class FTextureLoadTask {
public:
	// Do not allow to copy the task as it can possibly mess up delegates and async task.
	FTextureLoadTask(const FTextureLoadTask&) = delete;

	// The mainly used constructor accepting texture loading request.
	explicit FTextureLoadTask(TSharedPtr<FTextureLoadRequest> InRequest);

	~FTextureLoadTask();

	void StartBackgroundTask();

	/** Checks if the asynchronous task is complete. */
	bool IsComplete() const;

	FString GetUrl() const;

	TSharedPtr<FTextureLoadRequest> GetRequest() const;

	TSharedPtr<FTextureLoadResult> GetResult() const;

	FORCEINLINE bool friend operator==(const FTextureLoadTask& Lhs, const FTextureLoadTask& Rhs) {
		return Lhs.Request.Get() == Rhs.Request.Get();
	}

	FORCEINLINE bool friend operator!=(const FTextureLoadTask& Lhs, const FTextureLoadTask& Rhs) {
		return Lhs.Request.Get() != Rhs.Request.Get();
	}

	DECLARE_EVENT_OneParam(FTextureLoadTask, FTextureLoadingTaskComplete, const FTextureLoadResult& /* Result */)

	/** Get multicast task complete delegate. */
	FTextureLoadingTaskComplete& GetOnComplete() { return OnComplete; }

private:
	/** On complete multicast delegate. */
	FTextureLoadingTaskComplete OnComplete;

	/** Internal request. */
	TSharedPtr<FTextureLoadRequest> Request = nullptr;

	/** Result of the completed task. */
	TSharedPtr<FTextureLoadResult> Result = nullptr;

	/** Actual async task, not null if active. Delete to stop the task and free memory. */
	FAsyncTask<FTextureLoadAsyncTask>* AsyncTask;

	/** Callback for asynchronous task complete delegate. */
	void OnAsyncTaskComplete(const FTextureLoadResult& InResult);
};
