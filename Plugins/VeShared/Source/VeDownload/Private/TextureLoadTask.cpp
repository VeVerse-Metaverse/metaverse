// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "TextureLoadTask.h"

#include "VeShared.h"

FTextureLoadTask::FTextureLoadTask(const TSharedPtr<FTextureLoadRequest> InRequest): AsyncTask(nullptr) {
	Request = InRequest;

	// LogVerboseF("ctor for %s", *InRequest->Url);
}

FTextureLoadTask::~FTextureLoadTask() {
	if (AsyncTask && AsyncTask->Cancel()) {
		delete AsyncTask;
		AsyncTask = nullptr;
	}
}

void FTextureLoadTask::StartBackgroundTask() {
	// todo: find out how to run the task on the background rendering thread
	AsyncTask = new FAsyncTask<FTextureLoadAsyncTask>(Request);

	if (AsyncTask) {
		AsyncTask->GetTask().GetOnComplete().BindRaw(this, &FTextureLoadTask::OnAsyncTaskComplete);

		if (AsyncTask) {
			AsyncTask->StartBackgroundTask();
		}
	} else {
		LogErrorF("failed to create an async task");
	}
}

bool FTextureLoadTask::IsComplete() const {
	// LogVerboseF("is complete check");
	if (AsyncTask) {
		return AsyncTask->IsDone();
	}
	return true;
}

FString FTextureLoadTask::GetUrl() const {
	if (Request) {
		// LogVerboseF("url %s", *Request->Url);
		return Request->Url;
	}

	LogWarningF("empty URL");

	return "";
}

TSharedPtr<FTextureLoadRequest> FTextureLoadTask::GetRequest() const {
	return Request;
}

TSharedPtr<FTextureLoadResult> FTextureLoadTask::GetResult() const {
	return Result;
}

void FTextureLoadTask::OnAsyncTaskComplete(const FTextureLoadResult& InResult) {
	// LogVerboseF("async task complete");

	// Notify that the task has been completed.
	if (OnComplete.IsBound()) {
		OnComplete.Broadcast(InResult);
	}

	// Notify that the request has been completed.
	if (Request->GetOnComplete().IsBound()) {
		Request->GetOnComplete().Execute(InResult);
	}

	// Clear async task.
	if (AsyncTask && AsyncTask->IsDone()) {
		// LogVerboseF("async task is done, deleting");
		delete AsyncTask;
		AsyncTask = nullptr;
	}
}
