// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"

/** Ok status metadata descriptor. */
class VEAPI_API FApiOkMetadata final : public IApiMetadata {
public:
	bool bOk = false;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};
