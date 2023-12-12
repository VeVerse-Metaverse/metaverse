// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "Placeable/PlaceableUrlOpenSea.h"

#include "ApiFileMetadata.h"
#include "ApiPropertySubsystem.h"
#include "OpenSeaAssetRequest.h"
#include "VeApi.h"
#include "VeDownload.h"
#include "VeGameModule.h"
#include "VeShared.h"
#include "Components/WidgetComponent.h"
#include "PlaceableComponent.h"
#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Net/UnrealNetwork.h"
#include "Structs/PlaceableAssetMetadata.h"
#include "UI/UIEditorOpenSeaUrlPropertiesWidget.h"
#include "UI/UIEditorUrlPropertiesWidget.h"
#include "UI/UIInspectorImagePreviewWidget.h"
#include "UI/UIInspectorUrlPropertiesWidget.h"
#include "VeOpenSea/VeOpenSeaModule.h"
#include "DownloadChunkSubsystem.h"
#include "Api2FileSubsystem.h"

APlaceableUrlOpenSea::APlaceableUrlOpenSea() {
	PrimaryActorTick.bCanEverTick = false;
}

void APlaceableUrlOpenSea::BeginPlay() {
	Super::BeginPlay();
}

static const TArray SupportedOpenSeaMimeTypes = {
	FString(TEXT("image/jpeg")),
	FString(TEXT("image/png")),
	FString(TEXT("image/gif")),
	FString(TEXT("video/mp4"))
};

void APlaceableUrlOpenSea::OnPropertiesSaved_Http(const FString& Url) {
	// Process an http(s) file URL and replace file link as required. Will automatically handle supported file formats such as PDF documents, images, mp4 videos.  
	GET_MODULE_SUBSYSTEM(ChunkDownloadSubsystem, Download, Chunk);
	if (ChunkDownloadSubsystem) {
		// Make a request to determine file type.
		const auto CallbackPtr = MakeShared<FOnFileHeaderRequestComplete>();
		CallbackPtr->BindWeakLambda(this, [=](const FFileHeaderResult& Result, const bool bSuccessful, const FString& Error) {
			if (bSuccessful) {
				ReplacePlaceableFile(Url, Result.GetContentMimeType());
			} else {
				VeLogErrorFunc("failed to fetch the file: %s", *Error);
			}
		});
		ChunkDownloadSubsystem->GetFileHeaders(Url, CallbackPtr);
	}
}

void APlaceableUrlOpenSea::ReplacePlaceableFile(const FString& InUrl, const FString& InMimeType, const FString& InOriginalUrl) {
	auto Metadata = GetPlaceableMetadata();

	GET_MODULE_SUBSYSTEM(PropertySubsystem, Api, Property);
	if (PropertySubsystem) {
		FApiUpdatePropertyMetadata PropertyMetadata;
		PropertyMetadata.Name = TEXT("OriginalUrl");
		PropertyMetadata.Type = TEXT("string");
		PropertyMetadata.Value = InOriginalUrl;
		const auto Callback = MakeShared<FOnGenericRequestCompleted>();
		Callback->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& InError) {
			if (!bInSuccessful) {
				VeLogErrorFunc("failed to update properties");
			} else {
				OriginalUrl = InOriginalUrl;
			}
		});
		PropertySubsystem->Update(Metadata.Id, PropertyMetadata, Callback);
	}

	GET_MODULE_SUBSYSTEM(FileUploadSubssytem, Api, FileUpload);
	if (FileUploadSubssytem) {
		// Validate mime type.
		const auto MimeType = InMimeType;
		if (!MimeType.IsEmpty() && !SupportedOpenSeaMimeTypes.Contains(MimeType)) {
			VeLogErrorFunc("unsupported mime type: %s", *MimeType);
			return;
		}

		const auto ReplaceFileCallback = MakeShared<FOnFileRequestCompleted>();
		ReplaceFileCallback->BindWeakLambda(this, [this](const FApiFileMetadata InFileMetadata, const bool bSuccessful2, const FString& Error2) {
			if (bSuccessful2) {
				UpdatePlaceableOnServer();
			} else {
				VeLogErrorFunc("failed to replace placeable file: %s", *Error2);
			}
		});

		FApiEntityFileLinkMetadata FileMetadata;
		FileMetadata.File = InUrl;
		FileMetadata.EntityId = GetPlaceableMetadata().Id;
		FileMetadata.SetFileType(MimeType, InUrl);

		if (!FileUploadSubssytem->Replace(FileMetadata, ReplaceFileCallback)) {
			VeLogErrorFunc("failed to replace placeable file");
		}
	}
}

void APlaceableUrlOpenSea::OnPropertiesSaved_OpenSeaHttp(const FString& Url) {
	GET_MODULE_SUBSYSTEM(ChunkDownloadSubsystem, Download, Chunk);
	if (ChunkDownloadSubsystem) {
		TArray<FString> Tokens;
		Url.ParseIntoArray(Tokens,TEXT("/"), true);
		if (Tokens.Num() > 2) {
			const int32 ContractAddressIndex = Tokens.Num() - 2;
			const int32 TokenIdIndex = Tokens.Num() - 1;
			const auto ContractAddress = Tokens[ContractAddressIndex];
			if (ContractAddress.StartsWith(TEXT("0x"))) {
				const auto TokenId = Tokens[TokenIdIndex];
				GET_MODULE_SUBSYSTEM(OpenSeaAssetSubsystem, OpenSea, Asset);
				if (OpenSeaAssetSubsystem) {
					// 1. Create a callback.
					const auto OpenSeaAssetCallbackPtr = MakeShared<FOnOpenSeaAssetRequestCompleted>();
					// 2. Bind callback.
					OpenSeaAssetCallbackPtr->BindWeakLambda(this, [=](const FOpenSeaAssetMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
						if (!bInSuccessful) {
							LogErrorF("failed to request: %s", *InError);
							return;
						}

						ReplacePlaceableFile(InMetadata.ImageUrl, InMetadata.MimeType, Url);
					});
					// 3. Make the request using the callback.
					OpenSeaAssetSubsystem->GetAsset(ContractAddress, TokenId, OpenSeaAssetCallbackPtr);
				}
			}
		}
	}
}

