// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiEntityMetadata.h"


/** Analytics metadata descriptor. */
class VEAPI_API FApiAnalyticsMetadata : public FApiEntityMetadata {
public:
	FGuid AppId = FGuid();
	FGuid ContextEntityId = FGuid();
	FString ContextEntityType = FString();
	FGuid UserId = FGuid();

	FString Platform = FString();
	FString Deployment = FString();
	FString Configuration = FString();
	FString Event = FString();
	FString Payload = FString();


	//virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
	virtual TSharedPtr<FJsonObject> ToJson() override;
};

typedef TApiBatchMetadata<FApiAnalyticsMetadata> FApiAnalyticsBatchMetadata;

#pragma endregion
