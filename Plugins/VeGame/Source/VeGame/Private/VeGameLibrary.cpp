// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeGameLibrary.h"

#include "VeShared.h"
#include "FileStorageSubsystem.h"


#if 0
void UVeGameLibrary::TestFunc(const FString& InString) {
	GET_MODULE_SUBSYSTEM(FileStorageSubsystem, Download, FileStorage);
	if (!FileStorageSubsystem) {
		return;
	}

	const FStorageItemId StorageItemId(InString);
	constexpr FBaseStorageItemOptions StorageItemOptions;

	UE_LOG(LogTemp, Log, TEXT(">>> START: {%s}"), *InString);

	TSharedPtr<FBaseStorageItem> StorageItem = FileStorageSubsystem->GetFileStorageItem(StorageItemId);

	StorageItem->GetOnProgress().AddLambda([=](FBaseStorageItem* InStorageItem, const FFileStorageProgressMetadata& InMetadata) {
		UE_LOG(LogTemp, Log, TEXT(">>> STATE: PROGRESS %f"), InMetadata.Progress);
	});

	StorageItem->GetOnComplete().AddLambda([=](FBaseStorageItem* InStorageItem, const FFileStorageResultMetadata& InMetadata) {
		if (!InMetadata.bSuccessful) {
			UE_LOG(LogTemp, Log, TEXT(">>> FAILURE: {%s}"), *InString);
			return;
		}

		UE_LOG(LogTemp, Log, TEXT(">>> SUCCESSFUL: {%s}"), *InString);
	});

	StorageItem->Process(StorageItemOptions);
}
#endif
