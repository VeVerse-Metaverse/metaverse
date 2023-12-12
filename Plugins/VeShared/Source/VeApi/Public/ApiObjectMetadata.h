// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"
#include "ApiFileMetadata.h"
#include "ApiUserMetadata.h"

/** Object metadata descriptor. */
class VEAPI_API FApiObjectMetadata final : public FApiEntityMetadata {
public:
	FString Type = FString();
	FString Name = FString();
	FString Artist = FString();
	FString Date = FString();
	FString Description = FString();
	FString Medium = FString();
	FString Source = FString();
	FString SourceUrl = FString();
	FString License = FString();
	FString Copyright = FString();
	FString Credit = FString();
	FString Origin = FString();
	FString Location = FString();
	FString Dimensions = TEXT("");

	float Width = 0.0f;
	float Height = 0.0f;
	float ScaleMultiplier = 1.0f;

	FApiUserMetadata Owner;
	TArray<FApiFileMetadata> Files;
	bool bPublic = true;

	int32 TotalLikes;
	int32 TotalDislikes;
	int32 Liked;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

#pragma region Create / Update

/** Used to create and update object requests. */
class VEAPI_API FApiUpdateObjectMetadata final : public IApiMetadata {
public:
	FGuid Id = FGuid();
	FString Type = FString();
	FString Name = FString();
	FString Artist = FString();
	FString Date = FString();
	FString Description = FString();
	FString Medium = FString();
	FString Source = FString();
	FString SourceUrl = FString();
	FString License = FString();
	FString Copyright = FString();
	FString Credit = FString();
	FString Origin = FString();
	FString Location = FString();
	FString Dimensions = TEXT("");

	float Width = 0.0f;
	float Height = 0.f;
	float ScaleMultiplier = 1.0f;

	bool bPublic = true;

	FApiUpdateObjectMetadata() = default;
	explicit FApiUpdateObjectMetadata(const FApiObjectMetadata& InMetadata);

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

#pragma endregion

typedef TApiBatchMetadata<class FApiObjectMetadata/*, class UApiObjectMetadataObject*/> FApiObjectBatchMetadata;
