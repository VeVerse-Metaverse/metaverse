// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "ApiViewMetadata.h"

bool FApiViewMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	Id = GetJsonGuidField(JsonObject, Api::Fields::Id);
	Value = GetJsonIntField(JsonObject, Api::Fields::Value);

	return bOk;
}

TSharedPtr<FJsonObject> FApiViewMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	if (Value != 0) {
		JsonObject->SetNumberField(Api::Fields::Type, Value);
	}
	
	return JsonObject;
}
