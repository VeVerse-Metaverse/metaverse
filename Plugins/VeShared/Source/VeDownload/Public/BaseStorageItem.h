// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.
#pragma once

#include "HttpDownloadTypes.h"
#include "FileStorageTypes.h"
#include "StorageItemId.h"

class FDownloadFileStorageSubsystem;
class FHttpDownloadItem;


class VEDOWNLOAD_API FBaseStorageItem : public TSharedFromThis<FBaseStorageItem> {
	friend FDownloadFileStorageSubsystem;

	DECLARE_EVENT_TwoParams(FBaseStorageItem, FOnFileStorageItemProgress, FBaseStorageItem* InStorageItem, const FFileStorageProgressMetadata& InMetadata);
	DECLARE_EVENT_TwoParams(FBaseStorageItem, FOnFileStorageItemComplete, FBaseStorageItem* InStorageItem, const FFileStorageResultMetadata& InMetadata);

public:
	FBaseStorageItem(FDownloadFileStorageSubsystem* InOwner, const FStorageItemId& InStorageItemId);
	virtual ~FBaseStorageItem();

	const FString& GetId() const { return StorageItemId.GetId(); }
	const FBaseStorageItemOptions& GetOptions() const { return ProcessOptions; }
	const FStorageItemId& GetStorageItemId() const { return StorageItemId; }
	const FString& GetFileName() const { return FileName; }

	bool IsProcessing() const { return bIsProcessing; }
	virtual bool IsCacheValid();

	/** Cancel resource receiving. */
	void Cancel();

	/** Start resource receiving. */
	void Process(const FBaseStorageItemOptions& InOptions);

protected:
	/** Expire time (seconds) */
	virtual int32 GetExpireTimespan() const { return 24 * 60 * 60; }

	void DownloadHttp();

	void NativeOnComplete(const FString& Filename, int64 ContentLength);
	void NativeOnError(const FString& Message);
	void NativeOnProgress(int64 ContentLength, int64 DownloadedSize, float Progress, int32 BytesPerSec);

	FString FileDir;
	FString FileName;

	static void DeleteFileIfExists(const FString& InFileName);

private:
	FDownloadFileStorageSubsystem* Owner = nullptr;
	FStorageItemId StorageItemId;
	FBaseStorageItemOptions ProcessOptions;
	bool bIsProcessing = false;

	TSharedPtr<FHttpDownloadItem> HttpDownloadItem;

	/** Called from FDownloadFileStorageSubsystem */
	void ActivateProcess();

	void Timer_LoadFromCache();
	void HttpDownloadItem_OnProgress(FHttpDownloadItem* InDownloadItem, const FHttpDownloadProgressMetadata& InMetadata);
	void HttpDownloadItem_OnComplete(FHttpDownloadItem* InDownloadItem, const FHttpDownloadResultMetadata& InMetadata);

#pragma region Events

public:
	FOnFileStorageItemProgress& GetOnProgress() { return OnProgress; }
	FOnFileStorageItemComplete& GetOnComplete() { return OnComplete; }

private:
	FOnFileStorageItemProgress OnProgress;
	FOnFileStorageItemComplete OnComplete;

#pragma endregion Events
};


class VEDOWNLOAD_API FFileStorageItem : public FBaseStorageItem {
public:
	FFileStorageItem(FDownloadFileStorageSubsystem* InOwner, const FStorageItemId& InStorageItemId);
};


class VEDOWNLOAD_API FPakStorageItem : public FBaseStorageItem {
public:
	FPakStorageItem(FDownloadFileStorageSubsystem* InOwner, const FStorageItemId& InStorageItemId);

	virtual bool IsCacheValid() override;
};


class VEDOWNLOAD_API FTextureStorageItem : public FBaseStorageItem {
public:
	FTextureStorageItem(FDownloadFileStorageSubsystem* InOwner, const FStorageItemId& InStorageItemId);
};
