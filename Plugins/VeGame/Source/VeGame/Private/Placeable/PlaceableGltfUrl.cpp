// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableGltfUrl.h"

#include "Placeable/PlaceableFile.h"
#include "ApiFileMetadata.h"
#include "glTFRuntimeAsset.h"
#include "VeApi.h"
#include "VeDownload.h"
#include "VeGameModule.h"
#include "VeShared.h"
#include "Components/WidgetComponent.h"
#include "PlaceableComponent.h"
#include "VeUI.h"
#include "Structs/PlaceableAssetMetadata.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "UI/UIEditorGltfPropertiesWidget.h"
#include "VeOpenSea/VeOpenSeaModule.h"
#include "VeApi2.h"
#include "Api2FileSubsystem.h"
#include "UI/UIPlaceableStateWidget.h"
#include "PlaceableLib.h"
#include "Components/GltfMeshComponent.h"
#include "UI/UIInspectorModelPreviewWidget.h"

#define LOCTEXT_NAMESPACE "VeGame"


static const TArray SupportedMimeTypesGltf = {
	FString(TEXT("model/mesh")),
	FString(TEXT("model/gltf-binary")),
	FString(TEXT("binary/octet-stream")),
	FString(TEXT("application/unknown")),
	FString(TEXT("application/xml")),
};

APlaceableGltfUrl::APlaceableGltfUrl() {
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RootScene");
	RootComponent = RootSceneComponent;
	RootComponent->SetIsReplicated(true);

	GltfMeshComponent = CreateDefaultSubobject<UGltfMeshComponent>("GltfMesh");
	if (GltfMeshComponent) {
		GltfMeshComponent->SetupAttachment(RootComponent);
		GltfMeshComponent->SetRelativeLocation(FVector::ZeroVector);
		GltfMeshComponent->SetCanEverAffectNavigation(false);
	}
}

void APlaceableGltfUrl::PostInitializeComponents() {
	Super::PostInitializeComponents();

	// Client and DedicatedServer
	if (PlaceableComponent) {
		PlaceableComponent->GetOnPlaceableMetadataChanged().AddUObject(this, &APlaceableGltfUrl::OnPlaceableMetadataChanged);
	}

	if (IsRunningDedicatedServer()) {
		return;
	}

	GltfMeshComponent->OnDownloadingBegin.AddWeakLambda(this, [=]() {
		SetPlaceableState(EPlaceableState::Downloading);
	});
	GltfMeshComponent->OnDownloadingProgress.AddWeakLambda(this, [=](float Progress) {
		SetPlaceableState(EPlaceableState::Downloading, Progress);
	});
	GltfMeshComponent->OnDownloadingEnd.AddWeakLambda(this, [=]() {
		SetPlaceableState(EPlaceableState::None);
	});

	if (IsValid(WidgetClass)) {
		StateWidget = CreateWidget<UUIPlaceableStateWidget>(GetWorld(), WidgetClass);
		if (StateWidget.IsValid()) {
			WidgetComponent = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass());
			WidgetComponent->SetupAttachment(RootSceneComponent);
			WidgetComponent->SetRelativeLocation(FVector::ZeroVector);
			WidgetComponent->SetDrawSize(WidgetDrawSize);
			WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
			WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WidgetComponent->SetWidget(StateWidget.Get());
			WidgetComponent->RegisterComponent();

			StateWidget->SetPlaceableInterface(this);
		}
	}
}

void APlaceableGltfUrl::BeginPlay() {
	Super::BeginPlay();
}

UUIPlaceablePropertiesWidget* APlaceableGltfUrl::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIEditorGltfPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!PropertiesWidget) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	const auto& Metadata = PlaceableComponent->GetMetadata();

	if (const auto* File = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
		return Item.Type == EApiFileType::Model;
	})) {
		PropertiesWidget->SetUrl(File->Url);
	}

	PropertiesWidget->GetOnSave().BindWeakLambda(this, [=] {
		OnPropertiesSaved(PropertiesWidget->GetUrl());
	});

	return PropertiesWidget;
}

