// Authors: Khusan T. Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Api2EntityMetadata.h"
#include "ApiUserMetadata.h"

class VEAPI2_API FApi2SharedAccessMetadata final : public IApiMetadata {
public:

	FGuid EntityId;
	int32 Value;

	FApiUserMetadata Owner = {};
	FApiUserMetadata User = {};
	FGuid UserId = {};
	
	bool Public;
	bool CanView;
	bool CanEdit;
	bool CanDelete;
	
	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	virtual TSharedPtr<FJsonObject> ToJson() override;
};

typedef TApi2BatchMetadata<FApi2SharedAccessMetadata> FApi2SharedAccessBatchMetadata;


