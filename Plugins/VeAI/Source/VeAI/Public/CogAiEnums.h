// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CogAiEnums.generated.h"

UENUM(BlueprintType)
enum class ECogAiMessageRole : uint8 {
	None UMETA(Hidden),
	System UMETA(DisplayName = "System"),
	Npc UMETA(DisplayName = "NPC"),
};

/**
 * @brief System message type.
 */
UENUM(BlueprintType)
enum class ECogAiSystemMessageType : uint8 {
	None UMETA(Hidden),
	WhoAmI UMETA(DisplayName = "Who Am I"),
	Perception UMETA(DisplayName = "Perception"),
	Actions UMETA(DisplayName = "Actions"),
	Context UMETA(DisplayName = "Context"),
	Inspect UMETA(DisplayName = "Inspect"),
};
