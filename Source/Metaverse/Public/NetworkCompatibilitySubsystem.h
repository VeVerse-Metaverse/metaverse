// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "NetworkCompatibilitySubsystem.generated.h"

/**
 * 
 */
UCLASS()
class METAVERSE_API UVeNetworkCompatibilitySubsystem : public UGameInstanceSubsystem {
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

};
