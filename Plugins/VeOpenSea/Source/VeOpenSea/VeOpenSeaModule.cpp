// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeOpenSeaModule.h"

#include "OpenSeaAssetSubsystem.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogVeOpenSea);

IMPLEMENT_GAME_MODULE(FVeOpenSeaModule, VeOpenSea);

void FVeOpenSeaModule::StartupModule() {
	AddSubsystem(FOpenSeaAssetSubsystem::Name, MakeShared<FOpenSeaAssetSubsystem>());

	ISubsystemModule::StartupModule();
}

void FVeOpenSeaModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

TSharedPtr<FOpenSeaAssetSubsystem> FVeOpenSeaModule::GetAssetSubsystem() {
	return FindSubsystem<FOpenSeaAssetSubsystem>(FOpenSeaAssetSubsystem::Name);
}

FVeOpenSeaModule* FVeOpenSeaModule::Get() {
	if (FModuleManager::Get().IsModuleLoaded(TEXT("VeOpenSea"))) {
		return FModuleManager::Get().GetModulePtr<FVeOpenSeaModule>(TEXT("VeOpenSea"));
	}
	return FModuleManager::Get().LoadModulePtr<FVeOpenSeaModule>(TEXT("VeOpenSea"));
}
