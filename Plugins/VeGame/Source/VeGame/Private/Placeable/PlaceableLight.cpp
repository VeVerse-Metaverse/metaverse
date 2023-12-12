// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableLight.h"

#include "ApiPropertySubsystem.h"
#include "PlaceableComponent.h"
#include "VeGameModule.h"
#include "VeApi.h"
#include "VeShared.h"
#include "Components/PointLightComponent.h"
#include "UI/UIPlaceableLightPropertiesWidget.h"


// Sets default values
APlaceableLight::APlaceableLight() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>("CanvasMesh");
	if (PointLightComponent) {
		PointLightComponent->SetupAttachment(RootComponent);
		PointLightComponent->SetRelativeLocation(FVector::ZeroVector);
		PointLightComponent->SetCanEverAffectNavigation(false);
		PointLightComponent->SetIsReplicated(true);
	}
}

void APlaceableLight::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (IsRunningDedicatedServer()) {
		return;
	}

	PlaceableComponent->GetOnPlaceableMetadataChanged().AddWeakLambda(this, [=](const FVePlaceableMetadata& InMetadata, EPlaceableChangeType InChange) {
		OnPlaceableMetadataUpdated(InMetadata);
	});
}

void APlaceableLight::OnPlaceableMetadataUpdated(const FVePlaceableMetadata& InMetadata) {

	for (auto& Property : GetPlaceableMetadata().Properties) {
		if (Property.Name == TEXT("BaseColor")) {
			DefaultLightParams.BaseColor = FColor::FromHex(Property.Value);
			continue;
		}
		if (Property.Name == TEXT("Intensity")) {
			DefaultLightParams.Intensity = FCString::Atof(*Property.Value);
			continue;
		}
		if (Property.Name == TEXT("AttenuationRadius")) {
			DefaultLightParams.AttenuationRadius = FCString::Atof(*Property.Value);
			continue;
		}
	}

	SetLightParams(DefaultLightParams);
}

UUIPlaceablePropertiesWidget* APlaceableLight::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIPlaceableLightPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!IsValid(PropertiesWidget)) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	PropertiesWidget->SetProperties(CurrentLightParams.BaseColor, CurrentLightParams.Intensity, CurrentLightParams.AttenuationRadius);

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

void APlaceableLight::Properties_OnSave(UUIPlaceableLightPropertiesWidget* PropertiesWidget) {
	GET_MODULE_SUBSYSTEM(PropertySubsystem, Api, Property);
	if (PropertySubsystem) {
		SetPlaceableState(EPlaceableState::Uploading);

		TArray<FApiUpdatePropertyMetadata> Properties;
		Properties.Emplace(
			TEXT("BaseColor"),
			TEXT("color"),
			CurrentLightParams.BaseColor.ToHex()
			);
		Properties.Emplace(
			TEXT("Intensity"),
			TEXT("float"),
			FString::Printf(TEXT("%f"), CurrentLightParams.Intensity)
			);
		Properties.Emplace(
			TEXT("AttenuationRadius"),
			TEXT("float"),
			FString::Printf(TEXT("%f"), CurrentLightParams.AttenuationRadius)
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

void APlaceableLight::Properties_OnReset(UUIPlaceableLightPropertiesWidget* PropertiesWidget) {
	PropertiesWidget->SetProperties(DefaultLightParams.BaseColor, DefaultLightParams.Intensity, DefaultLightParams.AttenuationRadius);
}


void APlaceableLight::SetLightParams(FPlaceableLightParams InMaterialParams) {
	CurrentLightParams = InMaterialParams;
	PointLightComponent->SetLightColor(CurrentLightParams.BaseColor);
	PointLightComponent->SetIntensity(CurrentLightParams.Intensity);
	PointLightComponent->SetAttenuationRadius(CurrentLightParams.AttenuationRadius);
}


void APlaceableLight::Properties_OnChanged(UUIPlaceableLightPropertiesWidget* PropertiesWidget) {
	FPlaceableLightParams NewLightParams;
	NewLightParams.BaseColor = PropertiesWidget->GetBaseColor();
	NewLightParams.Intensity = PropertiesWidget->GetIntensity();
	NewLightParams.AttenuationRadius = PropertiesWidget->GetAttenuationRadius();
	SetLightParams(NewLightParams);
}
