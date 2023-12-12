// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiPlaceableMetadata.h"
#include "ApiPlaceableMetadataObject.generated.h"

/** Space metadata object. */
UCLASS()
class VEAPI_API UApiPlaceableMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiPlaceableMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiPlaceableMetadata>> Entities, UObject* Parent = nullptr);
};

typedef TApiBatchMetadata<class FApiPlaceableMetadata> FApiPlaceableBatchMetadata;

UCLASS()
class VEAPI_API UApiPlaceableClassMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiPlaceableClassMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiPlaceableClassMetadata>> Entities, UObject* Parent = nullptr);
};
typedef TApiBatchMetadata<FApiPlaceableClassMetadata> FApiPlaceableClassBatchMetadata;
