// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableText.h"

#include "ApiPropertySubsystem.h"
#include "PlaceableComponent.h"
#include "VeGameModule.h"
#include "VeApi.h"
#include "VeShared.h"
#include "Components/TextRenderComponent.h"
#include "UI/UIPlaceableTextPropertiesWidget.h"


// Sets default values
APlaceableText::APlaceableText() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlaceableTextRenderClass = CreateDefaultSubobject<UTextRenderComponent>("TextComponent");
	if (PlaceableTextRenderClass) {
		PlaceableTextRenderClass->SetupAttachment(RootComponent);
		PlaceableTextRenderClass->SetRelativeLocation(FVector::ZeroVector);
		PlaceableTextRenderClass->SetCanEverAffectNavigation(false);
		PlaceableTextRenderClass->SetIsReplicated(true);
	}
}

void APlaceableText::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (IsRunningDedicatedServer()) {
		return;
	}

	PlaceableComponent->GetOnPlaceableMetadataChanged().AddWeakLambda(this, [=](const FVePlaceableMetadata& InMetadata, EPlaceableChangeType InChange) {
		OnPlaceableMetadataUpdated(InMetadata);
	});
}

void APlaceableText::OnPlaceableMetadataUpdated(const FVePlaceableMetadata& InMetadata) {

	for (auto& Property : GetPlaceableMetadata().Properties) {
		if (Property.Name == TEXT("BaseText")) {
			DefaultTextParams.Text = Property.Value;
			continue;
		}
		if (Property.Name == TEXT("TextRenderColor")) {
			DefaultTextParams.BaseColor = FColor::FromHex(Property.Value);
		}
	}

	SetTextParams(DefaultTextParams);
}

UUIPlaceablePropertiesWidget* APlaceableText::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIPlaceableTextPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!IsValid(PropertiesWidget)) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	// auto* TextWidget = CreateWidget<UUIPlaceableTextWidget>(PlayerController, PlaceableTextClass);
	// if (!IsValid(TextWidget)) {
	// 	VeLogErrorFunc("Failed to create a properties widget instance");
	// 	return nullptr;
	// }

	PropertiesWidget->SetProperties(CurrentTextParams.BaseColor, CurrentTextParams.Text);

	if (PlaceableTextRenderClass) {
		PlaceableTextRenderClass->SetText(FText::AsCultureInvariant(CurrentTextParams.Text));
		PlaceableTextRenderClass->SetTextRenderColor(CurrentTextParams.BaseColor);
	}

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

void APlaceableText::Properties_OnSave(UUIPlaceableTextPropertiesWidget* PropertiesWidget) {
	GET_MODULE_SUBSYSTEM(PropertySubsystem, Api, Property);
	if (PropertySubsystem) {
		SetPlaceableState(EPlaceableState::Uploading);

		TArray<FApiUpdatePropertyMetadata> Properties;
		Properties.Emplace(
			TEXT("BaseText"),
			TEXT("Text"),
			CurrentTextParams.Text
			);
		Properties.Emplace(
			TEXT("TextRenderColor"),
			TEXT("color"),
			CurrentTextParams.BaseColor.ToHex()
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

	if (PlaceableTextRenderClass) {
		PlaceableTextRenderClass->SetText(FText::AsCultureInvariant(CurrentTextParams.Text));
		PlaceableTextRenderClass->SetTextRenderColor(CurrentTextParams.BaseColor);
	}
}

void APlaceableText::Properties_OnReset(UUIPlaceableTextPropertiesWidget* PropertiesWidget) {
	PropertiesWidget->SetProperties(DefaultTextParams.BaseColor, DefaultTextParams.Text);
}


void APlaceableText::SetTextParams(FPlaceableTextParams InTextParams) {
	CurrentTextParams = InTextParams;
	
	if (PlaceableTextRenderClass) {
		PlaceableTextRenderClass->SetText(FText::AsCultureInvariant(CurrentTextParams.Text));
		PlaceableTextRenderClass->SetTextRenderColor(CurrentTextParams.BaseColor);
	}
}


void APlaceableText::Properties_OnChanged(UUIPlaceableTextPropertiesWidget* PropertiesWidget) {
	FPlaceableTextParams NewTextParams;
	NewTextParams.Text = PropertiesWidget->GetBaseText();
	NewTextParams.BaseColor = PropertiesWidget->GetBaseColor();
	SetTextParams(NewTextParams);
}
