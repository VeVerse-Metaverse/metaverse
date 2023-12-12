// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableUserObject.h"

#include "PlaceableComponent.h"
#include "VeGameModule.h"
#include "VeShared.h"
#include "UI/UIPlaceablePropertiesWidget.h"

APlaceableUserObject::APlaceableUserObject() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Scene");
	if (SceneComponent) {
		RootComponent = SceneComponent;
		SceneComponent->SetIsReplicated(true);
	}
}

void APlaceableUserObject::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (PlaceableComponent) {
		PlaceableComponent->GetOnPlaceableMetadataChanged().AddUObject(this, &APlaceableUserObject::OnPlaceableMetadataChanged);
	}

	// todo: temporary, research purposes
	for (TFieldIterator<FProperty> It(GetClass()); It; ++It) {
		auto* Property = (*It);
		auto* PropertyClass = Property->GetClass();

		// Temporary: for client compile
#if 0
		if (Property->GetMetaData("Category") != "User") {
			continue;
		}

		if (PropertyClass == FBoolProperty::StaticClass()) {
			// todo: boolean property
		} else if (PropertyClass == FIntProperty::StaticClass()) {
			// todo: int property
		} else if (PropertyClass == FFloatProperty::StaticClass()) {
			// todo: float property
		} else if (PropertyClass == FStrProperty::StaticClass()) {
			// todo: string property
		} else if (PropertyClass == FStructProperty::StaticClass()) {
			// todo: struct property
			auto* StructProperty = CastField<FStructProperty>(Property);
			auto* StructPropertyClass = StructProperty->GetClass();
			if (StructPropertyClass->GetName() == TEXT("Vector")) {
				// todo: vector property
			} else if (StructPropertyClass->GetName() == TEXT("Rotator")) {
				// todo: rotator property
			}
		}
#endif

		// todo: object properties?

		FString Name = Property->GetName();
		FString ClassName = Property->GetClass()->GetName();

		if (Property->GetClass() == FStructProperty::StaticClass()) {
			auto* StructProperty = CastField<FStructProperty>(Property);
			ClassName = StructProperty->Struct->GetName();
		}

		VeLogWarningFunc("Property: %s of class %s", *Name, *ClassName);
	}
}

UUIPlaceablePropertiesWidget* APlaceableUserObject::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIPlaceablePropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!PropertiesWidget) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	return PropertiesWidget;
}

void APlaceableUserObject::OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange) {}
