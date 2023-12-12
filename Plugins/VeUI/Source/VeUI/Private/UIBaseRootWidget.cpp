// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIBaseRootWidget.h"


void UUIBaseRootWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
	NativeOnHide();
}

void UUIBaseRootWidget::Show() {
	NativeOnShow();
}

void UUIBaseRootWidget::Hide() {
	NativeOnHide();
}

void UUIBaseRootWidget::UpdateInputMode() {
	NativeOnUpdateInputMode();
}

void UUIBaseRootWidget::SetInputModeUiOnly(UWidget* InWidgetToFocus) {
	auto* PlayerController = GetOwningPlayer();
	if (!IsValid(PlayerController)) {
		return;
	}

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);

	if (InWidgetToFocus == nullptr) {
		InputMode.SetWidgetToFocus(TakeWidget());
	} else {
		InputMode.SetWidgetToFocus(InWidgetToFocus->TakeWidget());
	}

	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);

	PlayerController->SetIgnoreMoveInput(true);
}

void UUIBaseRootWidget::SetInputModeGameAndUi() {
	auto* PlayerController = GetOwningPlayer();
	if (!IsValid(PlayerController)) {
		return;
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(true);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);

	PlayerController->ResetIgnoreMoveInput();
}

void UUIBaseRootWidget::SetInputModeGame() {
	auto* PlayerController = GetOwningPlayer();
	if (!IsValid(PlayerController)) {
		return;
	}

	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);

	PlayerController->ResetIgnoreMoveInput();
}

void UUIBaseRootWidget::NativeOnShow() {
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUIBaseRootWidget::NativeOnHide() {
	SetVisibility(ESlateVisibility::Collapsed);
}

void UUIBaseRootWidget::NativeOnClose() {
	OnClose.Broadcast(this);
}

void UUIBaseRootWidget::NativeOnUpdateInputMode() {
	SetInputModeUiOnly();
}
