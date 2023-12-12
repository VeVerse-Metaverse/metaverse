// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiAnalyticsMetadata.h"

#include "ApiCommon.h"
#include "GenericPlatform/GenericPlatformHttp.h"

typedef TSharedPtr<FJsonValue> FJsonValuePtr;
typedef TSharedPtr<FJsonObject> FJsonObjectPtr;
typedef TArray<TSharedPtr<FJsonValue>> FJsonValuePtrArray;
//
// bool FApiAnalyticsMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
// 	const bool bOk = FApiEntityMetadata::FromJson(InJsonObject);
// 	AppId = GetJsonGuidField(InJsonObject, Api::AnalyticsType::AppId);
// 	ContextEntityId = GetJsonGuidField(InJsonObject, Api::AnalyticsType::ContextEntityId);
// 	UserId = GetJsonGuidField(InJsonObject, Api::Fields::UserId);
// 	ContextEntityType = GetJsonStringField(InJsonObject, Api::AnalyticsType::ContextEntityType);
// 	Platform = GetJsonStringField(InJsonObject, Api::Fields::Platform);
// 	Deployment = GetJsonStringField(InJsonObject, Api::Fields::Deployment);
//
// 	Configuration = GetJsonStringField(InJsonObject, Api::Fields::Configuration);
// 	Event = GetJsonStringField(InJsonObject, Api::AnalyticsType::Event);
//
// 	return bOk;
// }

TSharedPtr<FJsonObject> FApiAnalyticsMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::AnalyticsType::AppId, AppId.ToString(EGuidFormats::DigitsWithHyphensLower));
	JsonObject->SetStringField(Api::AnalyticsType::ContextEntityId, ContextEntityId.ToString(EGuidFormats::DigitsWithHyphensLower));
	JsonObject->SetStringField(Api::AnalyticsType::ContextEntityType, ContextEntityType);
	JsonObject->SetStringField(Api::Fields::UserId, UserId.ToString(EGuidFormats::DigitsWithHyphensLower));
	JsonObject->SetStringField(Api::Fields::Platform, Platform);
	JsonObject->SetStringField(Api::Fields::Deployment, Deployment);
	JsonObject->SetStringField(Api::Fields::Configuration, Configuration);
	JsonObject->SetStringField(Api::AnalyticsType::Event, Event);
	

	const auto JsonReader = TJsonReaderFactory<>::Create(Payload);
	TSharedPtr<FJsonObject> JsonPayloadObject;
	if (FJsonSerializer::Deserialize(JsonReader, JsonPayloadObject) && JsonPayloadObject.IsValid()) {
		JsonObject->SetObjectField(Api::Fields::Payload, JsonPayloadObject);
	}
	return JsonObject;
}
