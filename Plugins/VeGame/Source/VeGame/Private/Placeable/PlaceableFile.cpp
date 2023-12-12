// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableFile.h"

#include "VeDownload.h"
#include "VeGameModule.h"
#include "VeShared.h"
#include "PlaceableComponent.h"
#include "PlaceableLib.h"
#include "VeApi2.h"
#include "Api2FileSubsystem.h"
#include "ApiFileMetadata.h"
#include "UI/UIEditorUrlPropertiesWidget.h"
#include "UI/UIInspectorImagePreviewWidget.h"
#include "UI/UIInspectorUrlPropertiesWidget.h"
#include "DownloadChunkSubsystem.h"

static const TArray SupportedMimeTypes = {
	FString(TEXT("image/jpeg")),
	FString(TEXT("image/png")),
	FString(TEXT("image/gif")),
	FString(TEXT("application/mp4")),
	FString(TEXT("video/mp4"))
};

APlaceableFile::APlaceableFile() {
	PrimaryActorTick.bCanEverTick = false;
}

void APlaceableFile::BeginPlay() {
	Super::BeginPlay();
}

UUIPlaceablePropertiesWidget* APlaceableFile::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIEditorUrlPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!PropertiesWidget) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	const auto& Metadata = GetPlaceableMetadata();

	const auto* FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
		return Item.Type == EApiFileType::Pdf;
	});
	if (!FileMetadata) {
		FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
			return Item.Type == EApiFileType::Video;
		});
	}
	if (!FileMetadata) {
		FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
			return Item.Type == EApiFileType::ImageFull;
		});
	}

	if (FileMetadata) {
		PropertiesWidget->SetUrl(FileMetadata->Url);
	}

	PropertiesWidget->GetOnSave().BindWeakLambda(this, [=] {
		OnPropertiesSaved(PropertiesWidget->GetUrl());
	});

	return PropertiesWidget;
}

void APlaceableFile::OnPropertiesSaved(const FString& Url) {
	const auto Callback = MakeShared<FOnGenericRequestCompleted>();
	Callback->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& InError) {
		if (bInSuccessful) {
			UpdatePlaceableOnServer();
		}
		SetPlaceableState(EPlaceableState::None);
	});

	SetPlaceableState(EPlaceableState::Uploading);
	if (Url.StartsWith("http")) {
		OnPropertiesSaved_Http(Callback, Url);
	} else if (Url.StartsWith("file")) {
		OnPropertiesSaved_File(Callback, Url);
	} else {
		VeLogErrorFunc("Unsupported url protocol: %s", *Url);
		SetPlaceableState(EPlaceableState::None);
	}
}

void APlaceableFile::OnPropertiesSaved_Http(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& Url) {
	// Process an http(s) file URL and replace file link as required. Will automatically handle supported file formats such as PDF documents, images, mp4 videos.  
	GET_MODULE_SUBSYSTEM(ChunkDownloadSubsystem, Download, Chunk);
	if (!ChunkDownloadSubsystem) {
		InCallback->ExecuteIfBound(false, TEXT(""));
		return;
	}

	// Make a request to determine file type.
	const auto CallbackPtr = MakeShared<FOnFileHeaderRequestComplete>();
	CallbackPtr->BindWeakLambda(this, [=](const FFileHeaderResult& Result, const bool bInSuccessful, const FString& InError) {
		if (bInSuccessful) {
			ReplacePlaceableFile(InCallback, Url, Result.GetContentMimeType());
		} else {
			VeLogErrorFunc("Failed to fetch the file: %s", *InError);
			InCallback->ExecuteIfBound(false, InError);
		}
	});

	ChunkDownloadSubsystem->GetFileHeaders(Url, CallbackPtr);
}

