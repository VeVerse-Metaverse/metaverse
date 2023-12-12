// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UINavigatableWidget.h"

#include "Subsystems/UINavigationSubsystem.h"
#include "VeUI.h"

void UUINavigatableWidget::Register() {
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUINavigationSubsystem> NavigationSubsystem = UIModule->GetNavigationSubsystem()) {
			NavigationSubsystem->RegisterWidget(TEXT("mods"), this);
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Navigation)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
}

FString UUINavigatableWidget::GetPath() {
	return {};
}

void UUINavigatableWidget::Navigate(const FString& Options) {
}
