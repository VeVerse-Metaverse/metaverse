// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiEventMetadata.h"
#include "ApiEventMetadataObject.generated.h"

class FApiEventMetadata;

UCLASS()
class VEAPI_API UApiEventMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiEventMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiEventMetadata>> InEntities, UObject* InParent = nullptr);
};
