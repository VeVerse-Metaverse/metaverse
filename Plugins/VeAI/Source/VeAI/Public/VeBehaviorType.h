// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "VeBehaviorType.generated.h"

UENUM(BlueprintType)
enum class EVeBehaviorType : uint8 {
	Idle,
	Move,
	Speak,
	Take,
	Use
};
