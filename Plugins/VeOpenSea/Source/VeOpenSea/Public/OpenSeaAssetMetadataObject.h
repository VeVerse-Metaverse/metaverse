// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "OpenSeaAssetMetadata.h"


#include "OpenSeaAssetMetadataObject.generated.h"

/** Space metadata object. */
UCLASS()
class VEOPENSEA_API UOpenSeaAssetMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	FOpenSeaAssetMetadata Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<FOpenSeaAssetMetadata> Entities, UObject* Parent = nullptr);
};

//typedef TApiBatchMetadata<class FOpenSeaAssetMetadata/*, class UApiObjectMetadataObject*/> FOpenSeaAssetBatchMetadata;
