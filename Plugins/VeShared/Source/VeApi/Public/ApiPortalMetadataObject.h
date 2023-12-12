// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiPortalMetadata.h"
#include "ApiPortalMetadataObject.generated.h"

class FApiPackageMetadata;
UCLASS()
class VEAPI_API UApiPortalMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	FApiPortalMetadata Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<FApiPortalMetadata> InEntities, UObject* InParent = nullptr);

	static UObject* ToUObject(FApiPortalMetadata InEntity, UObject* InParent = nullptr);
};
