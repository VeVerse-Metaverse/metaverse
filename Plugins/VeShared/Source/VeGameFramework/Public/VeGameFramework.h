// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeShared.h"

VEGAMEFRAMEWORK_API DECLARE_LOG_CATEGORY_EXTERN(LogVeGameFramework, All, All);

class VEGAMEFRAMEWORK_API FVeGameFrameworkModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVeGameFrameworkModule* Get() {
		if (FModuleManager::Get().IsModuleLoaded(TEXT("VeGameFramework"))) {
			return FModuleManager::GetModulePtr<FVeGameFrameworkModule>(TEXT("VeGameFramework"));
		}

		if (GIsRunning) {
			return FModuleManager::LoadModulePtr<FVeGameFrameworkModule>(TEXT("VeGameFramework"));
		}

		return nullptr;
	}

	TSharedPtr<class FGameFrameworkServerSubsystem> GetServerSubsystem() const;
	TSharedPtr<class FGameFrameworkPlaceableSubsystem> GetPlaceableSubsystem() const;
	TSharedPtr<class FGameFrameworkEditorSubsystem> GetEditorSubsystem() const;
	TSharedPtr<class FGameFrameworkWorldSubsystem> GetWorldSubsystem() const;
	TSharedPtr<class FGameFrameworkAppSubsystem> GetAppSubsystem() const;
};