void APlaceableGltfUrl::OnPropertiesSaved(const FString& Url) {
	const auto Callback = MakeShared<FOnGenericRequestCompleted>();
	Callback->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& InError) {
		if (bInSuccessful) {
			UpdatePlaceableOnServer();
		}
		SetPlaceableState(EPlaceableState::None);
	});

	SetPlaceableState(EPlaceableState::Uploading);
	if (Url.Contains(TEXT("opensea.io"))) {
		OnPropertiesSaved_OpenSeaHttp(Callback, Url);
	} else if (Url.StartsWith("http")) {
		OnPropertiesSaved_Http(Callback, Url);
	} else if (Url.StartsWith("file")) {
		OnPropertiesSaved_File(Callback, Url);
	} else {
		VeLogErrorFunc("Unsupported url protocol: %s", *Url);
		SetPlaceableState(EPlaceableState::None);
	}
}

void APlaceableGltfUrl::OnPropertiesSaved_OpenSeaHttp(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& Url) {
	GET_MODULE_SUBSYSTEM(ChunkDownloadSubsystem, Download, Chunk);
	if (!ChunkDownloadSubsystem) {
		InCallback->ExecuteIfBound(false, TEXT(""));
		return;
	}

	TArray<FString> Tokens;
	Url.ParseIntoArray(Tokens,TEXT("/"), true);
	if (Tokens.Num() < 3) {
		const FString Error = FString::Printf(TEXT("Failed to request: %s"), *Url);
		VeLogErrorFunc("%s", *Error);
		InCallback->ExecuteIfBound(false, Error);
		return;
	}

	const int32 ContractAddressIndex = Tokens.Num() - 2;
	const int32 TokenIdIndex = Tokens.Num() - 1;
	const auto ContractAddress = Tokens[ContractAddressIndex];

	if (!ContractAddress.StartsWith(TEXT("0x"))) {
		const FString Error = FString::Printf(TEXT("Failed to request: %s"), *Url);
		VeLogErrorFunc("%s", *Error);
		InCallback->ExecuteIfBound(false, Error);
		return;
	}

	const auto TokenId = Tokens[TokenIdIndex];
	GET_MODULE_SUBSYSTEM(OpenSeaAssetSubsystem, OpenSea, Asset);
	if (!OpenSeaAssetSubsystem) {
		InCallback->ExecuteIfBound(false, TEXT(""));
		return;
	}

	// 1. Create a callback.
	const auto OpenSeaAssetCallbackPtr = MakeShared<FOnOpenSeaAssetRequestCompleted>();

	// 2. Bind callback.
	OpenSeaAssetCallbackPtr->BindWeakLambda(this, [=](const FOpenSeaAssetMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
		if (bInSuccessful) {
			ReplacePlaceableFile(InCallback, Url, InMetadata.MimeType);
		} else {
			VeLogErrorFunc("Failed to request: %s", *InError);
			InCallback->ExecuteIfBound(false, InError);
		}
	});

	// 3. Make the request using the callback.
	OpenSeaAssetSubsystem->GetAsset(ContractAddress, TokenId, OpenSeaAssetCallbackPtr);
}

void APlaceableGltfUrl::OnPropertiesSaved_Http(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& Url) {
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
			VeLogErrorFunc("failed to fetch the file: %s", *InError);
			InCallback->ExecuteIfBound(bInSuccessful, InError);
		}
	});

	ChunkDownloadSubsystem->GetFileHeaders(Url, CallbackPtr);
}

