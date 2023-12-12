// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "ApiPlaceableMetadata.h"
#include "Api2PlaceableMetadataObject.generated.h"

/** Space metadata object. */
UCLASS()
class VEAPI2_API UApi2PlaceableMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiPlaceableMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiPlaceableMetadata>> Entities, UObject* Parent = nullptr);
};

typedef TApiBatchMetadata<class FApiPlaceableMetadata> FApi2PlaceableBatchMetadata;

UCLASS()
class VEAPI2_API UApi2PlaceableClassMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiPlaceableClassMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiPlaceableClassMetadata>> Entities, UObject* Parent = nullptr);
};
typedef TApiBatchMetadata<FApiPlaceableClassMetadata> FApi2PlaceableClassBatchMetadata;
