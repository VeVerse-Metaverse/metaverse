// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2CogAiRequest.h"

#include "VeShared.h"
#include "VeApi2.h"

FString FApi2CogAiMessage::GetKey() const {
	static constexpr auto ActionKey = TEXT("action");
	static constexpr auto SystemKey = TEXT("system");

	// Determine the type of the message (it should be either "action" or "system")
	FString Key;
	if (Content->HasField(ActionKey)) {
		Key = ActionKey;
	} else if (Content->HasField(SystemKey)) {
		Key = SystemKey;
	} else {
		VeLogErrorFunc("invalid message type");
		return {};
	}
	return {};
}

TSharedPtr<FJsonObject> FApi2CogAiMessage::ToJson() const {
	TSharedPtr<FJsonObject> Json = IApiMetadata::ToJson();

	Json->SetStringField(TEXT("from"), From);
	Json->SetObjectField(TEXT("content"), Content);

	return Json;
}

TSharedPtr<FJsonObject> FApi2CogAiRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> Json = IApiMetadata::ToJson();

	TArray<TSharedPtr<FJsonValue>> MessagesJson;
	for (auto& Message : Messages) {
		MessagesJson.Add(MakeShareable(new FJsonValueObject(Message.ToJson())));
	}

	Json->SetArrayField(TEXT("data"), MessagesJson);

	return Json;
}

TSharedPtr<FJsonObject> FApi2CogAiUserRequestMetadata::ToJson() {
	auto Json = FApi2CogAiRequestMetadata::ToJson();
	Json->SetStringField(TEXT("key"), Key);
	Json->SetStringField(TEXT("version"), Version);
	return Json;
}

bool FApi2CogAiResponseMessage::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	static constexpr auto ActionKey = TEXT("action");
	static constexpr auto SystemKey = TEXT("system");

	// This is the response, so the message is from the NPC.
	From = TEXT("npc");

	// Copy the content of the message to the content field
	Content = CopyTemp(JsonObject);

	// Determine the type of the message (it should be either "action" or "system")
	FString Key;
	if (JsonObject->HasField(ActionKey)) {
		Key = ActionKey;
	} else if (JsonObject->HasField(SystemKey)) {
		Key = SystemKey;
	} else {
		VeLogErrorFunc("invalid message type");
		return false;
	}

	// Get the map of parameters as a JSON object.
	const auto Params = JsonObject->GetObjectField(Key);

	// Enumerate all keys of the system object
	for (const auto& Param : Params->Values) {
		// Add the key and value to the parameters array
		Parameters.Add(Param.Key, Param.Value->AsString());
	}

	return true;
}

bool FApi2CogAiResponseMetadata::FromJson(const TArray<TSharedPtr<FJsonValue>>& JsonArray) {
	/*
	{"data": [
		{
			"action": {
				"emotion": "neutral",
				"message": "This is a white cube. It appears to be fixed on the floor and can't be moved.",
				"target": "self",
				"thoughts": "I wonder what it's doing here.",
				"type": "say"
			}
		}
	]}
	*/

	for (const auto& DataItem : JsonArray) {
		if (DataItem->Type != EJson::Object) {
			VeLogErrorFunc("invalid data item type, expected object");
		}

		if (FApi2CogAiResponseMessage Message; Message.FromJson(DataItem->AsObject())) {
			Messages.Add(Message);
		} else {
			VeLogErrorFunc("failed to parse message");
		}
	}

	return true;
}

bool FApi2CogAiRequest::Process() {
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(Api::GetApi2RootUrl() / TEXT("ai/cog"));

	const auto Json = RequestMetadata.ToJson();

	TArray<uint8> Content;
	if (!SerializeJsonPayload(Json, Content)) {
		return false;
	}
	HttpRequest->SetContent(Content);

	return FApi2Request::Process();
}

bool FApi2CogAiRequest::OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) {
	LogWarningF("FApi2CogAiRequest::OnComplete: %s", *InResponse->GetContentAsString());

	if (CheckResponse()) {
		TArray<TSharedPtr<FJsonValue>> JsonValues;
		if (GetResponseJsonArrayPayload(InResponse, JsonValues)) {
			ResponseMetadata.FromJson(JsonValues);
			return true;
		}
	}

	return false;
}

bool FApi2CogAiUserRequest::Process() {
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(Api::GetApi2RootUrl() / TEXT("ai/cog/user"));

	const auto Json = RequestMetadata.ToJson();

	TArray<uint8> Content;
	if (!SerializeJsonPayload(Json, Content)) {
		return false;
	}
	HttpRequest->SetContent(Content);

	return FApi2Request::Process();
}

bool FApi2CogAiUserRequest::OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) {
	LogWarningF("FApi2CogAiUserRequest::OnComplete: %s", *InResponse->GetContentAsString());

	if (CheckResponse()) {
		TArray<TSharedPtr<FJsonValue>> JsonValues;
		if (GetResponseJsonArrayPayload(InResponse, JsonValues)) {
			ResponseMetadata.FromJson(JsonValues);
			return true;
		}
	}

	return false;
}
