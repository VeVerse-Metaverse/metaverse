// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Subsystems/UIPageSubsystem.h"

#include "UIPageManagerWidget.h"


const FName FUIPageSubsystem::Name = FName("UIPageSubsystem");

void FUIPageSubsystem::Initialize() {
}

void FUIPageSubsystem::Shutdown() {
}

UUIPageManagerWidget* FUIPageSubsystem::GetPageManagerUI() const {
	return PageManagerUI.Get();
}

void FUIPageSubsystem::SetPageManagerUI(UUIPageManagerWidget* InPageManager) {
	PageManagerUI = InPageManager;
}

#if WITH_EDITOR
UUIPageManagerWidget* FUIPageSubsystem::GetPageManagerSDK() const {
	return PageManagerSDK.Get();
}

void FUIPageSubsystem::SetPageManagerSDK(UUIPageManagerWidget* InPageManager) {
	PageManagerSDK = InPageManager;
}
#endif
