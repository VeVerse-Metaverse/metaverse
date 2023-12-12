// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableArtObject.h"

#include "Api2ObjectSubsystem.h"
#include "ApiFileMetadata.h"
#include "VeApi.h"
#include "VeGameModule.h"
#include "VeShared.h"
#include "Components/WidgetComponent.h"
#include "PlaceableComponent.h"
#include "PlaceableLib.h"
#include "VeApi2.h"
#include "Net/UnrealNetwork.h"
#include "UI/UIInspectorImagePreviewWidget.h"
#include "UI/UIInspectorModelPreviewWidget.h"
#include "UI/UIInspectorArtObjectPropertiesWidget.h"
#include "UI/UIPlaceableArtObjectPropertiesWidget.h"
#include "EditorComponent.h"
#include "Components/ImageMeshComponent.h"
#include "Components/GltfMeshComponent.h"
#include "UI/UIPlaceableStateWidget.h"


APlaceableArtObject::APlaceableArtObject() {
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RootScene");
	RootComponent = RootSceneComponent;
	RootSceneComponent->SetIsReplicated(true);

	ImageMeshComponent = CreateDefaultSubobject<UImageMeshComponent>("ImageMesh");
	ImageMeshComponent->SetupAttachment(RootSceneComponent);
	ImageMeshComponent->SetIsVisible(true);

	GltfMeshComponent = CreateDefaultSubobject<UGltfMeshComponent>("GltfMesh");
	GltfMeshComponent->SetupAttachment(RootSceneComponent);
	GltfMeshComponent->SetIsVisible(false);
}

void APlaceableArtObject::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (PlaceableComponent) {
		PlaceableComponent->GetOnPlaceableMetadataChanged().AddUObject(this, &APlaceableArtObject::OnPlaceableMetadataChanged);
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

void APlaceableArtObject::OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange) {

	const auto& EntityId = GetPlaceableMetadata().EntityId;

	GET_MODULE_SUBSYSTEM(ObjectSubsystem, Api2, Object);
	if (ObjectSubsystem) {
		const auto CallbackPtr = MakeShared<FOnObjectRequestCompleted2>();
		CallbackPtr->BindWeakLambda(this, [=](const FApiObjectMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			SetPlaceableState(EPlaceableState::None);
			SetObjectMetadata(InMetadata);

			if (InResponseCode != EApi2ResponseCode::Ok) {
				VeLogErrorFunc("Failed to receive metadata: %s", *InError);
				return;
			}
		});

		SetPlaceableState(EPlaceableState::Downloading);
		ObjectSubsystem->Get(EntityId, CallbackPtr);
	}
}

void APlaceableArtObject::SetObjectMetadata(const FApiObjectMetadata& InMetadata) {
	ArtObjectMetadata = InMetadata;

	// MODEL
	{
		const auto* FileMetadata = ArtObjectMetadata.Files.FindByPredicate([](const FApiFileMetadata& File) {
			return File.Type == EApiFileType::Model;
		});
		if (FileMetadata) {
			GltfMeshComponent->SetIsVisible(true);
			ImageMeshComponent->SetIsVisible(false);

			GltfMeshComponent->ScaleMultiplier(InMetadata.ScaleMultiplier);
			GltfMeshComponent->SetAssetUrl(FileMetadata->Url);
			return;
		}
	}

	// IMAGE
	{
		GltfMeshComponent->ClearAsset();
		GltfMeshComponent->SetIsVisible(false);
		ImageMeshComponent->SetIsVisible(true);

		ImageMeshComponent->SetSize(InMetadata.Height, InMetadata.Width);

		if (IsRunningDedicatedServer()) {
			return;
		}

		UpdatePlaceableProperties();

		const TArray<EApiFileType> Types = {
			EApiFileType::ImageFull,
			EApiFileType::ImagePreview,
			EApiFileType::TextureDiffuse,
		};

		if (auto* FileMetadata = FindFileMetadata(ArtObjectMetadata.Files, Types)) {
			ImageMeshComponent->ShowImage(FileMetadata->Url, TEXTUREGROUP_World);
		}
	}
}

