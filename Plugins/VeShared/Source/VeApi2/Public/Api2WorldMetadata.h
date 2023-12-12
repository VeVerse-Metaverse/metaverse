// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2EntityMetadata.h"
#include "ApiSpaceMetadata.h"
#include "VeApi2.h"

enum class EApiEntitySorts : uint8 {
	Id,
	Name,
	Description,
	Map,
	PackageId,
	Type,
	Scheduled,
	Game_Mode	,
	Views,
	CreatedAt,
	UpdatedAt,
	Public,
	Likes,
	Dislikes,
	PakFile,
	PreviewFile,
};

typedef TApi2BatchMetadata<FApiSpaceMetadata> FApi2WorldBatchMetadata;


class VEAPI2_API FApi2BatchSortRequestMetadata : public IApiMetadata {
public:
	FString Search = FString();
	FString Platform = FVeApi2Module::GetPlatform();
	FString Deployment = FVeApi2Module::GetDeployment();
	/** Request offset. */
	int32 Offset = 0;
	/** Request limit. */
	int32 Limit = 40;
	bool Pak = true;
	bool Preview = true;
	bool Likes = true;
	bool Owner = true;

	TArray<EApiEntitySorts> SortOptions;
	FString Options = FString();
	
	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2GetRequestMetadata : public IApiMetadata {
public:
	FString Platform = FVeApi2Module::GetPlatform();
	FString Deployment = FVeApi2Module::GetDeployment();
	bool Pak = true;
	bool Preview = true;
	bool Likes = true;
	bool Owner = true;
	
	virtual TSharedPtr<FJsonObject> ToJson() override;
};