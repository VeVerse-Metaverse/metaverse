// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiSpaceMetadata.h"
#include "ApiSpaceMetadataObject.generated.h"

/** Space metadata object. */
UCLASS()
class VEAPI_API UApiSpaceMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiSpaceMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiSpaceMetadata>> Entities, UObject* Parent = nullptr);
};
