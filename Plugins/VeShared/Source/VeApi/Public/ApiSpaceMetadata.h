// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiEntityMetadata.h"
#include "ApiPackageMetadata.h"
#include "ApiUserMetadata.h"

class FApiPlaceableMetadata;
class FApiPackageMetadata;

/** Space metadata descriptor. */
class VEAPI_API FApiSpaceMetadata final : public FApiEntityMetadata {
public:
	FString Name = FString();
	FString Description = FString();
	FString Map = FString();
	FString GameMode = FString();
	FApiUserMetadata Owner = {};
	TSharedPtr<FApiPackageMetadata> Package = MakeShared<FApiPackageMetadata>();
	/** List of files of the mod, can include previews, pak files, etc. */
	TArray<FApiFileMetadata> Files;
	bool bPublic = true;

	int32 TotalLikes;
	int32 TotalDislikes;
	int32 Likes;
	int32 Dislikes;
	int32 Liked;


	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};

typedef TApiBatchMetadata<FApiSpaceMetadata> FApiSpaceBatchMetadata;

/** Base interface for batched metadata requests with string query. */
class VEAPI_API FApiSpaceBatchQueryRequestMetadata : public IApiBatchQueryRequestMetadata {
public:
	/** Space type. */
	FString Type = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

#pragma region Create / Update

/** Used to create and update space requests. */
class VEAPI_API FApiUpdateSpaceMetadata final : public IApiMetadata {
public:
	FGuid Id = FGuid();
	FString Name = FString();
	FString Description = FString();
	FString Map = FString();
	FString GameMode = FString();
	FGuid PackageId = FGuid();
	bool bPublic = true;

	FApiUpdateSpaceMetadata() = default;
	explicit FApiUpdateSpaceMetadata(const FApiSpaceMetadata& InMetadata);

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

#pragma endregion
