// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2AiSimpleFsmRequest.h"

#include "VeShared.h"
#include "VeApi2.h"

static const auto KeySubject = TEXT("s");
static const auto KeyAction = TEXT("a");
static const auto KeyContext = TEXT("c");
static const auto KeyTarget = TEXT("t");
static const auto KeyMetadata = TEXT("m");

TSharedPtr<FJsonObject> FApi2AiSimpleFsmEntity::ToJson() {
	const auto Jo = MakeShared<FJsonObject>();

	Jo->SetStringField(Api::Fields::AiFsmName, Name);

	Jo->SetStringField(Api::Fields::AiFsmDescription, Description);

	return Jo;
}

bool FApi2AiSimpleFsmEntity::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	return IApiMetadata::FromJson(JsonObject);
}

FString FApi2AiSimpleFsmEntity::ToUrlParams() {
	return IApiMetadata::ToUrlParams();
}

TSharedPtr<FJsonObject> FApi2AiSimpleFsmLocationLink::ToJson() {
	const auto Jo = FApi2AiSimpleFsmEntity::ToJson();

	Jo->SetStringField(Api::Fields::AiFsmTarget, Target);

	Jo->SetStringField(Api::Fields::AiFsmObject, Object);

	return Jo;
}

TSharedPtr<FJsonObject> FApi2AiSimpleFsmEntities::ToJson() {
	const auto Jo = MakeShared<FJsonObject>();

	TArray<TSharedPtr<FJsonValue>> JsonAiNpcArray;
	for (auto& Npc : NPCs) {
		const auto JsonAiNpc = MakeShareable(new FJsonValueObject(Npc.ToJson()));
		JsonAiNpcArray.Add(JsonAiNpc);
	}
	Jo->SetArrayField(Api::Fields::AiFsmNpcs, JsonAiNpcArray);

	TArray<TSharedPtr<FJsonValue>> JsonAiPlayerArray;
	for (auto& Player : Players) {
		const auto JsonAiPlayer = MakeShareable(new FJsonValueObject(Player.ToJson()));
		JsonAiPlayerArray.Add(JsonAiPlayer);
	}
	Jo->SetArrayField(Api::Fields::AiFsmPlayers, JsonAiPlayerArray);

	TArray<TSharedPtr<FJsonValue>> JsonAiObjectArray;
	for (auto& Object : Objects) {
		const auto JsonAiObject = MakeShareable(new FJsonValueObject(Object.ToJson()));
		JsonAiObjectArray.Add(JsonAiObject);
	}
	Jo->SetArrayField(Api::Fields::AiFsmObjects, JsonAiObjectArray);

	return Jo;
}

TSharedPtr<FJsonObject> FApi2AiSimpleFsmLocation::ToJson() {
	const auto Jo = FApi2AiSimpleFsmEntity::ToJson();

	TArray<TSharedPtr<FJsonValue>> JsonAiLinkArray;
	for (auto& Link : Links) {
		const auto JsonAiLink = MakeShareable(new FJsonValueObject(Link.ToJson()));
		JsonAiLinkArray.Add(JsonAiLink);
	}
	Jo->SetArrayField(Api::Fields::AiFsmLocationLinks, JsonAiLinkArray);

	Jo->SetObjectField(Api::Fields::AiFsmLocationEntities, Entities.ToJson());

	return Jo;
}

FApi2AiSimpleFsmRequestMetadata::FApi2AiSimpleFsmRequestMetadata() {}

TSharedPtr<FJsonObject> FApi2AiSimpleFsmRequestMetadata::ToJson() {
	const auto Jo = MakeShared<FJsonObject>();

	Jo->SetNumberField(Api::Fields::AiFsmVersion, 2);

	Jo->SetNumberField(Api::Fields::AiFsmStateNum, StateNum);

	Jo->SetStringField(Api::Fields::AiFsmContext, Context);

	TArray<TSharedPtr<FJsonValue>> JsonAiLocationArray;
	for (auto& Location : Locations) {
		const auto JsonAiLocation = MakeShareable(new FJsonValueObject(Location.ToJson()));
		JsonAiLocationArray.Add(JsonAiLocation);
	}

	Jo->SetArrayField(Api::Fields::AiFsmLocations, JsonAiLocationArray);

	return Jo;
}

