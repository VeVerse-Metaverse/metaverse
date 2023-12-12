// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiRatingMetadata.h"

bool FApiRatingMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	Id = GetJsonGuidField(JsonObject, Api::Fields::Id);
	Value = GetJsonIntField(JsonObject, Api::Fields::Value);

	return bOk;
}

TSharedPtr<FJsonObject> FApiRatingMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();

	JsonObject->SetNumberField(Api::Fields::Value, Value);

	return JsonObject;
}
