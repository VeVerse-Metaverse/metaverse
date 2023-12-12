// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Subsystems/UIFocusSubsystem.h"
#include "UIKeyboardWidget.h"
#include "Framework/Application/SlateUser.h"

const FName FUIFocusSubsystem::Name = FName("UIFocusSubsystem");


void FUIFocusSubsystem::Initialize() {
}

void FUIFocusSubsystem::Shutdown() {
	FocusedWidget = nullptr;
}

void FUIFocusSubsystem::SetFocusedWidget(UUIWidgetBase* InFocusedWidget) {
	if (OnFocusedWidgetChanged.IsBound()) {
		OnFocusedWidgetChanged.Broadcast(FocusedWidget, InFocusedWidget);
	}
	FocusedWidget = InFocusedWidget;
}

UUIWidgetBase* FUIFocusSubsystem::GetFocusedWidget() const {
	return FocusedWidget;
}

void FUIFocusSubsystem::RestoreFocus() const {
	if (IsValid(LastFocusedWidget)) {
		LastFocusedWidget->SetWidgetKeyboardFocus();
	}
}

void FUIFocusSubsystem::RegisterKeyboard(UUIKeyboardWidget* InKeyboardWidget) {
	RegisteredKeyboardWidgets.Add(InKeyboardWidget);
}

void FUIFocusSubsystem::UnregisterKeyboard(const UUIKeyboardWidget* InKeyboardWidget) {
	RegisteredKeyboardWidgets.Remove(InKeyboardWidget);
}

bool FUIFocusSubsystem::CheckIfKeyboardIsFocused(const uint32 InSlateUserIndex) {
	if (!FSlateApplication::IsInitialized()) {
		return false;
	}

	const TSharedPtr<FSlateUser> User = FSlateApplication::Get().GetUser(InSlateUserIndex);

	if (User) {
		for (UUIKeyboardWidget* Keyboard : RegisteredKeyboardWidgets) {
			if (User->IsWidgetInFocusPath(Keyboard->TakeWidget())) {
				return true;
			}
		}
	}

	return false;
}
