// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

UENUM(BlueprintType)
enum class EGizmoPlacement : uint8 {
	None UMETA(DisplayName = "None"),
	OnFirstSelection UMETA(DisplayName = "On First Selection"),
	OnLastSelection UMETA(DisplayName = "On Last Selection"),
};
