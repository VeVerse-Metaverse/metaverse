// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
#include "VeShared.h"

/**
 * 
 */
class VEGAME_API FVeGameClientSubsystem final : IModuleSubsystem {
public:
	FVeGameClientSubsystem();
	virtual ~FVeGameClientSubsystem() override;

	virtual void Initialize() override;
	virtual void Shutdown() override;
};
