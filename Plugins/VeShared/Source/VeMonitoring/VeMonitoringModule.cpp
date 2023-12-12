// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "VeMonitoringModule.h"

DEFINE_LOG_CATEGORY(LogVeMonitoring);

IMPLEMENT_GAME_MODULE(FVeMonitoringModule, VeMonitoring);

void FVeMonitoringModule::StartupModule() {
	AddSubsystem(FMonitoringMonitoringSubsystem::Name, MakeShared<FMonitoringMonitoringSubsystem>());

	ISubsystemModule::StartupModule();
}

void FVeMonitoringModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}
