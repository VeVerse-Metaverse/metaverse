// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiObjectMetadata.h"
#include "ApiObjectMetadataObject.generated.h"

/** Space metadata object. */
UCLASS()
class VEAPI_API UApiObjectMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiObjectMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiObjectMetadata>> Entities, UObject* Parent = nullptr);
};

typedef TApiBatchMetadata<class FApiObjectMetadata/*, class UApiObjectMetadataObject*/> FApiObjectBatchMetadata;
