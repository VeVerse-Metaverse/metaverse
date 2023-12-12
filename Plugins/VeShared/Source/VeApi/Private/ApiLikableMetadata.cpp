// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiLikableMetadata.h"

bool FApiLikableMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);
	
	const TSharedPtr<FJsonObject> UserJsonObject = GetJsonObjectField(JsonObject, Api::Fields::User);
	if (UserJsonObject.IsValid()) {
		User.FromJson(UserJsonObject);
	}

	Value = GetJsonIntField(JsonObject,Api::Fields::Value);
	
	return bOk;
}

bool FApiLikableBatchMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	Liked = GetJsonBoolField(JsonObject,Api::Fields::Liked);
	
	return bOk;
}