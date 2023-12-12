// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "VePlaceableMetadata.h"
#include "PlaceableAssetMetadata.generated.h"

USTRUCT(BlueprintType)
struct FPlaceableAssetMetadata {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVePlaceableClassMetadata PlaceableClassMetadata;

	/** Id of the placeable, valid for existing placeables */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid Id;

	/** File URL for placeable, not valid for class-based placeables */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Url;

	/** Type of the placeable, @see PlaceableTypes namespace for possible values for file-based, stores class path for class based placeables */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MediaType;

	/** Mime type for the placeable, application/x-ue-class for class-based */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MimeType;
};
