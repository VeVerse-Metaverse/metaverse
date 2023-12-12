// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"
#include "ApiEntityMetadataObject.generated.h"

/** Entity metadata object. */
UCLASS(BlueprintType)
class VEAPI_API UApiEntityMetadata final : public UObject {
	GENERATED_BODY()

public:
	FApiEntityMetadata Metadata;
};
