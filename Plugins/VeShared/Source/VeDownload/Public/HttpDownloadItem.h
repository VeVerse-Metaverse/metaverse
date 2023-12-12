// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "HttpDownloadTypes.h"

class FDownloadHttpDownloadSubsystem;
class FHttpDownloadTask;
class FCacheFile;
class FStorageItemId;


/**
 * Scheduled download.
 * @details Manages internal asynchronous download task and provides callback interface for requesters. Safe to store in containers using smart pointers. Uses chunked download to download big files if server supports byte range requests.
 * @note Can have issues downloading files bigger than 2GiB using current UE HTTP implementation due to 31-bit TArray indexation.
 */
class VEDOWNLOAD_API FHttpDownloadItem : public TSharedFromThis<FHttpDownloadItem> {
	friend FDownloadHttpDownloadSubsystem;

	DECLARE_EVENT_TwoParams(FHttpDownloadItem, FOnHttpDownloadItemProgress, FHttpDownloadItem* InDownloadItem, const FHttpDownloadProgressMetadata& InMetadata);
	DECLARE_EVENT_TwoParams(FHttpDownloadItem, FOnHttpDownloadItemComplete, FHttpDownloadItem* InDownloadItem, const FHttpDownloadResultMetadata& InMetadata);

public:
	FHttpDownloadItem(FDownloadHttpDownloadSubsystem* InOwner, const FStorageItemId& InStorageItemId);
	~FHttpDownloadItem();

	const FString& GetId() const;
	const FHttpDownloadItemOptions& GetOptions() const { return ProcessOptions; }
	const FStorageItemId& GetStorageItemId() const;

	bool IsProcessing() const { return bIsProcessing; }

	void Cancel(bool bDeleteCacheFile = false) const;
	void Process(const FHttpDownloadItemOptions& InOptions);

private:
	FDownloadHttpDownloadSubsystem* Owner;
	TSharedRef<FHttpDownloadTask> HttpDownloadTask;
	FHttpDownloadItemOptions ProcessOptions;
	bool bIsProcessing = false;

	/** Called from FDownloadHttpDownloadSubsystem */
	void ActivateProcess();

	void HttpTask_OnProgress(FHttpDownloadTask* InDownloadTask, const FHttpDownloadProgressMetadata& InMetadata);
	void HttpTask_OnComplete(FHttpDownloadTask* InDownloadTask, const FHttpDownloadResultMetadata& InMetadata);

#pragma region Events

public:
	FOnHttpDownloadItemProgress& GetOnProgress() { return OnProgress; }
	FOnHttpDownloadItemComplete& GetOnComplete() { return OnComplete; }

private:
	FOnHttpDownloadItemProgress OnProgress;
	FOnHttpDownloadItemComplete OnComplete;

#pragma endregion Events
};
