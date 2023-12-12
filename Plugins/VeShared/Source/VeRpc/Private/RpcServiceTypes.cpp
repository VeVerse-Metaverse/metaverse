// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "RpcWebSocketsServiceTypes.h"


TSharedRef<FJsonObject> FRpcWebSocketsUser::ToJson() const {
	auto JsonObject = MakeShared<FJsonObject>();

	if (Id.IsValid()) {
		JsonObject->SetStringField(RpcWebSocketsServiceJsonFields::Id, Id.ToString());
	}

	if (!Name.IsEmpty()) {
		JsonObject->SetStringField(RpcWebSocketsServiceJsonFields::Name, Name);
	}

	if (!AvatarUrl.IsEmpty()) {
		JsonObject->SetStringField(RpcWebSocketsServiceJsonFields::AvatarUrl, AvatarUrl);
	}

	return JsonObject;
}

void FRpcWebSocketsUser::ParseJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (!JsonObject) {
		return;
	}
	
	if (JsonObject->HasTypedField<EJson::String>(RpcWebSocketsServiceJsonFields::Id)) {
		Id = FGuid(JsonObject->GetStringField(RpcWebSocketsServiceJsonFields::Id));
	}

	if (JsonObject->HasTypedField<EJson::String>(RpcWebSocketsServiceJsonFields::Name)) {
		Name = JsonObject->GetStringField(RpcWebSocketsServiceJsonFields::Name);
	}

	if (JsonObject->HasTypedField<EJson::String>(RpcWebSocketsServiceJsonFields::AvatarUrl)) {
		AvatarUrl = JsonObject->GetStringField(RpcWebSocketsServiceJsonFields::AvatarUrl);
	}
}

bool FRpcWebSocketsUser::IsValid() const {
	return Id.IsValid() && !Name.IsEmpty();
}

//==============================================================================

FRpcWebSocketsMessage::FRpcWebSocketsMessage()
    : Id(FGuid::NewGuid()),
      Type(ERpcWebSocketsMessageType::None),
      Topic(ERpcWebSocketsMessageTopic::None),
      Method(TEXT("")),
      Body(nullptr) {
}

FRpcWebSocketsMessage::FRpcWebSocketsMessage(ERpcWebSocketsMessageType InType, ERpcWebSocketsMessageTopic InTopic, const FString& InMethod, TSharedPtr<FJsonObject> InBody)
    : Id(FGuid::NewGuid()),
      Type(InType),
      Topic(InTopic),
      Method(InMethod),
      Body(InBody) {
}

TSharedRef<FJsonObject> FRpcWebSocketsMessage::ToJson() const {
    auto JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField(RpcWebSocketsServiceJsonFields::Id, Id.ToString(EGuidFormats::DigitsWithHyphens));
    JsonObject->SetNumberField(RpcWebSocketsServiceJsonFields::Type, static_cast<int32>(Type));
    JsonObject->SetNumberField(RpcWebSocketsServiceJsonFields::Topic, static_cast<int32>(Topic));
    JsonObject->SetStringField(RpcWebSocketsServiceJsonFields::Method, Method);
    JsonObject->SetObjectField(RpcWebSocketsServiceJsonFields::Args, Body);
    return JsonObject;
}

void FRpcWebSocketsMessage::ParseJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (!JsonObject) {
		return;
	}

	if (JsonObject->HasTypedField<EJson::String>(RpcWebSocketsServiceJsonFields::Id)) {
        FGuid::Parse(JsonObject->GetStringField(RpcWebSocketsServiceJsonFields::Id), Id);
    }

    if (JsonObject->HasTypedField<EJson::Number>(RpcWebSocketsServiceJsonFields::Type)) {
        Type = static_cast<ERpcWebSocketsMessageType>(JsonObject->GetNumberField(RpcWebSocketsServiceJsonFields::Type));
    }

    if (JsonObject->HasTypedField<EJson::Number>(RpcWebSocketsServiceJsonFields::Topic)) {
        Topic = static_cast<ERpcWebSocketsMessageTopic>(JsonObject->GetNumberField(RpcWebSocketsServiceJsonFields::Topic));
    }

    if (JsonObject->HasTypedField<EJson::String>(RpcWebSocketsServiceJsonFields::Method)) {
        Method = JsonObject->GetStringField(RpcWebSocketsServiceJsonFields::Method);
    }

    if (JsonObject->HasTypedField<EJson::Object>(RpcWebSocketsServiceJsonFields::Payload)) {
        Body = JsonObject->GetObjectField(RpcWebSocketsServiceJsonFields::Payload);
    }
}
