// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIWidgetBase.h"

#include "Components/PanelWidget.h"
#include "VeHUD.h"
#include "UIPlayerController.h"
#include "VeWorldSubsystem.h"


void UUIWidgetBase::NativeConstruct() {
	Super::NativeConstruct();
	RegisterCallbacks();
}

void UUIWidgetBase::NativeDestruct() {
	Super::NativeDestruct();
	UnregisterCallbacks();
}

void UUIWidgetBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	OnMouseEnter.Broadcast();
}

void UUIWidgetBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseLeave(InMouseEvent);
	OnMouseLeave.Broadcast();
}

AVeHUD* UUIWidgetBase::GetHUD() const {
	if (const APlayerController* PlayerController = GetOwningPlayer()) {
		if (AHUD* HUD = PlayerController->GetHUD()) {
			return Cast<AVeHUD>(HUD);
		}
	}
	return nullptr;
}

bool UUIWidgetBase::Initialize() {
	const bool bResult = Super::Initialize();

	if (bResult && !bNativeInitialized) {
		NativeOnInitializedSdk();
	}

	return bResult;
}

void UUIWidgetBase::NativeOnInitialized() {
	Super::NativeOnInitialized();
	NativeOnInitializedShared();
	bNativeInitialized = true;
}

void UUIWidgetBase::NativeOnInitializedSdk() {
	NativeOnInitializedShared();
}

void UUIWidgetBase::NativeOnInitializedShared() {
	// Show();
}

UWorld* UUIWidgetBase::GetWorld() const {
#if WITH_EDITORONLY_DATA
	return FVeWorldSubsystem::GetWorld();
#else
	return Super::GetWorld();
#endif
}

void UUIWidgetBase::Show() {
	SetVisibility(VisibilityAfterFadeIn);
}

void UUIWidgetBase::Hide() {
	SetVisibility(VisibilityAfterFadeOut);
}

FTimerManager* UUIWidgetBase::GetTimerManager() const {
#if WITH_EDITORONLY_DATA

	if (const UWorld* const World = GetWorld()) {
		return &World->GetTimerManager();
	}
	// if (const auto* GameInstance = GetGameInstance()) {
	// 	return &GameInstance->GetTimerManager();
	// }
	if (IsValid(this) && IsValid(GEngine) && GEngine->IsEditor()) {
		auto* EditorEngine = Cast<UEditorEngine>(GEngine);
		if (IsValid(EditorEngine) && EditorEngine->IsTimerManagerValid()) {
			return &EditorEngine->GetTimerManager().Get();
		}
	}
	return nullptr;

#else
	
	if (const UWorld* const World = GetWorld()) {
		return &World->GetTimerManager();
	}
	return nullptr;

#endif
}
