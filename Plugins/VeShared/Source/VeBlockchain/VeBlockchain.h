// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeShared.h"

class FBlockchainNftSubsystem;

VEBLOCKCHAIN_API DECLARE_LOG_CATEGORY_EXTERN(LogVeBlockchain, All, All);

class VEBLOCKCHAIN_API FVeBlockchainModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVeBlockchainModule* Get() {
		if (FModuleManager::Get().IsModuleLoaded(TEXT("VeBlockchain"))) {
			return FModuleManager::GetModulePtr<FVeBlockchainModule>(TEXT("VeBlockchain"));
		}

		if (GIsRunning) {
			return FModuleManager::LoadModulePtr<FVeBlockchainModule>(TEXT("VeBlockchain"));
		}

		return nullptr;
	}

	TSharedPtr<FBlockchainNftSubsystem> GetNftSubsystem();
};
