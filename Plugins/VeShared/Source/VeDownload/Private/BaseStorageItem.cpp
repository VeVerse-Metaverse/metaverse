// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "BaseStorageItem.h"

#include "VeShared.h"
#include "VeDownload.h"
#include "VeWorldSubsystem.h"
#include "HttpDownloadSubsystem.h"
#include "HttpDownloadItem.h"
#include "FileStorageSubsystem.h"


//==============================================================================
// File

FBaseStorageItem::FBaseStorageItem(FDownloadFileStorageSubsystem* InOwner, const FStorageItemId& InStorageItemId)
	: Owner(InOwner),
	  StorageItemId(InStorageItemId) {}

FBaseStorageItem::~FBaseStorageItem() {
	Cancel();
	if (Owner) {
		Owner->Item_OnDestroy(this);
	}
}

void FBaseStorageItem::Cancel() {
	if (bIsProcessing) {
		if (HttpDownloadItem.IsValid()) {
			HttpDownloadItem->Cancel();
		}
	}
}

void FBaseStorageItem::Process(const FBaseStorageItemOptions& InOptions) {
	ProcessOptions = InOptions;

	if (InOptions.DeleteCache) {
		DeleteFileIfExists(FileName);
		DeleteFileIfExists(FileName + TEXT(".meta"));
	}

	if (Owner) {
		Owner->Item_OnProcess(this);
	}
}

bool FBaseStorageItem::IsCacheValid() {
	auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	const FString MetadataFileName = FileName + TEXT(".meta");

	if (!PlatformFile.FileExists(*FileName) || !PlatformFile.FileExists(*MetadataFileName)) {
		return false;
	}

	const FDateTime NowTime = FDateTime::UtcNow();

	FFileStorageMetadata FileStorageMetadata;
	FileStorageMetadata.LoadFromFile(MetadataFileName);

	if (FileStorageMetadata.ExpireTime.GetTicks()) {
		return NowTime >= FileStorageMetadata.ExpireTime;
	}

	const FDateTime FileTime = FileStorageMetadata.GetTimeStamp();
	const FTimespan ExpireTimespan(0, 0, GetExpireTimespan());
	if (NowTime - FileTime < ExpireTimespan) {
		return true;
	}

	return false;
}

void FBaseStorageItem::NativeOnComplete(const FString& Filename, int64 ContentLength) {
	FFileStorageResultMetadata ResultMetadata;
	ResultMetadata.bSuccessful = true;
	ResultMetadata.Filename = Filename;
	ResultMetadata.ContentLength = ContentLength;

	OnComplete.Broadcast(this, ResultMetadata);

	if (Owner) {
		Owner->Item_OnComplete(this);
	}
}

void FBaseStorageItem::NativeOnError(const FString& Message) {
	FFileStorageResultMetadata ResultMetadata;
	ResultMetadata.bSuccessful = false;
	ResultMetadata.Error = Message;

	OnComplete.Broadcast(this, ResultMetadata);

	if (Owner) {
		Owner->Item_OnComplete(this);
	}
}

void FBaseStorageItem::NativeOnProgress(int64 ContentLength, int64 DownloadedSize, float Progress, int32 BytesPerSec) {
	FFileStorageProgressMetadata ProgressMetadata;
	ProgressMetadata.ContentLength = ContentLength;
	ProgressMetadata.DownloadedSize = DownloadedSize;
	ProgressMetadata.Progress = Progress;
	ProgressMetadata.BytesPerSec = BytesPerSec;

	OnProgress.Broadcast(this, ProgressMetadata);
}

void FBaseStorageItem::ActivateProcess() {
	if (bIsProcessing) {
		return;
	}

	auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!PlatformFile.DirectoryExists(*FileDir)) {
		if (!PlatformFile.CreateDirectoryTree(*FileDir)) {
			VeLogErrorFunc("Failed to create directory: %s", *FileDir);
			NativeOnError("failed to create directory");
			return;
		}
	}

	if (IsCacheValid()) {
		if (ProcessOptions.DelayForCache > 0.0f) {
			FTimerDelegate MyTimerDelegate;
			MyTimerDelegate.BindSP(this, &FBaseStorageItem::Timer_LoadFromCache);

			FTimerHandle MyTimerHandle;
			FVeSharedModule::GetTimerManager().SetTimer(MyTimerHandle, MyTimerDelegate, ProcessOptions.DelayForCache, false);
			return;
		}

		NativeOnComplete(FileName, PlatformFile.FileSize(*FileName));
		return;
	}

	bIsProcessing = true;
	DownloadHttp();
}

void FBaseStorageItem::Timer_LoadFromCache() {
	auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	NativeOnComplete(FileName, PlatformFile.FileSize(*FileName));
}

