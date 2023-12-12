// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiPersonaMetadata.h"
#include "ApiPersonaMetadataObject.generated.h"

/** Space metadata object. */
UCLASS()
class VEAPI_API UApiPersonaMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiPersonaMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiPersonaMetadata>> Entities, UObject* Parent = nullptr);
};

typedef TApiBatchMetadata<class FApiPersonaMetadata/*, class UApiObjectMetadataObject*/> FApiPersonaBatchMetadata;
