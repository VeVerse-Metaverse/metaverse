// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableOpenSeaNft.h"

#include "VeGameModule.h"
#include "VeShared.h"
#include "Components/WidgetComponent.h"
#include "PlaceableComponent.h"
#include "PlaceableLib.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Api2PlaceableSubsystem.h"
#include "Api2OpenSeaSubsystem.h"
#include "ApiFileMetadata.h"
#include "UI/UIInspectorImagePreviewWidget.h"
#include "EditorComponent.h"
#include "Components/ImageMeshComponent.h"
#include "Components/GltfMeshComponent.h"
#include "UI/UIEditorOpenSeaAssetsPropertiesWidget.h"
#include "UI/UIInspectorModelPreviewWidget.h"
#include "UI/UIInspectorOpenSeaAssetPropertiesWidget.h"
#include "UI/UIPlaceableStateWidget.h"


APlaceableOpenSeaNft::APlaceableOpenSeaNft() {
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
void APlaceableOpenSeaNft::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (PlaceableComponent) {
		PlaceableComponent->GetOnPlaceableMetadataChanged().AddUObject(this, &APlaceableOpenSeaNft::OnPlaceableMetadataChanged);
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

void APlaceableOpenSeaNft::OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange) {

	FGuid EntityId;
	if (GetPlaceableMetadata().EntityId.IsValid()) {
		EntityId = GetPlaceableMetadata().EntityId;
	}
	
	GET_MODULE_SUBSYSTEM(OpenSeaSubsystem, Api2, OpenSea);

	if (OpenSeaSubsystem) {
		// 1. Callback
		TSharedRef<FOnOpenSeaRequestCompleted> CallbackPtr = MakeShareable(new FOnOpenSeaRequestCompleted{});

		// 2. Lambda
		CallbackPtr->BindWeakLambda(this, [=](const FApiOpenSeaAssetMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) mutable {
			if (InResponseCode != EApi2ResponseCode::Ok) {
				LogErrorF("failed to request opensea asset: %s", *InError);
			}

			if (InResponseCode == EApi2ResponseCode::Ok) {
				SetOpenSeaAssetMetadata(InMetadata);
			}

		});

		// 3. Request
		OpenSeaSubsystem->Get(EntityId, CallbackPtr);
	}
}

void APlaceableOpenSeaNft::SetOpenSeaAssetMetadata(const FApiOpenSeaAssetMetadata& InMetadata) {
	OpenSeaAssetMetadata = InMetadata;

	// MODEL
	{
		if (InMetadata.MimeType == TEXT("model/gltf-binary")) {
			GltfMeshComponent->SetIsVisible(true);
			ImageMeshComponent->SetIsVisible(false);

			GltfMeshComponent->ScaleMultiplier(1.0f);
			GltfMeshComponent->SetAssetUrl(InMetadata.AnimationUrl);
			return;
		}
	}

	// IMAGE
	{
		GltfMeshComponent->ClearAsset();
		GltfMeshComponent->SetIsVisible(false);
		ImageMeshComponent->SetIsVisible(true);

		if (IsRunningDedicatedServer()) {
			return;
		}

		UpdatePlaceableProperties();

		const TArray<EApiFileType> Types = {
			EApiFileType::ImageFull,
			EApiFileType::ImagePreview,
			EApiFileType::TextureDiffuse,
		};


		if (!InMetadata.ImageUrl.IsEmpty()) {
			ImageMeshComponent->ShowImage(InMetadata.ImageUrl, TEXTUREGROUP_World);
		}
	}
}

UUIPlaceablePropertiesWidget* APlaceableOpenSeaNft::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	PlaceableProperties = CreateWidget<UUIEditorOpenSeaAssetsPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
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

void APlaceableOpenSeaNft::UpdatePlaceableProperties() {
	if (PlaceableProperties.IsValid()) {
		PlaceableProperties->SetEntityId(GetPlaceableMetadata().EntityId);
	}
}

void APlaceableOpenSeaNft::OnPropertiesSaved(const FApiOpenSeaAssetMetadata& InMetadata) {
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

UUserWidget* APlaceableOpenSeaNft::GetInspectPreviewWidget_Implementation(APlayerController* PlayerController) {
	const auto& Metadata = OpenSeaAssetMetadata;

	//MODEL
	{
		if (Metadata.MimeType == TEXT("model/gltf-binary")) {

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
		
		auto* InspectPreviewWidget = CreateWidget<UUIInspectorImagePreviewWidget>(PlayerController, InspectableImagePreviewClass);
		if (!IsValid(InspectPreviewWidget)) {
			VeLogErrorFunc("Failed to create a preview widget instance");
			return nullptr;
		}

		InspectPreviewWidget->SetUrl(OpenSeaAssetMetadata.ImageUrl);

		return InspectPreviewWidget;
	}
}

UUserWidget* APlaceableOpenSeaNft::GetInspectPropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(InspectablePropertiesClass)) {
		return nullptr;
	}

	const auto& Metadata = OpenSeaAssetMetadata;

	const TArray<EApiFileType> Types = {
		EApiFileType::Model,
		EApiFileType::ImageFull,
		EApiFileType::ImagePreview,
		EApiFileType::TextureDiffuse,
	};

	auto* InspectPropertiesWidget = CreateWidget<UUIInspectorOpenSeaAssetPropertiesWidget>(PlayerController, InspectablePropertiesClass);
	if (!IsValid(InspectPropertiesWidget)) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	InspectPropertiesWidget->SetProperties(Metadata);

	return InspectPropertiesWidget;
}