void APlaceableGltfUrl::ReplacePlaceableFile(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& InUrl, const FString& InMimeType) {
	GET_MODULE_SUBSYSTEM(FileSubsystem, Api2, File);
	if (!FileSubsystem) {
		InCallback->ExecuteIfBound(false, TEXT(""));
		return;
	}

	// Validate mime type.
	if (!SupportedMimeTypesGltf.Contains(InMimeType)) {
		VeLogErrorFunc("Unsupported mime type: %s", *InMimeType);
		InCallback->ExecuteIfBound(false, FString::Printf(TEXT("unsupported mime type: %s"), *InMimeType));
		return;
	}

	const auto Callback = MakeShared<FOnFileRequestCompleted>();
	Callback->BindWeakLambda(this, [=](const FApiFileMetadata InFileMetadata, const bool bInSuccessful, const FString& InError) {
		if (bInSuccessful) {
			GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
			if (NotificationSubsystem) {
				FUINotificationData NotificationData;
				FglTFRuntimeConfig Config;
				Config.bAllowExternalFiles = true;
				NotificationData.Type = EUINotificationType::Success;
				NotificationData.Header = LOCTEXT("DownloadglTFHeader", "Upload glTF mesh started");
				NotificationData.Message = FText::Format(LOCTEXT("DownloadglTFMessage", "Loaded glTF mesh: {0}"), FText::FromString(InFileMetadata.Url));
				NotificationSubsystem->AddNotification(NotificationData);
			}
		} else {
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

void APlaceableGltfUrl::OnPropertiesSaved_File(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& Url) {
	// Process file protocol URLs and upload file. Will automatically handle supported file formats such as PDF documents, images, mp4 videos.
	GET_MODULE_SUBSYSTEM(FileSubsystem, Api2, File);
	if (!FileSubsystem) {
		InCallback->ExecuteIfBound(false, TEXT(""));
		return;
	}

	// Get file mime type.
	const auto MimeType = UPlaceableLib::GetFileMimeType(Url);

	// Validate mime type.
	if (!SupportedMimeTypesGltf.Contains(MimeType)) {
		VeLogErrorFunc("Unsupported mime type: %s", *MimeType);
		InCallback->ExecuteIfBound(false, FString::Printf(TEXT("unsupported mime type: %s"), *MimeType));
		return;
	}

	// Upload placeable file.
	const auto CallbackPtr = MakeShared<FOnFileRequestCompleted>();
	CallbackPtr->BindWeakLambda(this, [=](const FApiFileMetadata& InFileMetadata, const bool bInSuccessful, const FString& InError) {
		if (bInSuccessful) {
			GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
			if (NotificationSubsystem) {
				FUINotificationData NotificationData;
				FglTFRuntimeConfig Config;
				Config.bAllowExternalFiles = true;
				NotificationData.Type = EUINotificationType::Success;
				NotificationData.Header = LOCTEXT("DownloadglTFHeader", "Upload glTF mesh started");
				NotificationData.Message = FText::Format(LOCTEXT("DownloadglTFMessage", "Loaded glTF mesh: {0}"), FText::FromString(InFileMetadata.Url));
				NotificationSubsystem->AddNotification(NotificationData);
			}
		} else {
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

void APlaceableGltfUrl::OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange) {
	// Collision for dedicated server
	// if (IsRunningDedicatedServer()) return;

	if (const auto* FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
		return InFileMetadata.Type == EApiFileType::Model;
	})) {
		GltfMeshComponent->SetAssetUrl(FileMetadata->Url);
	} else {
		LogWarningF("Failed to get a file");
	}
}

UUserWidget* APlaceableGltfUrl::GetInspectPreviewWidget_Implementation(APlayerController* PlayerController) {
	const auto& Metadata = GetPlaceableMetadata();

	// MODEL
	{
		const auto* FileMetadata = Metadata.Files.FindByPredicate([](const FVeFileMetadata& File) {
			return File.Type == EApiFileType::Model;
		});
		if (FileMetadata) {
			if (!IsValid(InspectableModelPreviewClass)) {
				return nullptr;
			}

			auto* InspectPreviewWidget = CreateWidget<UUIInspectorModelPreviewWidget>(PlayerController, InspectableModelPreviewClass);
			if (!IsValid(InspectPreviewWidget)) {
				VeLogErrorFunc("Failed to create a preview widget instance");
				return nullptr;
			}

			return InspectPreviewWidget;
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
