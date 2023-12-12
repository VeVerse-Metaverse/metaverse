// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "InspectableActor.h"

#include "VeApi.h"
#include "VeShared.h"
#include "VeGameModule.h"
#include "UI/UIUrlPropertiesWidget.h"
#include "VeGameFramework.h"

AInspectableActor::AInspectableActor()
	: Super() {}

FVector AInspectableActor::GetInspectableForwardVector_Implementation() {
	return bUseCustomForwardVector ? GetActorTransform().TransformPositionNoScale(CustomForwardVector).GetSafeNormal() : GetActorForwardVector();
}
