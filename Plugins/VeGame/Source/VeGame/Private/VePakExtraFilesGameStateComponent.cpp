// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VePakExtraFilesGameStateComponent.h"

#include "VeShared.h"
#include "VeGameModule.h"
#include "VeDownload.h"
#include "FileStorageSubsystem.h"
#include "VeApi2.h"
#include "Api2PackageSubsystem.h"
#include "GameFrameworkWorldItem.h"
#include "VeGameStateBase.h"
#include "VeGameFramework.h"
#include "GameFrameworkWorldSubsystem.h"
#include "VeWorldSettings.h"


UVePakExtraFilesGameStateComponent::UVePakExtraFilesGameStateComponent() {}


void UVePakExtraFilesGameStateComponent::BeginInitialization() {
	// Copy PakExtraContent files
	if (!IsNetMode(NM_DedicatedServer)) {
#if !WITH_EDITOR

		DeletePakExtraContentFiles();
		
		if (const auto* World = GetWorld()) {
			if (auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
				auto OnWorldItem = AVeWorldSettings::FWorldItemDelegate::CreateWeakLambda(this, [=](TSharedPtr<FGameFrameworkWorldItem> InWorldItem) {
					if (InWorldItem.IsValid()) {
						const auto& WorldMetadata = InWorldItem->GetMetadata();
						if (WorldMetadata.Package->Id.IsValid()) {
							CopyPakExtraContentFiles(WorldMetadata.Package->Id);
							return;
						}
					} else {
						VeLogWarningFunc("Failed to get WorldItem.")
					}
					Complete(true);
				});

				VeWorldSettings->OnReady_WorldItem(OnWorldItem);
				return;
			}
		}
		
#endif

		Complete(true);
	}
}

void UVePakExtraFilesGameStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

#if !WITH_EDITOR
	// Delete PakExtraContent files
	if (!IsRunningDedicatedServer()) {
		DeletePakExtraContentFiles();
	}
#endif
}

void UVePakExtraFilesGameStateComponent::Complete(bool InForceComplete) {
	if (InForceComplete) {
		bComplete = true;
	} else {
		if (const int32 Num = PakExtraContentFiles.Num()) {
			if (++PakExtraContentFilesComplete < Num) {
				return;
			}
			bComplete = true;
		}
	}

	if (bComplete) {
		NativeOnComplete();
	}
}

void UVePakExtraFilesGameStateComponent::NativeOnComplete() {
	OnComplete.Broadcast();
}

void UVePakExtraFilesGameStateComponent::CopyPakExtraContentFiles(const FGuid& PackageId) {
	PakExtraContentFiles.Empty();
	PakExtraContentFilesComplete = 0;

	GET_MODULE_SUBSYSTEM(PackageSubsystem, Api2, Package);
	if (!PackageSubsystem) {
		Complete(true);
		return;
	}

	const auto Callback = MakeShared<FOnPackageRequestCompleted2>();
	Callback->BindWeakLambda(this, [=](const FApiPackageMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (!bSuccessful) {
			VeLogErrorFunc("Failed to request package metadata: %s", *InError);
			Complete(true);
			return;
		}

		for (const auto& FileMetadata : InMetadata.Files) {
			if (FileMetadata.Type == EApiFileType::PakExtraContent) {
				PakExtraContentFiles.Emplace(FileMetadata);
			}
		}

		if (!PakExtraContentFiles.Num()) {
			Complete(true);
			return;
		}

		for (const auto& FileMetadata : PakExtraContentFiles) {
			CopyPakExtraContent(FileMetadata);
		}
	});

	PackageSubsystem->Get(PackageId, Callback);
}

void UVePakExtraFilesGameStateComponent::CopyPakExtraContent(const FApiFileMetadata& InFileMetadata) {
	if (InFileMetadata.OriginalPath.IsEmpty()) {
		Complete(false);
		return;
	}

	GET_MODULE_SUBSYSTEM(FileStorageSubsystem, Download, FileStorage);
	if (!FileStorageSubsystem) {
		Complete(false);
		return;
	}

	FStorageItemId StorageItemId(InFileMetadata.Url);
	FBaseStorageItemOptions StorageItemOptions;

	auto StorageItem = FileStorageSubsystem->GetFileStorageItem(StorageItemId);

	// StorageItem->GetOnProgress().AddWeakLambda(this, [=](FBaseStorageItem* InStorageItem, const FFileStorageProgressMetadata& InMetadata) {
	// 	VeLogFunc(">>> Downloading PROCESS: %.2f", InMetadata.Progress);
	// });

	StorageItem->GetOnComplete().AddWeakLambda(this, [=](FBaseStorageItem* InStorageItem, const FFileStorageResultMetadata& InMetadata) {
		if (!InMetadata.bSuccessful) {
			Complete(false);
			return;
		}

		const FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / InFileMetadata.OriginalPath);
		const FString FileDir = FPaths::GetPath(FilePath);

		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (PlatformFile.FileExists(*FilePath)) {
			if (!PlatformFile.DeleteFile(*FilePath)) {
				VeLogErrorFunc("Failed to delete file: %s", *FilePath);
				Complete(false);
				return;
			}
		}

		if (!PlatformFile.DirectoryExists(*FileDir)) {
			if (PlatformFile.CreateDirectory(*FileDir)) {
				VeLogErrorFunc("Failed to create directory for file: %s", *FilePath);
				Complete(false);
				return;
			}
		}

		if (!PlatformFile.CopyFile(*FilePath, *InMetadata.Filename)) {
			VeLogErrorFunc("Failed to copy file: %s", *FilePath);
			Complete(false);
			return;
		}

		Complete(false);
	});

	StorageItem->Process(StorageItemOptions);
}

void UVePakExtraFilesGameStateComponent::DeletePakExtraContentFiles() {
	for (const auto& FileMetadata : PakExtraContentFiles) {
		DeletePakExtraContent(FileMetadata);
	}
	PakExtraContentFiles.Empty();
}

void UVePakExtraFilesGameStateComponent::DeletePakExtraContent(const FApiFileMetadata& InFileMetadata) {
	GET_MODULE_SUBSYSTEM(FileStorageSubsystem, Download, FileStorage);
	if (!FileStorageSubsystem) {
		return;
	}

	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / InFileMetadata.OriginalPath);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (PlatformFile.FileExists(*FilePath)) {
		if (!PlatformFile.DeleteFile(*FilePath)) {
			VeLogErrorFunc("Failed to delete file: %s", *FilePath);
			return;
		}
	}
}
