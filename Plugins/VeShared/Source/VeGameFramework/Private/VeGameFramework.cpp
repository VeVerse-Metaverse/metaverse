// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeGameFramework.h"

#include "GameFrameworkEditorSubsystem.h"
#include "Modules/ModuleManager.h"
#include "GameFrameworkServerSubsystem.h"
#include "GameFrameworkPlaceableSubsystem.h"
#include "GameFrameworkWorldSubsystem.h"
#include "GameFrameworkAppSubsystem.h"

DEFINE_LOG_CATEGORY(LogVeGameFramework);

IMPLEMENT_GAME_MODULE(FVeGameFrameworkModule, VeGameFramework);

void FVeGameFrameworkModule::StartupModule() {
	AddSubsystem(FGameFrameworkServerSubsystem::Name, MakeShared<FGameFrameworkServerSubsystem>());
	AddSubsystem(FGameFrameworkPlaceableSubsystem::Name, MakeShared<FGameFrameworkPlaceableSubsystem>());
	AddSubsystem(FGameFrameworkEditorSubsystem::Name, MakeShared<FGameFrameworkEditorSubsystem>());
	AddSubsystem(FGameFrameworkWorldSubsystem::Name, MakeShared<FGameFrameworkWorldSubsystem>());
	AddSubsystem(FGameFrameworkAppSubsystem::Name, MakeShared<FGameFrameworkAppSubsystem>());

	ISubsystemModule::StartupModule();
}

void FVeGameFrameworkModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

TSharedPtr<FGameFrameworkServerSubsystem> FVeGameFrameworkModule::GetServerSubsystem() const {
	return FindSubsystem<FGameFrameworkServerSubsystem>(FGameFrameworkServerSubsystem::Name);
}

TSharedPtr<FGameFrameworkPlaceableSubsystem> FVeGameFrameworkModule::GetPlaceableSubsystem() const {
	return FindSubsystem<FGameFrameworkPlaceableSubsystem>(FGameFrameworkPlaceableSubsystem::Name);
}

TSharedPtr<FGameFrameworkEditorSubsystem> FVeGameFrameworkModule::GetEditorSubsystem() const {
	return FindSubsystem<FGameFrameworkEditorSubsystem>(FGameFrameworkEditorSubsystem::Name);
}

TSharedPtr<FGameFrameworkWorldSubsystem> FVeGameFrameworkModule::GetWorldSubsystem() const {
	return FindSubsystem<FGameFrameworkWorldSubsystem>(FGameFrameworkWorldSubsystem::Name);
}

TSharedPtr<FGameFrameworkAppSubsystem> FVeGameFrameworkModule::GetAppSubsystem() const {
	return FindSubsystem<FGameFrameworkAppSubsystem>(FGameFrameworkAppSubsystem::Name);
}
