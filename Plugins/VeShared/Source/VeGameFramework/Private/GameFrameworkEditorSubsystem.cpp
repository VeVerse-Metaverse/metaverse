// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "GameFrameworkEditorSubsystem.h"

#include "EditorComponent.h"
#include "VeApi.h"
#include "VeShared.h"
#include "VeGameFramework.h"
#include "Kismet/GameplayStatics.h"
// #include "EditorComponent.h"
#include "PlaceableComponent.h"


const FName FGameFrameworkEditorSubsystem::Name = TEXT("VeGameEditor");

void FGameFrameworkEditorSubsystem::Initialize() {}

void FGameFrameworkEditorSubsystem::Shutdown() {}

void FGameFrameworkEditorSubsystem::SetEditorComponent(UEditorComponent* InEditorComponent) {
	if (EditorComponent == InEditorComponent) {
		return;
	}

	auto OldEditorComponent = EditorComponent;
	EditorComponent = InEditorComponent;

	OnEditorComponentChanged.Broadcast(EditorComponent.Get(), OldEditorComponent.Get());
}

void FGameFrameworkEditorSubsystem::ResetEditorComponent() {
	if (!EditorComponent.IsValid()) {
		return;
	}

	auto OldEditorComponent = EditorComponent;
	EditorComponent.Reset();

	OnEditorComponentChanged.Broadcast(nullptr, OldEditorComponent.Get());
}

void FGameFrameworkEditorSubsystem::SetIsEditorModeEnabled(bool Value) {
	if (bIsEditorModeEnabled == Value) {
		return;
	}
	
	bIsEditorModeEnabled = Value;

	if (Value) {
		OnEditorModeEnabled.Broadcast();
	} else {
		OnEditorModeDisabled.Broadcast();
	}
}
