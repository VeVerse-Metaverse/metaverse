// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Download1Metadata.h"
#include "Interfaces/IHttpRequest.h"

/** Asynchronous download task managed by the FVeDownload object. */
class FVeDownload1Task {
	/** Content length to download. */
	int64 ContentLength = -1;
	/** Already downloaded content length. */
	int64 DownloadedContentLength = 0;

#pragma region Progress
	/** Progress ratio. */
	float Progress = 0.0f;
	/** Threshold to update progress (1%). */
	float ProgressThreshold = 0.01f;
#pragma endregion

#pragma region Time
	/** Task start time. */
	float StartTime = 0.0f;
	/** Task completion time. */
	float CompleteTime = 0.0f;
#pragma endregion

#pragma region Range Download
	/** Server supports range requests. */
	bool bRangeHeaderSupported = false;
	/** Using chunk downloads (server supports it and a file is big enough). */
	bool bUsingRangeHeader = false;
	/** Total block number. */
	int64 BlockNum = 0;
	/** Max block size: 1 << 29 B = 512 MiB. */
	int64 BlockSize = 1 << 29;
	/** Current block index. */
	int64 BlockIndex = -1;
	/** Current block offset. */
	int64 BlockOffset = 0;
#pragma endregion

public:
	/** Main constructor */
	explicit FVeDownload1Task(const FDownload1RequestMetadata& InRequestMetadata);

private:
	/** Download progress callback */
	FOnDownload1ProgressCallback DownloadTaskProgressCallback;
	/** Download complete callback */
	FOnDownload1CompleteCallback DownloadTaskCompleteCallback;
	/** Download chunk callback */
	FOnDownload1ProgressCallback DownloadTaskBlockCallback;

public:
	/** Download progress callback */
	FOnDownload1ProgressCallback& GetOnDownloadProgress() {
		return DownloadTaskProgressCallback;
	}

	/** Download complete callback */
	FOnDownload1CompleteCallback& GetOnDownloadComplete() {
		return DownloadTaskCompleteCallback;
	}

	/** Download chunk callback */
	FOnDownload1ProgressCallback& GetOnDownloadChunk() {
		return DownloadTaskBlockCallback;
	}

private:
	/** Is in progress */
	bool bProcessing = false;
	/** Has been completed */
	bool bCompleted = false;

	/** Original request metadata */
	FDownload1RequestMetadata RequestMetadata;
	/** Result metadata */
	FDownload1ResultMetadata ResultMetadata;

	/** Signature HTTP request for protected files */
	FHttpRequestPtr SignedUrlHttpRequest;
	/** HTTP request */
	FHttpRequestPtr HttpRequest;

public:
	/** Starts the task processing. */
	EDownload1Code Process();
	/** Cancel the download task */
	void Cancel(bool bDeleteFile = false);

private:
	/** Clears the previous HTTP header and initializes the new one for the current chunk */
	void ResetHttpRequest();
	/** Set range header for the next chunk */
	void SetRangeHeader(const int64 Start, const int64 End) const;
	/** Complete the task and call required delegates */
	void Complete(EDownload1Code Code, const FString& InError);

	/** HTTP request progress callback */
	void HttpContentRequestProgress(FHttpRequestPtr InRequest, int32 BytesSent, int32 BytesReceived);

	/** Set up the API authorization headers for the pre-signed download link request. */
	bool SetPreSignedLinkAuthorizationHeaders() const;
	/** Set up the API URL for the pre-signed download link request. */
	bool SetPreSignedLinkRequestUrl() const;
	/** Request the pre-signed download link from the API. */
	EDownload1Code RequestPreSignedDownloadLink();
	/** Callback for pre-signed download link response. */
	void HttpPreSignedUrlRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful);

	/** Request the file headers. */
	EDownload1Code RequestHead();
	/** Callback for the head request. */
	void HttpHeadRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful);

	/** Request the full file content at once. */
	EDownload1Code RequestContent();
	/** Callback for the file content request. */
	void HttpContentRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful);

	/** Request the next file chunk. */
	EDownload1Code RequestContentBlock();
	/** Callback for the file chunk request. */
	void HttpContentBlockRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful);
};
