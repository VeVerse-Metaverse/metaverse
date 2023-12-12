// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableShape.h"

#include "VeShared.h"
#include "VeGameModule.h"
#include "UI/UIPlaceableShapePropertiesWidget.h"
#include "PlaceableComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "VeApi.h"
#include "ApiPropertyRequest.h"
#include "ApiPropertySubsystem.h"

APlaceableShape::APlaceableShape() : Super() {
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	SceneComponent->SetIsReplicated(true);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(SceneComponent);
}

void APlaceableShape::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (IsRunningDedicatedServer()) {
		return;
	}

	PlaceableComponent->GetOnPlaceableMetadataChanged().AddWeakLambda(this, [=](const FVePlaceableMetadata& InMetadata, EPlaceableChangeType InChange) {
		OnPlaceableMetadataUpdated(InMetadata);
	});

	UMaterialInterface* Material = StaticMeshComponent->GetMaterial(0);
	DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
	StaticMeshComponent->SetMaterial(0, DynMaterial);
}

void APlaceableShape::OnPlaceableMetadataUpdated(const FVePlaceableMetadata& InMetadata) {
	DefaultMaterialParams = FPlaceableShapeMaterialParams();

	for (auto& Property : GetPlaceableMetadata().Properties) {
		if (Property.Name == TEXT("BaseColor")) {
			DefaultMaterialParams.BaseColor = FColor::FromHex(Property.Value);
			continue;
		}
		if (Property.Name == TEXT("Metallic")) {
			DefaultMaterialParams.Metallic = FCString::Atof(*Property.Value);
			continue;
		}
		if (Property.Name == TEXT("Roughness")) {
			DefaultMaterialParams.Roughness = FCString::Atof(*Property.Value);
			continue;
		}
	}

	SetMaterialparams(DefaultMaterialParams);
}

void APlaceableShape::SetMaterialparams(FPlaceableShapeMaterialParams InMaterialParams) {
	CurrentMaterialParams = InMaterialParams;
	DynMaterial->SetVectorParameterValue(TEXT("BaseColor"), InMaterialParams.BaseColor);
	DynMaterial->SetScalarParameterValue(TEXT("Metallic"), InMaterialParams.Metallic);
	DynMaterial->SetScalarParameterValue(TEXT("Roughness"), InMaterialParams.Roughness);
}

UUIPlaceablePropertiesWidget* APlaceableShape::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIPlaceableShapePropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!IsValid(PropertiesWidget)) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	PropertiesWidget->SetProperties(CurrentMaterialParams.BaseColor, CurrentMaterialParams.Metallic, CurrentMaterialParams.Roughness);

	PropertiesWidget->GetOnSave().BindWeakLambda(this, [=]() {
		Properties_OnSave(PropertiesWidget);
	});

	PropertiesWidget->GetOnReset().BindWeakLambda(this, [=]() {
		Properties_OnReset(PropertiesWidget);
	});

	PropertiesWidget->GetOnChanged().BindWeakLambda(this, [=]() {
		Properties_OnChanged(PropertiesWidget);
	});

	return PropertiesWidget;
}

void APlaceableShape::Properties_OnSave(UUIPlaceableShapePropertiesWidget* PropertiesWidget) {
	GET_MODULE_SUBSYSTEM(PropertySubsystem, Api, Property);
	if (PropertySubsystem) {
		SetPlaceableState(EPlaceableState::Uploading);

		TArray<FApiUpdatePropertyMetadata> Properties;
		Properties.Emplace(
			TEXT("BaseColor"),
			TEXT("color"),
			CurrentMaterialParams.BaseColor.ToHex()
			);
		Properties.Emplace(
			TEXT("Metallic"),
			TEXT("float"),
			FString::Printf(TEXT("%f"), CurrentMaterialParams.Metallic)
			);
		Properties.Emplace(
			TEXT("Roughness"),
			TEXT("float"),
			FString::Printf(TEXT("%f"), CurrentMaterialParams.Roughness)
			);

		const auto Callback = MakeShared<FOnGenericRequestCompleted>();
		Callback->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& InError) {
			SetPlaceableState(EPlaceableState::None);
			if (!bInSuccessful) {
				VeLogErrorFunc("failed to update properties");
				return;
			}
			UpdatePlaceableOnServer();
		});

		PropertySubsystem->Update(GetPlaceableMetadata().Id, Properties, 0, Callback);
	}
}

void APlaceableShape::Properties_OnReset(UUIPlaceableShapePropertiesWidget* PropertiesWidget) {
	PropertiesWidget->SetProperties(DefaultMaterialParams.BaseColor, DefaultMaterialParams.Metallic, DefaultMaterialParams.Roughness);
}

void APlaceableShape::Properties_OnChanged(UUIPlaceableShapePropertiesWidget* PropertiesWidget) {
	FPlaceableShapeMaterialParams NewMaterialParams;
	NewMaterialParams.BaseColor = PropertiesWidget->GetBaseColor();
	NewMaterialParams.Metallic = PropertiesWidget->GetMetallic();
	NewMaterialParams.Roughness = PropertiesWidget->GetRoughness();
	SetMaterialparams(NewMaterialParams);
}