void FBaseStorageItem::DownloadHttp() {
	auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	GET_MODULE_SUBSYSTEM(HttpDownloadSubsystem, Download, HttpDownload);
	if (!HttpDownloadSubsystem) {
		NativeOnError(TEXT("download subsystem not found"));
		return;
	}

	FFileStorageMetadata FileStorageMetadata;
	FileStorageMetadata.LoadFromFile(FileName + TEXT(".meta"));

	FHttpDownloadItemOptions HttpDownloadOptions;
	HttpDownloadOptions.ContentMetadata.ContentLength = FileStorageMetadata.ContentLength;
	HttpDownloadOptions.ContentMetadata.LastModified = FileStorageMetadata.LastModified;
	HttpDownloadOptions.ContentMetadata.ETag = FileStorageMetadata.Hash;
	HttpDownloadOptions.Filename = FileName;

	HttpDownloadItem = HttpDownloadSubsystem->GetDownloadItem(StorageItemId);
	HttpDownloadItem->GetOnProgress().AddSP(AsShared(), &FBaseStorageItem::HttpDownloadItem_OnProgress);
	HttpDownloadItem->GetOnComplete().AddSP(AsShared(), &FBaseStorageItem::HttpDownloadItem_OnComplete);
	HttpDownloadItem->Process(HttpDownloadOptions);
}

void FBaseStorageItem::HttpDownloadItem_OnProgress(FHttpDownloadItem* InDownloadItem, const FHttpDownloadProgressMetadata& InMetadata) {
	NativeOnProgress(InMetadata.ContentLength, InMetadata.DownloadedSize, InMetadata.Progress, InMetadata.BytesPerSec);
}

void FBaseStorageItem::HttpDownloadItem_OnComplete(FHttpDownloadItem* InDownloadItem, const FHttpDownloadResultMetadata& InMetadata) {
	HttpDownloadItem.Reset();
	bIsProcessing = false;

	if (InMetadata.Code == EHttpDownloadCode::Completed) {
		const FString& Filename = InMetadata.Filename;

		FFileStorageMetadata FileStorageMetadata;
		FileStorageMetadata.ContentLength = InMetadata.ContentMetadata.ContentLength;
		FileStorageMetadata.LastModified = InMetadata.ContentMetadata.LastModified;
		FileStorageMetadata.Hash = InMetadata.ContentMetadata.ETag;
		FileStorageMetadata.SaveToFile(Filename + TEXT(".meta"));

		NativeOnComplete(Filename, InMetadata.ContentMetadata.ContentLength);
		return;
	}

	NativeOnError(InMetadata.Error);
}

void FBaseStorageItem::DeleteFileIfExists(const FString& InFileName) {
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (PlatformFile.FileExists(*InFileName)) {
		if (!PlatformFile.DeleteFile(*InFileName)) {
			VeLogErrorFunc("Failed to delete file: %s", *InFileName);
		}
	}
}

//==============================================================================
// File

FFileStorageItem::FFileStorageItem(FDownloadFileStorageSubsystem* InOwner, const FStorageItemId& InStorageItemId)
	: FBaseStorageItem(InOwner, InStorageItemId) {
	FileDir = FPaths::ConvertRelativePathToFull(FString::Printf(TEXT("%s/dlcache/Files/%s"), *FPaths::ProjectPersistentDownloadDir(), *GetStorageItemId().GetId().Left(2)));
	FileName = FileDir / InStorageItemId.GetId();
}

//==============================================================================
// Pak

FPakStorageItem::FPakStorageItem(FDownloadFileStorageSubsystem* InOwner, const FStorageItemId& InStorageItemId)
	: FBaseStorageItem(InOwner, InStorageItemId) {
	FileDir = FPaths::ConvertRelativePathToFull(FString::Printf(TEXT("%s/dlcache/Packages/%s"), *FPaths::ProjectPersistentDownloadDir(), *GetStorageItemId().GetId().Left(2)));
	FileName = FileDir / InStorageItemId.GetId();
}

bool FPakStorageItem::IsCacheValid() {
	auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	const FString MetadataFileName = FileName + TEXT(".meta");

	if (!PlatformFile.FileExists(*FileName) || !PlatformFile.FileExists(*MetadataFileName)) {
		return false;
	}

	return true;
}

//==============================================================================
// Texture

FTextureStorageItem::FTextureStorageItem(FDownloadFileStorageSubsystem* InOwner, const FStorageItemId& InStorageItemId)
	: FBaseStorageItem(InOwner, InStorageItemId) {
	FileDir = FPaths::ConvertRelativePathToFull(FString::Printf(TEXT("%s/dlcache/Textures/%s"), *FPaths::ProjectPersistentDownloadDir(), *GetStorageItemId().GetId().Left(2)));
	FileName = FileDir / InStorageItemId.GetId();
}
