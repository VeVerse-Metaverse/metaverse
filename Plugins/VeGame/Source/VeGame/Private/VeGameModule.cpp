// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeGameModule.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogVeGame);

IMPLEMENT_GAME_MODULE(FVeGameModule, VeGame);

void FVeGameModule::StartupModule() {
	ISubsystemModule::StartupModule();
}

void FVeGameModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

FVeGameModule* FVeGameModule::Get() {
	if (FModuleManager::Get().IsModuleLoaded(TEXT("VeGame"))) {
		return FModuleManager::Get().GetModulePtr<FVeGameModule>(TEXT("VeGame"));
	}
	return FModuleManager::Get().LoadModulePtr<FVeGameModule>(TEXT("VeGame"));
}
