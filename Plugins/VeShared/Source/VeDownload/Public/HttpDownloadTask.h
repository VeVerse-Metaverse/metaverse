// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "HttpDownloadTypes.h"
#include "StorageItemId.h"
#include "Interfaces/IHttpRequest.h"

/** Asynchronous download task managed by the FVeDownload object. */
class FHttpDownloadTask : public TSharedFromThis<FHttpDownloadTask> {

	typedef TMulticastDelegate<void(FHttpDownloadTask* InDownloadTask, const FHttpDownloadProgressMetadata& InMetadata)> FOnHttpDownloadTaskProgress;
	typedef TMulticastDelegate<void(FHttpDownloadTask* InDownloadTask, const FHttpDownloadResultMetadata& InMetadata)> FOnHttpDownloadTaskComplete;

public:
	/** Main constructor */
	explicit FHttpDownloadTask(const FStorageItemId& InStorageItemId);
	~FHttpDownloadTask();

	const FStorageItemId& GetStorageItemId() const { return StorageItemId; }
	const FHttpDownloadTaskOptions& GetProcessOptions() const { return ProcessOptions; }

	/** Starts the task processing. */
	void Process(const FHttpDownloadTaskOptions& InOptions);

	/** Cancel the download task */
	void Cancel(bool DeleteCache = false);

protected:
	FString Original_ContentFilename;
	FString Cache_MetadataFilename;
	FString Cache_ContentFilename;

	void RequestPreSigned(const FString& PreSignedUrl);
	void RequestContent(const FString& Url);

	void HttpRequest_OnHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue);
	void HttpRequest_OnRequestProgress(FHttpRequestPtr InRequest, const int32 BytesSent, const int32 BytesReceived);
	void HttpRequest_OnProcessRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful);

	void NativeOnHeaders(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const TMap<FString, FString>& InHeaders);
	void NativeOnContent(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful);
	void NativeOnProgress(int64 ContentLength, int64 DownloadedSize, int64 BytesReceived);
	void NativeOnComplete(int64 ContentLength, const FString& Filename);
	void NativeOnCancel(const FString&& Message, bool InDeleteCache = false);
	void NativeOnError(const FString&& Message);

	/** Get API URL for the pre-signed download link request. */
	FString GetPreSignedUrl(const FStorageItemId& InStorageId) const;

	/** Clears the previous HTTP header and initializes the new one for the current chunk */
	void HttpRequest_Reset();
	void HttpRequest_Stop();

	void DeleteCacheFiles() const;
	bool RenameCacheToOriginal() const;
	static bool RenameFile(const TCHAR* To, const TCHAR* From);

	void LogHttpResponse(FHttpResponsePtr InResponse) const;

	static FString GetResponseContent(FHttpResponsePtr InResponse);

private:
	FStorageItemId StorageItemId;
	FHttpDownloadTaskOptions ProcessOptions;

	/** HTTP request */
	FHttpRequestPtr HttpRequest;

	FHttpDownloadMetadata Session_DownloadMetadata;
	FString Session_ContentUrl;
	int64 Session_BytesReceived_Start = 0;
	int64 Session_BlockOffset = 0;
	int64 Session_BlockSize = 0;
	int32 Session_RestartNum = 0;
	int64 Session_BytesReceived = 0;
	void ResetSession();

	/** Max block size: 1 << 29 B = 512 MiB. */
	/** Max block size: 1 << 28 B = 256 MiB. */
	int64 BlockSize = 1 << 28;
	// int64 BlockSize = 30'000'000;

	bool bLogHttpContent = false;

#pragma region HttpRequest
	bool HttpRequest_HeaderComplete = false;
	TMap<FString, FString> HttpRequest_Headers;
	FHttpDownloadMetadata HttpRequest_DownloadMetadata;
#pragma endregion HttpRequest

#pragma region Time

	class FTimer {
	public:
		void Start();
		double GetSeconds() const;
		bool IsStarted() const { return Session_TimerStarted; }

	private:
		FDateTime Session_StartTime;
		bool Session_TimerStarted = false;

	} Timer;

	class FProgress {
	public:
		void Start(int64 DownloadedSize);
		int64 GetSpeed(int64 DownloadedSize);
		bool IsTimeToProgress();

	private:
		int64 PrevDownloadedSize = 0;
		FDateTime PrevDownloadedTime;
		FDateTime ProgressTime;

	} Progress;

#pragma endregion

#pragma region Events

public:
	/** Download progress callback */
	FOnHttpDownloadTaskProgress& GetOnProgress() { return OnProgress; }

	/** Download complete callback */
	FOnHttpDownloadTaskComplete& GetOnComplete() { return OnComplete; }

private:
	/** Download progress callback */
	FOnHttpDownloadTaskProgress OnProgress;
	/** Download complete callback */
	FOnHttpDownloadTaskComplete OnComplete;

#pragma endregion Events

public:
	static FString PrintfSeparator(int64 Value);
};


inline void FHttpDownloadTask::FTimer::Start() {
	if (Session_TimerStarted) {
		return;
	}
	Session_TimerStarted = true;
	Session_StartTime = FDateTime::UtcNow();
}

inline double FHttpDownloadTask::FTimer::GetSeconds() const {
	if (!Session_TimerStarted) {
		return 0.0f;
	}
	return (FDateTime::UtcNow() - Session_StartTime).GetTotalMilliseconds() / 1000.f;
}

inline void FHttpDownloadTask::FProgress::Start(int64 DownloadedSize) {
	ProgressTime = FDateTime::UtcNow();
	PrevDownloadedTime = ProgressTime;
	PrevDownloadedSize = DownloadedSize;
}

inline int64 FHttpDownloadTask::FProgress::GetSpeed(int64 DownloadedSize) {
	const auto Now = FDateTime::UtcNow();

	const double DeltaTime = (Now - PrevDownloadedTime).GetTotalMilliseconds() / 1000.f;
	const double DeltaSize = static_cast<double>(DownloadedSize - PrevDownloadedSize);
	PrevDownloadedTime = Now;
	PrevDownloadedSize = DownloadedSize;

	return DeltaSize / DeltaTime;
}

inline bool FHttpDownloadTask::FProgress::IsTimeToProgress() {
	if (/*!Session_TimerStarted ||*/ (FDateTime::UtcNow() - ProgressTime).GetTotalMilliseconds() < 2000.f) {
		return false;
	}
	ProgressTime = FDateTime::UtcNow();
	return true;
}
