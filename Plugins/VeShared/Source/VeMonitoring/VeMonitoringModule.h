// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "VeMonitoring/Public/MonitoringMonitoringSubsystem.h"
#include "Modules/ModuleManager.h"
#include "VeShared.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVeMonitoring, All, All);

class FVeMonitoringModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVeMonitoringModule* Get() {
		if (FModuleManager::Get().IsModuleLoaded(TEXT("VeMonitoring"))) {
			return FModuleManager::Get().GetModulePtr<FVeMonitoringModule>(TEXT("VeMonitoring"));
		}
		return FModuleManager::Get().LoadModulePtr<FVeMonitoringModule>(TEXT("VeMonitoring"));
	}

	TSharedPtr<FMonitoringMonitoringSubsystem> GetMonitoringSubsystem() {
		return FindSubsystem<FMonitoringMonitoringSubsystem>(FMonitoringMonitoringSubsystem::Name);
	}
};
