// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

UENUM(BlueprintType)
enum class ETransformType : uint8 {
	None UMETA(DisplayName = "None"),
	Translation UMETA(DisplayName = "Translation"),
	Rotation UMETA(DisplayName = "Rotation"),
	Scale UMETA(DisplayName = "Scale")
};

UENUM(BlueprintType)
enum class ETransformSpace : uint8 {
	World UMETA(DisplayName = "World Space"),
	Local UMETA(DisplayName = "Local Space")
};

UENUM(BlueprintType)
enum class ETransformationDomain : uint8 {
	None UMETA(DisplayName = "None"),

	AxisX UMETA(DisplayName = "X Axis"),
	AxisY UMETA(DisplayName = "Y Axis"),
	AxisZ UMETA(DisplayName = "Z Axis"),

	PlaneXY UMETA(DisplayName = "XY Plane"),
	PlaneYZ UMETA(DisplayName = "YZ Plane"),
	PlaneXZ UMETA(DisplayName = "XZ Plane"),

	XYZ UMETA(DisplayName = "XYZ"),
};
