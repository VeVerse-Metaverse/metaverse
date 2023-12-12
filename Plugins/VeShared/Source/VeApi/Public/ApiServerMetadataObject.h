// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiEntityMetadata.h"
#include "ApiServerMetadata.h"
#include "ApiServerMetadataObject.generated.h"

class FApiServerMetadata;
UCLASS()
class VEAPI_API UApiServerMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiServerMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiServerMetadata>> InEntities, UObject* InParent = nullptr);
};
