// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "HttpModule.h"

#pragma region Http constants
namespace Http { namespace Code {
		static constexpr int32 OK = 200;
		static constexpr int32 Created = 201;
		static constexpr int32 PartialContent = 206;
		static constexpr int32 Forbidden = 403;
	}

	namespace Method {
		static const TCHAR* Get = TEXT("GET");
		static const TCHAR* Head = TEXT("HEAD");
	}

	namespace Header {
		static const TCHAR* Range = TEXT("Range");
		static const TCHAR* ContentLength = TEXT("Content-Length");
		static const TCHAR* ContentRange = TEXT("Content-Range");
		static const TCHAR* AcceptRanges = TEXT("Accept-Ranges");
		static const TCHAR* IfNoneMatch = TEXT("If-None-Match");
		static const TCHAR* ETag = TEXT("ETag");
	}}
#pragma endregion

namespace Errors {
	/** Request error string */
	const static FString FailedToRequestNextChunk = FString(TEXT("failed to request the first file chunk"));
	const static FString FailedToWriteToFile = FString(TEXT("failed to write a chunk to the file"));
	const static FString FailedToOpenFile = FString(TEXT("failed to open the file for writing"));
	const static FString ChunkResponseError = FString(TEXT("download chunk response error: %s"));
	const static FString FailedToDeleteFile = FString(TEXT("failed to delete the target file"));
	const static FString FailedLengthRequest = FString(TEXT("failed to make file content length request"));
	const static FString Cancelled = FString(TEXT("task was cancelled"));

	const static FString PreSignedUrlResponseError = FString(TEXT("request download url error: %s"));
}

namespace Le7elDownloadJsonFields {
	const auto Detail = TEXT("detail");
}

struct FFileDownloadTaskRequest {
	/** Unique identifier. */
	FGuid Id;

	/** Source URL. */
	FString Url;

	/** Downloaded file path. */
	FString Path;

	/** Force overwrite */
	bool bOverwrite;
};

struct FFileDownloadTaskResult {
	/** Unique identifier. */
	FGuid Id;

	/** Success flag. */
	bool bSuccessful;

	/** Error string for cases when the task was not successful. */
	FString Error;

	/** URL of the downloaded file. */
	FString Url;

	/** Path to the downloaded file. */
	FString Path;
};

struct FFileDownloadLink {
	/** Prepared download URL. */
	FString URL;

	/** Success flag. */
	bool bSuccessful;

	/** Error string for cases when the task was not successful. */
	FString Error;
};

struct FFileDownloadTaskProgress {
	/** Unique identifier. */
	FGuid Id;

	/** Total file size. */
	int64 iTotalSize;

	/** Whole file download percentage. */
	float fProgress;
};

struct FFileDownloadTaskHeader {
	/** Unique identifier. */
	FGuid Id;

	/** Version identifier of file */
	FString Version;

	/** Was forced to overwrite */
	bool bOverwrite;
};

enum class ESegmentResult {
	Complete,
	Processing,
	NeedMore,
};

DECLARE_DELEGATE_OneParam(FFileDownloadTaskOnProgressCallback, const FFileDownloadTaskProgress& /* Progress */);
DECLARE_DELEGATE_OneParam(FFileDownloadTaskOnCompleteCallback, const FFileDownloadTaskResult& /* Result */);
DECLARE_DELEGATE_OneParam(FFileDownloadTaskOnLinkCallback, const FFileDownloadLink& /* Link */);
DECLARE_DELEGATE_RetVal_OneParam(bool, FFileDownloadTaskOnHeaderCallback, const FFileDownloadTaskHeader& /* Header */);

class FFileDownloadTask {
	FString VersionHeader = TEXT("ETag");
	float fProgressThreshold = 0.01f;

	volatile float fProgress = 0.f;

	int64 lCurrentChunk;
	int64 lTotalChunks;
	int64 lTotalSize = -1;
	int64 lMaxChunkSize;
	int64 lChunkOffset = 0;

#pragma region Pre-signed download URL
	FString PreSignedUrl = {};
	FHttpRequestPtr PreSignedUrlHttpRequest = nullptr;
	void PreSignedUrlHttpRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccess);
	FFileDownloadTaskOnLinkCallback OnLink;
#pragma endregion

	volatile bool bProgressing = false;
	volatile bool bCompleted = false;
	FFileDownloadTaskRequest Request;
	FFileDownloadTaskResult Result;
	FHttpRequestPtr HttpRequest;
	FFileDownloadTaskOnProgressCallback OnProgress;
	FFileDownloadTaskOnProgressCallback OnChunk;
	FFileDownloadTaskOnCompleteCallback OnComplete;
	FFileDownloadTaskOnHeaderCallback OnHeader;

	void HttpRequestProgress(FHttpRequestPtr httpRequest, int32 bytesSend, int32 inBytesReceived);
	void HttpRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccess);
	void SetRangeHeader(const int64 Start, const int64 End);
	void Complete(bool bSuccessful, const FString& Error);
	FString GetDetailMessage(const FString& ResponseMessasge) const;
public:
	FFileDownloadTask(const FFileDownloadTaskRequest& request);
	FFileDownloadTask(FFileDownloadTask&& other);
	FFileDownloadTask& operator=(FFileDownloadTask&& other);

	/** ticks progress every http download tick with percentage threshold */
	FFileDownloadTaskOnProgressCallback& OnDownloadProgress() { return OnProgress; }
	/** returns even if task completed successfully or not */
	FFileDownloadTaskOnProgressCallback& OnDownloadChunk() { return OnChunk; }
	/** returns when task completed or cancelled */
	FFileDownloadTaskOnCompleteCallback& OnDownloadComplete() { return OnComplete; }
	/** returns when task completed header request */
	FFileDownloadTaskOnHeaderCallback& OnDownloadHeader() { return OnHeader; }
	/** returns when task completed link request */
	FFileDownloadTaskOnLinkCallback& OnDownloadLink() { return OnLink; }

	/** Requests next chunk if available, returns true when downloading is complete */
	bool RequestNextSegment();
	bool IsCompleted() const { return bCompleted; }

	bool TryGetResult(FFileDownloadTaskResult& result) const {
		result = Result;
		return bCompleted;
	}

	float GetProgress() const {
		if (lTotalSize < 0) return 0.f;
		return fProgress;
	}

	int64 GetTotalSize() const {
		return lTotalSize;
	}

	void Stop();
	~FFileDownloadTask();
};
