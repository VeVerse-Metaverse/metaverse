// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once


#include "ApiUserMetadata.h"
#include "UObject/Object.h"
#include "ApiUserMetadataObject.generated.h"

/** User metadata object. */
UCLASS()
class VEAPI_API UApiUserMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiUserMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiUserMetadata>> Entities, UObject* Parent = nullptr);
};
