// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "GameFrameworkPlaceableSubsystem.h"
#include "PlaceableComponent.h"


const FName FGameFrameworkPlaceableSubsystem::Name = FName("PlaceableSubsystem");

void FGameFrameworkPlaceableSubsystem::Initialize() { }

void FGameFrameworkPlaceableSubsystem::Shutdown() { }

void FGameFrameworkPlaceableSubsystem::SetSelected(AActor* Object) {
	
}

void FGameFrameworkPlaceableSubsystem::SetSelected(UPlaceableComponent* PlaceableComponent) {
	
}
