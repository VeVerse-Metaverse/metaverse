// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Subsystems/UINavigationSubsystem.h"

const FName FUINavigationSubsystem::Name = TEXT("UINavigationSubsystem");

void FUINavigationSubsystem::Initialize() {
}

void FUINavigationSubsystem::Shutdown() {
}

void FUINavigationSubsystem::Home() {
	// todo: implement
}

void FUINavigationSubsystem::Back() {
	// todo: implement
}

void FUINavigationSubsystem::Navigate(const FString& Path, const FString& Id) {
	// todo: implement

	// 	Widgets[Path].Navigate(Id);
}

void FUINavigationSubsystem::RegisterWidget(const FString& Path, UUINavigatableWidget* Widget) {
	// Path e.g. TEXT("mods")
	// Widget e.g. typeof UIModBrowserWidget*

	Widgets.Add(Path, Widget);
}
