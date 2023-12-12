// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once
#include "UIUserStatus.generated.h"

UENUM(BlueprintType)
enum class EUIUserStatus : uint8 {
	Offline,
	Online,
	GameLocal,
	GameOnline,
	Any = 0xFF
};
