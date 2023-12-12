// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeUI.h"
#include "Modules/ModuleManager.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "Subsystems/UICharacterCustomizationSubsystem.h"
#include "Subsystems/UIDialogSubsystem.h"
#include "Subsystems/UIFocusSubsystem.h"
#include "Subsystems/UINavigationSubsystem.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Subsystems/UIPageSubsystem.h"

IMPLEMENT_GAME_MODULE(FVeUIModule, VeUI);
VEUI_API DEFINE_LOG_CATEGORY(LogVeUI);

void FVeUIModule::StartupModule() {
	AddSubsystem(FUIBreadcrumbSubsystem::Name, MakeShared<FUIBreadcrumbSubsystem>());
	AddSubsystem(FUIFocusSubsystem::Name, MakeShared<FUIFocusSubsystem>());
	AddSubsystem(FUIDialogSubsystem::Name, MakeShared<FUIDialogSubsystem>());
	AddSubsystem(FUINotificationSubsystem::Name, MakeShared<FUINotificationSubsystem>());
	AddSubsystem(FUINavigationSubsystem::Name, MakeShared<FUINavigationSubsystem>());
	AddSubsystem(FUICharacterCustomizationSubsystem::Name, MakeShared<FUICharacterCustomizationSubsystem>());
	AddSubsystem(FUIPageSubsystem::Name, MakeShared<FUIPageSubsystem>());

	ISubsystemModule::StartupModule();
}

void FVeUIModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

FVeUIModule* FVeUIModule::Get() {
	if (FModuleManager::Get().IsModuleLoaded(TEXT("VeUI"))) {
		return FModuleManager::Get().GetModulePtr<FVeUIModule>(TEXT("VeUI"));
	}
	return FModuleManager::Get().LoadModulePtr<FVeUIModule>(TEXT("VeUI"));
}

FUIBreadcrumbSubsystemPtr FVeUIModule::GetBreadcrumbSubsystem() {
	return FindSubsystem<FUIBreadcrumbSubsystem>(FUIBreadcrumbSubsystem::Name);
}

FUIFocusSubsystemPtr FVeUIModule::GetFocusSubsystem() {
	return FindSubsystem<FUIFocusSubsystem>(FUIFocusSubsystem::Name);
}

TSharedPtr<FUIDialogSubsystem> FVeUIModule::GetDialogSubsystem() {
	return FindSubsystem<FUIDialogSubsystem>(FUIDialogSubsystem::Name);
}

TSharedPtr<FUINotificationSubsystem> FVeUIModule::GetNotificationSubsystem() {
	return FindSubsystem<FUINotificationSubsystem>(FUINotificationSubsystem::Name);
}

TSharedPtr<FUINavigationSubsystem> FVeUIModule::GetNavigationSubsystem() {
	return FindSubsystem<FUINavigationSubsystem>(FUINavigationSubsystem::Name);
}

TSharedPtr<FUICharacterCustomizationSubsystem> FVeUIModule::GetCharacterCustomizationSubsystem() {
	return FindSubsystem<FUICharacterCustomizationSubsystem>(FUICharacterCustomizationSubsystem::Name);
}

TSharedPtr<FUIPageSubsystem> FVeUIModule::GetPageSubsystem() {
	return FindSubsystem<FUIPageSubsystem>(FUIPageSubsystem::Name);
}
