// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "ApiSharedAccessMetadata.h"
#include "ApiSharedAccessMetadataObject.generated.h"

/** SharedAccess metadata object. */
UCLASS()
class VEAPI_API UApiSharedAccessMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiSharedAccessMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiSharedAccessMetadata>> Entities, UObject* Parent = nullptr);
};
