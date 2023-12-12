// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "ApiEntityMetadata.h"

class VEAPI_API FApiRatingMetadata final : public IApiMetadata {
public:

	FGuid Id;
	int32 Value;
	
	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	virtual TSharedPtr<FJsonObject> ToJson() override;
};