UUIPlaceablePropertiesWidget* APlaceableArtObject::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	PlaceableProperties = CreateWidget<UUIPlaceableArtObjectPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!PlaceableProperties.IsValid()) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	UpdatePlaceableProperties();

	PlaceableProperties->GetOnSave().BindWeakLambda(this, [=] {
		OnPropertiesSaved(PlaceableProperties->GetMetadata());
	});

	return PlaceableProperties.Get();
}

void APlaceableArtObject::UpdatePlaceableProperties() {
	if (PlaceableProperties.IsValid()) {
		PlaceableProperties->SetEntityId(GetPlaceableMetadata().EntityId);
	}
}

void APlaceableArtObject::OnPropertiesSaved(const FApiObjectMetadata& InMetadata) {
	GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api, Placeable);
	if (PlaceableSubsystem) {
		const auto PlaceableCallback = MakeShared<FOnPlaceableRequestCompleted>();
		PlaceableCallback->BindLambda([=](const FApiPlaceableMetadata& InPlaceableMetadata, const EApiResponseCode InResponseCode, const FString& InError) {
			SetPlaceableState(EPlaceableState::None);

			if (InResponseCode != EApiResponseCode::Ok) {
				VeLogErrorFunc("failed to update placeable entity: %s", *InError);
				return;
			}

			UpdatePlaceableOnServer();
		});

		// Set EntityId for Placeable
		const auto& PlaceableId = GetPlaceableMetadata().Id;
		const auto& EntityId = InMetadata.Id;

		// UpdateSize(InMetadata);
		SetPlaceableState(EPlaceableState::Uploading);
		PlaceableSubsystem->UpdatePlaceableEntity(PlaceableId, EntityId, PlaceableCallback);
	}
}

UUserWidget* APlaceableArtObject::GetInspectPreviewWidget_Implementation(APlayerController* PlayerController) {
	const auto& Metadata = ArtObjectMetadata;

	// MODEL
	{
		const auto* FileMetadata = Metadata.Files.FindByPredicate([](const FApiFileMetadata& File) {
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

	// IMAGE
	{
		if (!IsValid(InspectableImagePreviewClass)) {
			return nullptr;
		}

		const TArray<EApiFileType> Types = {
			EApiFileType::ImageFull,
			EApiFileType::ImagePreview,
			EApiFileType::TextureDiffuse,
		};

		auto* FileMetadata = FindFileMetadata(Metadata.Files, Types);

		if (!FileMetadata) {
			VeLogWarningFunc("Failed to get a file");
			return nullptr;
		}

		auto* InspectPreviewWidget = CreateWidget<UUIInspectorImagePreviewWidget>(PlayerController, InspectableImagePreviewClass);
		if (!IsValid(InspectPreviewWidget)) {
			VeLogErrorFunc("Failed to create a preview widget instance");
			return nullptr;
		}

		InspectPreviewWidget->SetUrl(FileMetadata->Url);

		return InspectPreviewWidget;
	}
}

UUserWidget* APlaceableArtObject::GetInspectPropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(InspectablePropertiesClass)) {
		return nullptr;
	}

	const auto& Metadata = ArtObjectMetadata;

	const TArray<EApiFileType> Types = {
		EApiFileType::Model,
		EApiFileType::ImageFull,
		EApiFileType::ImagePreview,
		EApiFileType::TextureDiffuse,
	};

	auto* FileMetadata = FindFileMetadata(Metadata.Files, Types);

	if (!FileMetadata) {
		VeLogWarningFunc("Failed to get a file");
		return nullptr;
	}

	auto* InspectPropertiesWidget = CreateWidget<UUIInspectorArtObjectPropertiesWidget>(PlayerController, InspectablePropertiesClass);
	if (!IsValid(InspectPropertiesWidget)) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	InspectPropertiesWidget->SetProperties(Metadata);

	return InspectPropertiesWidget;
}
