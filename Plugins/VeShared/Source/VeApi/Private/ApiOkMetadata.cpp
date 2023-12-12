// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiOkMetadata.h"

bool FApiOkMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bIsOk = IApiMetadata::FromJson(JsonObject);
	bOk = JsonObject->GetBoolField(Api::Fields::Ok);
	return bIsOk;
}
