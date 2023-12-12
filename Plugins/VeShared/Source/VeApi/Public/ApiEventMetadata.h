// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"
#include "ApiFileMetadata.h"
#include "ApiLinkMetadata.h"
#include "ApiSpaceMetadata.h"
#include "ApiUserMetadata.h"

class FApiSpaceMetadata;
class UApiEventMetadataObject;

enum class EApiEventPlatforms : uint8 {
	Unknown,
	Windows,
	Mac,
	Linux,
	SteamVR,
	OculusVR,
	OculusQuest,
	IOS,
	Android
};

class VEAPI_API FApiEventMetadata final : public FApiEntityMetadata {
public:	
	FString Name = TEXT("");
	FString Title = TEXT("");
	FString Summary = TEXT("");
	FString Description = TEXT("");
	bool Active = false;
	FDateTime StartsAt = FDateTime();
	FDateTime EndsAt = FDateTime();
	FString Type = TEXT("");
	FApiUserMetadata Owner;
	TArray<FApiFileMetadata> Files;
	FGuid SpaceId;
	FApiSpaceMetadata Space;

	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};

/** Used to create and update mod requests. */
class VEAPI_API FApiUpdateEventMetadata final : public IApiMetadata {
public:
	FGuid Id = FGuid();
	FString Name = TEXT("");
	FString Title = TEXT("");
	FString Summary = TEXT("");
	FString Description = TEXT("");
	FDateTime StartsAt = {};
	FDateTime EndsAt = {};
	bool bPublic = true;
	bool bActive = true;

	bool operator==(const FApiUpdateEventMetadata& rhsMetadata) const;
	bool operator!=(const FApiUpdateEventMetadata& rhsMetadata) const;

	FApiUpdateEventMetadata() = default;
	explicit FApiUpdateEventMetadata(const FApiEventMetadata& InMetadata);
	
	virtual TSharedPtr<FJsonObject> ToJson() override;
};

typedef TApiBatchMetadata<class FApiEventMetadata/*, class UApiEventMetadataObject*/> FApiEventBatchMetadata;
