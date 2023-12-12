// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "TextureLoadRequest.h"
#include "Interfaces/IHttpRequest.h"
#include "Async/AsyncWork.h"

/** One-to-one task complete delegate to bind outer scheduled tasks and the async tasks. */
DECLARE_DELEGATE_OneParam(FTextureLoadingAsyncTaskComplete, const FTextureLoadResult& /* Result */)

/** Task for asynchronous texture loading. Loads file from the disk cache if available or from the internet using URL. */
class FTextureLoadAsyncTask {
	friend class FAsyncTask<FTextureLoadAsyncTask>;

	explicit FTextureLoadAsyncTask(const TSharedPtr<FTextureLoadRequest> InRequest);

	~FTextureLoadAsyncTask();

	// ReSharper disable once CppMemberFunctionMayBeStatic
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(FTextureLoadAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

public:
	bool CanAbandon();
	void Abandon();

	void DoWork();

	FTextureLoadingAsyncTaskComplete& GetOnComplete() { return OnComplete; }

	/** Texture load request. */
	TSharedPtr<FTextureLoadRequest> Request;
private:
	bool bShouldAbandon = false;
	FHttpRequestPtr PreSignedUrlHttpRequest = nullptr;
	FString PreSignedUrl = {};

	UTexture* Texture;

	/** Async task complete callback to communicate with the subsystem. */
	FTextureLoadingAsyncTaskComplete OnComplete;

	/** Texture load result. */
	FTextureLoadResult Result;

	/** Synchronization event to control the async thread execution. */
	FEvent* AsyncTaskCompleteSyncEvent;

	/** Event to fire when download is complete. */
	FEvent* DownloadCompleteEvent;

	/** Event to fire when pre-signed URL request is complete. */
	FEvent* PreSignedUrlRequestCompleteEvent;

	/** HTTP request. */
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = nullptr;
	
	/** HTTP response if downloaded from the Web. */
	FHttpResponsePtr HttpResponse;

	/** Generates mipmaps for the created texture. Note that the first texture mip should be filled with the image bytes when this method is called. */
	void GenerateMipmaps() const;

	void ExecuteCompleteDelegate();

	void PreSignedUrlHttpRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccess);

	/** Download request complete callback. Fired when all data is finished loading or interrupted by any error. */
	void OnDownloadRequestComplete(FHttpRequestPtr InHttpRequest, FHttpResponsePtr InHttpResponse, bool bSucceeded);

	/** Download request progress callback. Fired each time when the next chunk of bytes is received. */
	void OnDownloadRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 InBytesReceived);
};
