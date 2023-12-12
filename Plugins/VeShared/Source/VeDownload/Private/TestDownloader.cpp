// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "TestDownloader.h"

#include "VeShared.h"
#include "VeDownload.h"
#include "Download1Subsystem.h"
#include "FileStorageSubsystem.h"
#include "HttpDownloadSubsystem.h"
#include "HttpDownloadItem.h"
#include "HttpDownloadTask.h"
#include "HttpDownloadTypes.h"


// Sets default values
ATestDownloader::ATestDownloader() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATestDownloader::BeginPlay() {
	Super::BeginPlay();
}

void ATestDownloader::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ATestDownloader::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// URL:			http://test.loc/003.jpg
// Filename:	D:/Projects/LE7EL/API_REST/Test.jpg
void ATestDownloader::TestDownload(const FString& URL, const FString& Filename) {
#if 0
	
	GET_MODULE_SUBSYSTEM(DownloadSubsystem, Download, Download);
	if (!DownloadSubsystem) {
		return;
	}
	
	FDownloadRequestMetadata RequestMetadata;
	RequestMetadata.Url = URL;
	RequestMetadata.Mode = EDownloadMode::Overwrite;
	RequestMetadata.bUseRange = true;
	RequestMetadata.bUsePreSignedUrl = false;
	RequestMetadata.Priority = 0;
	
	const auto Download = DownloadSubsystem->Download(RequestMetadata);
	
	Download->GetOnDownloadProgress().AddWeakLambda(this, [=](const FDownload1ProgressMetadata& Metadata) {
		VeLogFunc(">>> Downloading: %.2f", Metadata.Progress);
	});
	
	Download->GetOnDownloadComplete().AddWeakLambda(this, [=](const FDownload1ResultMetadata& Metadata) {
		VeLogFunc(">>> Downloading: COMPLETE");
	});

#endif

#if 0 // HttpDownloadTask

	if (!HttpDownloadTask.IsValid()) {
		FStorageItemId StorageItemId(URL);
		HttpDownloadTask = MakeShared<FHttpDownloadTask>(StorageItemId);
	}

	FHttpDownloadTaskOptions DownloadOptions;
	DownloadOptions.Filename = Filename;
	HttpDownloadTask->Process(DownloadOptions);
	
#endif

#if 1 // HttpDownloadSubsystem, HttpDownloadItem
	{
		GET_MODULE_SUBSYSTEM(HttpDownloadSubsystem, Download, HttpDownload);
		if (!HttpDownloadSubsystem) {
			return;
		}

		FStorageItemId StorageItemId(URL);

		FHttpDownloadItemOptions DownloadOptions;
		DownloadOptions.Filename = Filename;

		HttpDownloadItem = HttpDownloadSubsystem->GetDownloadItem(StorageItemId);

		HttpDownloadItem->GetOnProgress().AddWeakLambda(this, [=](FHttpDownloadItem* InDownloadItem, const FHttpDownloadProgressMetadata& InMetadata) {
			VeLogFunc(">>> Downloading PROCESS: %.2f", InMetadata.Progress);
		});

		HttpDownloadItem->GetOnComplete().AddWeakLambda(this, [=](FHttpDownloadItem* InDownloadItem, const FHttpDownloadResultMetadata& InMetadata) {
			VeLogFunc(">>> Downloading COMPLETE: %s", *InMetadata.Filename);
		});

		HttpDownloadItem->Process(DownloadOptions);
	}
#endif

#if 0 // FileStorageSubsystem
	{
		GET_MODULE_SUBSYSTEM(FileStorageSubsystem, Download, FileStorage);
		if (!FileStorageSubsystem) {
			return;
		}
	
		FStorageItemId StorageItemId(URL);
		FBaseStorageItemOptions StorageItemOptions;
		StorageItemOptions.DeleteCache = true;
	
		auto StorageItem = FileStorageSubsystem->GetFileStorageItem(StorageItemId);

		StorageItem->GetOnProgress().AddWeakLambda(this, [=](FBaseStorageItem* InStorageItem, const FFileStorageProgressMetadata& InMetadata) {
			VeLogFunc(">>> Downloading PROCESS: %.2f", InMetadata.Progress);
		});
	
		StorageItem->GetOnComplete().AddWeakLambda(this, [=](FBaseStorageItem* InStorageItem, const FFileStorageResultMetadata& InMetadata) {
			VeLogFunc(">>> Downloading COMPLETE: %s", *InMetadata.Filename);
		});
	
		StorageItem->Process(StorageItemOptions);
	}
#endif

}

void ATestDownloader::TestDownloadCancel() {
	if (HttpDownloadItem.IsValid()) {
		HttpDownloadItem->Cancel();
	}
}

void ATestDownloader::TestDateTime(const FString& Key) {}

// void ATestDownloader::VeTest() {
// 	GET_MODULE_SUBSYSTEM(DownloadSubsystem, Download, Download2);
// 	if (!DownloadSubsystem) {
// 		return;
// 	}
//
// 	if (!DownloadTask2.IsValid()) {
// 		FString TestUrl = TEXT("http://test.loc/003.jpg");
// 		FFileStorageId FileStorageId(TestUrl);
// 		DownloadTask2 = MakeShared<FVeDownload2Task>(FileStorageId);
// 	}
//
// 	FDownload2Options DownloadOptions;
// 	DownloadOptions.Path = TEXT("D:/Projects/LE7EL/API_REST/Test.jpg");
//
// 	DownloadTask2->Process(DownloadOptions);
//
// }
