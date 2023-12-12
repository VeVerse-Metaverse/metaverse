// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
#include "OpenSeaAssetSubsystem.h"
#include "VeShared.h"

VEOPENSEA_API DECLARE_LOG_CATEGORY_EXTERN(LogVeOpenSea, All, All);

class VEOPENSEA_API FVeOpenSeaModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FOpenSeaAssetSubsystem> GetAssetSubsystem();;

	static FVeOpenSeaModule* Get();
};
