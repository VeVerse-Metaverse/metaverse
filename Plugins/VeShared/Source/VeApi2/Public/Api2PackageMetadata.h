// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiEntityMetadata.h"
#include "Api2EntityMetadata.h"


class VEAPI2_API FApi2MapMetadata final : public FApiEntityMetadata {
public:
	FString Path;

	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
	virtual bool FromJson(const TSharedPtr<FJsonValue> InJsonPayload) override;
};


typedef TArray<TSharedRef<FApi2MapMetadata>> FApi2MapArrayMetadata;
