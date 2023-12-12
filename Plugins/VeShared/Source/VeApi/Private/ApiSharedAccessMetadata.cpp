// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiSharedAccessMetadata.h"

bool FApiSharedAccessMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	EntityId = GetJsonGuidField(JsonObject, Api::Fields::Id);
	UserId = GetJsonGuidField(JsonObject, Api::Fields::UserId);
	Public = GetJsonBoolField(JsonObject, Api::Fields::Public);
	CanView = GetJsonBoolField(JsonObject, Api::Fields::CanView);
	CanEdit = GetJsonBoolField(JsonObject, Api::Fields::CanEdit);
	CanDelete = GetJsonBoolField(JsonObject, Api::Fields::CanDelete);

	if (const TSharedPtr<FJsonObject> OwnerJsonObject = GetJsonObjectField(JsonObject, Api::Fields::Owner)) {
		Owner.FromJson(OwnerJsonObject);
	}
	
	return bOk;
}

TSharedPtr<FJsonObject> FApiSharedAccessMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();

	JsonObject->SetBoolField(Api::Fields::Public, Public);
	JsonObject->SetBoolField(Api::Fields::CanView, CanView);
	JsonObject->SetBoolField(Api::Fields::CanEdit, CanEdit);
	JsonObject->SetBoolField(Api::Fields::CanDelete, CanDelete);
	JsonObject->SetStringField(Api::Fields::UserId, UserId.ToString(EGuidFormats::DigitsWithHyphensLower));

	return JsonObject;
}

