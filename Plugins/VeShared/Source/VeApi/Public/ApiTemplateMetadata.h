// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "ApiEntityMetadata.h"
#include "ApiUserMetadata.h"

class FApiPlaceableMetadata;
class FApiPackageMetadata;

/** Space metadata descriptor. */
class VEAPI_API FApiTemplateMetadata final : public FApiEntityMetadata {
public:
	FGuid TemplateId = FGuid();
	FString Name = FString();
	FString Title = FString();
	FString Summary = FString();
	FString Description = FString();
	FString Version = FString();
	
	/** Owner and developer of the metaverse, user who uploaded the metaverse. */
	TSharedPtr<FApiUserMetadata> Owner;
	
	/** List of files of the metaverse, can include previews, pak files, etc. */
	TArray<FApiFileMetadata> Files;
	
	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};

/** Base interface for batched metadata requests with string query. */
class VEAPI_API FApiTemplateBatchQueryRequestMetadata : public IApiBatchQueryRequestMetadata {
public:
	
	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

#pragma region Create / Update

/** Used to create and update space requests. */
class VEAPI_API FApiUpdateTemplateMetadata final : public IApiMetadata {
public:
	FGuid TemplateId = FGuid();
	FString Name = FString();
	FString Title = FString();
	FString Summary = FString();
	FString Description = FString();
	FString Version = FString();
	bool bPublic = true;

	FApiUpdateTemplateMetadata() = default;
	explicit FApiUpdateTemplateMetadata(const FApiTemplateMetadata& InMetadata);

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

#pragma endregion

typedef TApiBatchMetadata<class FApiTemplateMetadata/*, class UApiMetaverseMetadataObject*/> FApiTemplateBatchMetadata;