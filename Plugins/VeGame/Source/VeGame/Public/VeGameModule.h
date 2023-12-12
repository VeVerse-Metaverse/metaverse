// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
#include "VeShared.h"

VEGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogVeGame, All, All);

class FVeGameModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVeGameModule* Get();
};
