// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "PlaceableActor.h"

#include "VeShared.h"
#include "PlaceableComponent.h"
#include "VeGameModule.h"
#include "UI/UIUrlPropertiesWidget.h"
#include "VeGameFramework.h"
#include "GameFrameworkEditorSubsystem.h"
#include "EditorComponent.h"


APlaceableActor::APlaceableActor() : Super() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	PlaceableComponent = CreateDefaultSubobject<UPlaceableComponent>(TEXT("PlaceableComponent"));
}

void APlaceableActor::PreInitializeComponents() {
	Super::PostInitializeComponents();
	if (RootComponent) {
		RootComponent->SetIsReplicated(true);
	}
}

const FVePlaceableMetadata& APlaceableActor::GetPlaceableMetadata() const {
	return PlaceableComponent->GetMetadata();
}

void APlaceableActor::UpdatePlaceableOnServer() {
	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		if (auto* EditorComponent = EditorSubsystem->GetEditorComponent(); IsValid(EditorComponent)) {
			EditorComponent->Owner_UpdatePlaceables({this});
		}
	}
}

void APlaceableActor::SetPlaceableState(EPlaceableState State, float Ratio) {
	SetPlaceableStateHelper(State, Ratio);
}
