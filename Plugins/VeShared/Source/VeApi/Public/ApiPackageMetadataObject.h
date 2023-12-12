// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiPackageMetadata.h"
#include "ApiPackageMetadataObject.generated.h"

class FApiPackageMetadata;

UCLASS()
class VEAPI_API UApiPackageMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiPackageMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiPackageMetadata>> InEntities, UObject* InParent = nullptr);
};
