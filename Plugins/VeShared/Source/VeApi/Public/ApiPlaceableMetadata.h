// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"
#include "ApiFileMetadata.h"
#include "ApiPropertyMetadata.h"
#include "ApiUserMetadata.h"

class VEAPI_API FApiPlaceableClassMetadata final : public IApiMetadata {
public:
	FGuid Id;
	FString Name;
	FString Description;
	FString Class;
	FString Category;

	TArray<FApiFileMetadata> Files;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI_API FApiPlaceableMetadata final : public IApiMetadata {
public:
	/** Unique id. */
	FGuid Id;
	FGuid SlotId;
	FVector Position;
	FRotator Rotation;
	FVector Scale;
	FGuid EntityId;
	FGuid PlaceableClassId;
	FString Type;

	FApiPlaceableClassMetadata PlaceableClassMetadata;
	FApiUserMetadata Owner;
	TArray<FApiFileMetadata> Files;
	TArray<FApiPropertyMetadata> Properties;

	FTransform GetTransform() const;
	
	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	virtual TSharedPtr<FJsonObject> ToJson() override;
};

/** Base interface for batched metadata requests with string query. */
class VEAPI_API FApiPlaceableClassBatchQueryRequestMetadata : public IApiBatchQueryRequestMetadata {
public:
	FString Category = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

/** Used with patch api request UpdatePlaceableTransform */
class VEAPI_API FApiUpdatePlaceableTransformMetadata final : public IApiMetadata {
public:
	FGuid Id;
	FVector Position;
	FRotator Rotation;
	FVector Scale;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

typedef TApiBatchMetadata<class FApiPlaceableMetadata> FApiPlaceableBatchMetadata;
typedef TApiBatchMetadata<FApiPlaceableClassMetadata> FApiPlaceableClassBatchMetadata;