void APlaceableFile::ReplacePlaceableFile(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& InUrl, const FString& InMimeType) {
	GET_MODULE_SUBSYSTEM(FileSubsystem, Api2, File);
	if (!FileSubsystem) {
		InCallback->ExecuteIfBound(false, TEXT(""));
		return;
	}

	// Validate mime type.
	if (!SupportedMimeTypes.Contains(InMimeType)) {
		VeLogErrorFunc("Unsupported mime type: %s", *InMimeType);
		InCallback->ExecuteIfBound(false, FString::Printf(TEXT("unsupported mime type: %s"), *InMimeType));
		return;
	}

	const auto Callback = MakeShared<FOnFileRequestCompleted>();
	Callback->BindWeakLambda(this, [=](const FApiFileMetadata InFileMetadata, const bool bInSuccessful, const FString& InError) {
		if (!bInSuccessful) {
			VeLogErrorFunc("Failed to replace placeable file: %s", *InError);
		}
		InCallback->ExecuteIfBound(bInSuccessful, InError);
	});

	FApi2FileLinkRequestMetadata FileMetadata;
	FileMetadata.Url = InUrl;
	FileMetadata.Id = GetPlaceableMetadata().Id.ToString(EGuidFormats::DigitsWithHyphensLower);
	FileMetadata.SetFileType(InMimeType, InUrl);
	FileMetadata.Mime = InMimeType;

	FileSubsystem->Link(FileMetadata, Callback);
}

void APlaceableFile::OnPropertiesSaved_File(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& Url) {
	// Process file protocol URLs and upload file. Will automatically handle supported file formats such as PDF documents, images, mp4 videos.
	GET_MODULE_SUBSYSTEM(FileSubsystem, Api2, File);
	if (!FileSubsystem) {
		InCallback->ExecuteIfBound(false, TEXT(""));
		return;
	}

	// Get file mime type.
	const auto MimeType = UPlaceableLib::GetFileMimeType(Url);

	// Validate mime type.
	if (!SupportedMimeTypes.Contains(MimeType)) {
		VeLogErrorFunc("Unsupported mime type: %s", *MimeType);
		InCallback->ExecuteIfBound(false, FString::Printf(TEXT("unsupported mime type: %s"), *MimeType));
		return;
	}

	// Upload placeable file.
	const TSharedRef<FOnFileRequestCompleted> CallbackPtr = MakeShareable(new FOnFileRequestCompleted());
	CallbackPtr->BindWeakLambda(this, [=](const FApiFileMetadata& InFileMetadata, const bool bInSuccessful, const FString& InError) {
		if (!bInSuccessful) {
			VeLogErrorFunc("Failed to upload a placeable file: %s", *InError);
		}
		InCallback->ExecuteIfBound(bInSuccessful, InError);
	});

	FApi2FileUploadRequestMetadata FileMetadata;
	FileMetadata.EntityId = GetPlaceableMetadata().Id;
	FileMetadata.FilePath = Url;
	FileMetadata.SetFileType(MimeType, Url);
	FileMetadata.Mime = MimeType;

	FileSubsystem->Upload(FileMetadata, CallbackPtr);
}

UUserWidget* APlaceableFile::GetInspectPreviewWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(InspectablePreviewClass)) {
		return nullptr;
	}

	const auto& Metadata = GetPlaceableMetadata();

	const auto* FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
		return Item.Type != EApiFileType::TextureDiffuse;
	});
	if (!FileMetadata) {
		FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
			return Item.Type == EApiFileType::ImageFull;
		});
	}
	if (!FileMetadata) {
		FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
			return Item.Type == EApiFileType::ImagePreview;
		});
	}

	if (!FileMetadata) {
		VeLogWarningFunc("Failed to get a file");
		return nullptr;
	}

	auto* InspectPreviewWidget = CreateWidget<UUIInspectorImagePreviewWidget>(PlayerController, InspectablePreviewClass);
	if (!IsValid(InspectPreviewWidget)) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	InspectPreviewWidget->SetUrl(FileMetadata->Url);

	return InspectPreviewWidget;
}

UUserWidget* APlaceableFile::GetInspectPropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(InspectablePropertiesClass)) {
		return nullptr;
	}

	const auto& Metadata = GetPlaceableMetadata();

	const auto* FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
		return Item.Type != EApiFileType::TextureDiffuse;
	});
	if (!FileMetadata) {
		FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
			return Item.Type == EApiFileType::ImageFull;
		});
	}
	if (!FileMetadata) {
		FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
			return Item.Type == EApiFileType::ImagePreview;
		});
	}

	if (!FileMetadata) {
		VeLogWarningFunc("Failed to get a file");
		return nullptr;
	}

	auto* InspectPropertiesWidget = CreateWidget<UUIInspectorUrlPropertiesWidget>(PlayerController, InspectablePropertiesClass);
	if (!IsValid(InspectPropertiesWidget)) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	InspectPropertiesWidget->SetUrl(FileMetadata->Url);

	return InspectPropertiesWidget;
}
