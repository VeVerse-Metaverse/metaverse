// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "HttpDownloadItem.h"

#include "VeShared.h"
#include "HttpDownloadSubsystem.h"
#include "HttpDownloadTask.h"
#include "StorageItemId.h"

#undef VE_LOG_MODULE
#define VE_LOG_MODULE HttpDownloadSubsystem


FHttpDownloadItem::FHttpDownloadItem(FDownloadHttpDownloadSubsystem* InOwner, const FStorageItemId& InStorageItemId)
	: Owner(InOwner),
	  HttpDownloadTask(MakeShared<FHttpDownloadTask>(InStorageItemId)) { }

FHttpDownloadItem::~FHttpDownloadItem() {
	Cancel();
	if (Owner) {
		Owner->Item_OnDestroy(this);
	}
}

const FString& FHttpDownloadItem::GetId() const {
	return HttpDownloadTask->GetStorageItemId().GetId();
}

const FStorageItemId& FHttpDownloadItem::GetStorageItemId() const {
	return HttpDownloadTask->GetStorageItemId();
}

void FHttpDownloadItem::Cancel(const bool bDeleteCacheFile) const {
	if (bIsProcessing) {
		HttpDownloadTask->Cancel(bDeleteCacheFile);
	}
}

void FHttpDownloadItem::Process(const FHttpDownloadItemOptions& InOptions) {
	ProcessOptions = InOptions;
	if (Owner) {
		Owner->Item_OnProcess(this);
	}
}

void FHttpDownloadItem::ActivateProcess() {
	if (bIsProcessing) {
		return;
	}
	bIsProcessing = true;

	HttpDownloadTask->GetOnProgress().AddRaw(this, &FHttpDownloadItem::HttpTask_OnProgress);
	HttpDownloadTask->GetOnComplete().AddRaw(this, &FHttpDownloadItem::HttpTask_OnComplete);

	FHttpDownloadTaskOptions TaskProcessOptions;
	TaskProcessOptions.ContentMetadata = ProcessOptions.ContentMetadata;
	TaskProcessOptions.Filename = ProcessOptions.Filename;
	HttpDownloadTask->Process(TaskProcessOptions);
}

void FHttpDownloadItem::HttpTask_OnProgress(FHttpDownloadTask* InDownloadTask, const FHttpDownloadProgressMetadata& InMetadata) {
	OnProgress.Broadcast(this, InMetadata);
}

void FHttpDownloadItem::HttpTask_OnComplete(FHttpDownloadTask* InDownloadTask, const FHttpDownloadResultMetadata& InMetadata) {
	HttpDownloadTask->GetOnProgress().RemoveAll(this);
	HttpDownloadTask->GetOnComplete().RemoveAll(this);

	bIsProcessing = false;
	OnComplete.Broadcast(this, InMetadata);

	if (Owner) {
		Owner->Item_OnComplete(this);
	}
}
