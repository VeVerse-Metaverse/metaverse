// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

// #include "ApiEntityMetadata.h"
// #include "ApiFileMetadata.h"
#include "ApiSpaceMetadata.h"

class UApiPortalMetadataObject;

class VEAPI_API FApiPortalDestinationMetadata final : public FApiEntityMetadata {
public:
	/** Name of the portal. */
	FString Name;

	/** Destination space to find a matching server. */
	FApiSpaceMetadata Space;

	/** List of files of the portal. */
	TArray<FApiFileMetadata> Files;

	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};

class VEAPI_API FApiPortalMetadata final : public FApiEntityMetadata {
public:
	/** Name of the portal. */
	FString Name;
	FApiPortalDestinationMetadata Destination;

	FApiSpaceMetadata Space;

	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};

class VEAPI_API FApiUpdatePortalMetadata final : public IApiMetadata {
public:
	FGuid Id = FGuid();
	FGuid WorldId = FGuid();
	FString Name = TEXT("");
	FGuid DestinationId = FGuid();
	bool bPublic = true;

	FApiUpdatePortalMetadata() = default;
	FApiUpdatePortalMetadata(const FApiPortalMetadata& InMetadata);

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

typedef TApiBatchMetadata<FApiPortalMetadata/*, class UApiPortalMetadataObject*/> FApiPortalBatchMetadata;
