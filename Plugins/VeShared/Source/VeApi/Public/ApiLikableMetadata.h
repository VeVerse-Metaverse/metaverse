// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"
#include "ApiUserMetadata.h"

class VEAPI_API FApiLikableMetadata final : public IApiMetadata {
public:

	FApiUserMetadata User;
	int32 Value;
	
	
	
	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;

};

class VEAPI_API FApiLikableBatchMetadata final  : public TApiBatchMetadata<> {
public:
	bool Liked;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};