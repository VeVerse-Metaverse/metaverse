// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "PlaceableFileMetadata.generated.h"

USTRUCT()
struct FPlaceableFileMetadata {
	GENERATED_BODY()

	UPROPERTY()
	FGuid PlaceableId;

	UPROPERTY()
	FString FilePath;

	UPROPERTY()
	FString FileType;

	/** Type of the placeable, @see PlaceableTypes namespace for possible values for file-based, stores class path for class based placeables */
	UPROPERTY()
	FString MediaType;
};
