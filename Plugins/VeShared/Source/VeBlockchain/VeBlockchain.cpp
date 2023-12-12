// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeBlockchain.h"

#include "BlockchainNftSubsystem.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogVeBlockchain);

IMPLEMENT_GAME_MODULE(FVeBlockchainModule, VeBlockchain);

void FVeBlockchainModule::StartupModule() {
	AddSubsystem(FBlockchainNftSubsystem::Name, MakeShared<FBlockchainNftSubsystem>());
	ISubsystemModule::StartupModule();
}

void FVeBlockchainModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

TSharedPtr<FBlockchainNftSubsystem> FVeBlockchainModule::GetNftSubsystem() {
	return FindSubsystem<FBlockchainNftSubsystem>(FBlockchainNftSubsystem::Name);
}
