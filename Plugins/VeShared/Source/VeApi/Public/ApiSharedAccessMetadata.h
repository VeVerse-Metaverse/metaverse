// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "ApiEntityMetadata.h"
#include "ApiUserMetadata.h"

class VEAPI_API FApiSharedAccessMetadata final : public IApiMetadata {
public:

	FGuid EntityId;
	int32 Value;

	FApiUserMetadata Owner = {};
	FGuid UserId = {};
	
	bool Public;
	bool CanView;
	bool CanEdit;
	bool CanDelete;
	
	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	virtual TSharedPtr<FJsonObject> ToJson() override;
};

typedef TApiBatchMetadata<FApiSharedAccessMetadata> FApiSharedAccessBatchMetadata;