FApi2AiSimpleFsmStateMetadata::FApi2AiSimpleFsmStateMetadata() {}

bool FApi2AiSimpleFsmStateMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (!JsonObject) {
		return false;
	}

	NPC = GetJsonStringField(JsonObject, Api::Fields::AiFsmNPC, "");
	Action = GetJsonStringField(JsonObject, Api::Fields::AiFsmAction, "");
	Context = GetJsonStringField(JsonObject, Api::Fields::AiFsmObject, "");
	Target = GetJsonStringField(JsonObject, Api::Fields::AiFsmTarget, "");
	Metadata = GetJsonStringField(JsonObject, Api::Fields::AiFsmMetadata, "");
	Emotion = GetJsonStringField(JsonObject, Api::Fields::AiFsmEmotion, "");

	return true;
}

void FApi2AiSimpleFsmStateMetadata::FromPlayScriptString(const FString& InString) {
	TArray<FString> Tokens;
	InString.ParseIntoArray(Tokens, TEXT("|"), true);

	for (const FString& Token : Tokens) {
		TArray<FString> Pair;
		Token.ParseIntoArray(Pair, TEXT(":"), true);

		if (Pair.Num() == 2) {
			if (Pair[0] == KeySubject) {
				NPC = Pair[1];
			} else if (Pair[0] == KeyAction) {
				Action = Pair[1];
			} else if (Pair[0] == KeyContext) {
				Context = Pair[1];
			} else if (Pair[0] == KeyTarget) {
				Target = Pair[1];
			} else if (Pair[0] == KeyMetadata) {
				Metadata = Pair[1];
				// Remove quotes if present (used to wrap phrases with spaces)
				if (Metadata.StartsWith("\"")) {
					Metadata = Metadata.Mid(1);
				}
				if (Metadata.EndsWith("\"")) {
					Metadata = Metadata.LeftChop(1);
				}
			}
		}
	}
}

FString FApi2AiSimpleFsmStateMetadata::ToString() const {
	return FString::Printf(TEXT("%s|%s|%s|%s|%s"), *NPC, *Action, *Context, *Target, *Metadata);
}

FApi2AiSimpleFsmMetadata::FApi2AiSimpleFsmMetadata() {}

bool FApi2AiSimpleFsmMetadata::FromString(const FString& InString) {
	TArray<FString> Tokens;
	InString.ParseIntoArray(Tokens, TEXT("\n"), true);

	for (const FString& Token : Tokens) {
		FApi2AiSimpleFsmStateMetadata State;
		State.FromPlayScriptString(Token);
		States.Add(State);
	}

	return true;
}

bool FApi2AiSimpleFsmMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (!JsonObject) {
		return false;
	}

	// Extract the scene states from the JSON payload
	const auto JoStates = GetJsonArrayField(JsonObject, Api::Fields::Payload);

	for (const auto& JvState : JoStates) {
		FApi2AiSimpleFsmStateMetadata StateMetadata;
		StateMetadata.FromJson(JvState->AsObject());
		States.Add(StateMetadata);
	}

	return true;
}

bool FApi2AiSimpleFsmRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(TEXT("POST"));
#if 1
	HttpRequest->SetURL(FString::Printf(TEXT("%s/ai/simple-fsm/states"), *Api::GetApi2RootUrl()));
#else
	HttpRequest->SetURL(FString::Printf(TEXT("%s/ai/simple-fsm/states-static"), *Api::GetApi2RootUrl()));
#endif
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2AiSimpleFsmRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		const auto ResponseContent = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		const auto JsonReaderRef = TJsonReaderFactory<>::Create(ResponseContent);
		if (!FJsonSerializer::Deserialize(JsonReaderRef, JsonObject) || !JsonObject) {
			VeLogError("failed to deserialize json: %s", *ResponseContent);
			return false;
		}
		if (ResponseMetadata.FromJson(JsonObject)) {
			return true;
		}
	}
	return false;
}
