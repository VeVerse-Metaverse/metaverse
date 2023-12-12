// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2PackageMetadata.h"


bool FApi2MapMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	bool bOk = FApiEntityMetadata::FromJson(InJsonObject);

	Path = GetJsonStringField(InJsonObject, Api::Fields::Map);

	return bOk;
}

bool FApi2MapMetadata::FromJson(const TSharedPtr<FJsonValue> InJsonPayload) {
	Path =	InJsonPayload->AsString();
	return true;
}
