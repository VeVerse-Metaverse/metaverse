// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiEntityMetadata.h"


class VEAPI_API FApiPropertyMetadata final : public IApiMetadata {
public:
	FString Name;
	FString Type;
	FString Value;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	static TArray<FApiPropertyMetadata> ToArray(const TArray<TSharedPtr<FApiPropertyMetadata>>& Entities);
};

class VEAPI_API FApiUpdatePropertyMetadata final : public IApiMetadata {
public:
	FString Name;
	FString Type;
	FString Value;

	FApiUpdatePropertyMetadata() = default;
	explicit FApiUpdatePropertyMetadata(const FApiPropertyMetadata& InMetadata);
	FApiUpdatePropertyMetadata(const FString& InName, const FString& InType, const FString& InValue);

	virtual TSharedPtr<FJsonObject> ToJson() override;
};


typedef TApiBatchMetadata<FApiPropertyMetadata/*, class UApiPortalMetadataObject*/> FApiPropertyBatchMetadata;