void APlaceableUrlOpenSea::OnPropertiesSaved_File(const FString& Url) {
	// Process file protocol URLs and upload file. Will automatically handle supported file formats such as PDF documents, images, mp4 videos.
	GET_MODULE_SUBSYSTEM(FileSubsystem, Api2, File);
	if (FileSubsystem) {
		// Get file mime type.
		const auto MimeType = FGenericPlatformHttp::GetMimeType(Url);

		// Validate mime type.
		if (!SupportedOpenSeaMimeTypes.Contains(MimeType)) {
			VeLogErrorFunc("unsupported mime type: %s", *MimeType);
			return;
		}

		// Upload placeable file.
		const TSharedRef<FOnFileRequestCompleted> CallbackPtr = MakeShareable(new FOnFileRequestCompleted());
		CallbackPtr->BindWeakLambda(this, [this](const FApiFileMetadata& InFileMetadata, const bool bInSuccessful, const FString& InError) {
			if (bInSuccessful) {
				UpdatePlaceableOnServer();
			} else {
				VeLogErrorFunc("failed to upload a placeable file: %s", *InError);
			}
		});

		FApi2FileUploadRequestMetadata FileMetadata;
		FileMetadata.EntityId = GetPlaceableMetadata().Id;
		FileMetadata.FilePath = Url;
		FileMetadata.SetFileType(MimeType, Url);

		FileSubsystem->Upload(FileMetadata, CallbackPtr);
	}
}

UUIPlaceablePropertiesWidget* APlaceableUrlOpenSea::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIEditorOpenSeaUrlPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!PropertiesWidget) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	const auto& Metadata = PlaceableComponent->GetMetadata();

	if (const auto* File = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
		return Item.Type == EApiFileType::Pdf;
	})) {
		PropertiesWidget->SetUrl(File->Url);
	} else if (const auto* File1 = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
		return Item.Type == EApiFileType::Video;
	})) {
		PropertiesWidget->SetUrl(File1->Url);
	} else if (const auto* File2 = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
		return Item.Type == EApiFileType::ImageFull;
	})) {
		PropertiesWidget->SetUrl(File2->Url);
	} else if (const auto* File3 = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
		return Item.Type == EApiFileType::TextureDiffuse;
	})) {
		PropertiesWidget->SetUrl(File3->Url);
	}

	if (auto* PropertyMetadata = Metadata.Properties.FindByPredicate([](const FVePropertyMetadata& Property) {
		return Property.Type == EVePropertyType::String && Property.Name == TEXT("OriginalUrl");
	})) {
		//PropertiesWidget->OriginalUrl = PropertyMetadata->Value;
		PropertiesWidget->SetOriginalUrl(PropertyMetadata->Value);
	}

	PropertiesWidget->GetOnSave().BindWeakLambda(this, [=] {
		OnPropertiesSaved(PropertiesWidget->GetUrl());
	});

	return PropertiesWidget;
}

UUserWidget* APlaceableUrlOpenSea::GetInspectPreviewWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(InspectablePreviewClass)) {
		return nullptr;
	}

	auto* PreviewWidget = CreateWidget<UUIInspectorImagePreviewWidget>(PlayerController, InspectablePreviewClass);
	if (!IsValid(PreviewWidget)) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	auto Metadata = GetPlaceableMetadata();

	if (const auto* FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
		return InFileMetadata.Type != EApiFileType::TextureDiffuse;
	})) {
		PreviewWidget->SetUrl(FileMetadata->Url);
	} else {
		FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
			return InFileMetadata.Type == EApiFileType::ImageFull;
		});

		if (FileMetadata) {
			PreviewWidget->SetUrl(FileMetadata->Url);
		} else {
			FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
				return InFileMetadata.Type == EApiFileType::ImagePreview;
			});

			if (FileMetadata) {
				PreviewWidget->SetUrl(FileMetadata->Url);
			} else {
				LogWarningF("failed to get a file");
			}
		}
	}

	return PreviewWidget;
}

UUserWidget* APlaceableUrlOpenSea::GetInspectPropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(InspectablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIInspectorUrlPropertiesWidget>(PlayerController, InspectablePropertiesClass);
	if (!IsValid(PropertiesWidget)) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	auto Metadata = GetPlaceableMetadata();

	if (const auto* PropertyMetadata = Metadata.Properties.FindByPredicate([](const FVePropertyMetadata& Property) {
		return Property.Type == EVePropertyType::String && Property.Name == TEXT("OriginalUrl");
	})) {
		//PropertiesWidget->OriginalUrl = PropertyMetadata->Value;
		PropertiesWidget->SetUrl(PropertyMetadata->Value);
	}

	if (!OriginalUrl.IsEmpty()) {
		PropertiesWidget->SetUrl(OriginalUrl);
	}

	return PropertiesWidget;
}

void APlaceableUrlOpenSea::OnPropertiesSaved(const FString& Url) {
	if (Url.Contains(TEXT("opensea.io"))) {
		OnPropertiesSaved_OpenSeaHttp(Url);
	} else if (Url.StartsWith("http")) {
		OnPropertiesSaved_Http(Url);
	} else if (Url.StartsWith("file")) {
		OnPropertiesSaved_File(Url);
	} else {
		VeLogErrorFunc("unsupported url protocol: %s", *Url);
	}
}
