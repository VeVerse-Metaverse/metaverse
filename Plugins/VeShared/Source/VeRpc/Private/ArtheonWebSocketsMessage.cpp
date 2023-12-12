// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ArtheonWebSocketsMessage.h"
#include "Misc/Guid.h"
#include "ArtheonWebSocketsPayload.h"
#include "ArtheonWebSocketsTypes.h"

FArtheonWebSocketsMessage::FArtheonWebSocketsMessage() {
    Id = FGuid::NewGuid();
    Type = ArtheonWebSocketsType::None;
    Topic = ArtheonWebSocketsTopic::None;
    Payload = nullptr;
    Args = nullptr;
}

TSharedPtr<FJsonObject> FArtheonWebSocketsMessage::ToJson() {
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Id, Id.ToString());
    JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::Type, Type);
    JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::Topic, Topic);
    JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Method, Method);

    if (Payload != nullptr) {
        JsonObject->SetObjectField(ArtheonWebSocketsJsonFields::Payload, Payload->ToJson());
    }

    if (Args != nullptr) {
        JsonObject->SetObjectField(ArtheonWebSocketsJsonFields::Args, Args->ToJson());
    }

    return JsonObject;
}

void FArtheonWebSocketsMessage::ParseJson(const TSharedPtr<FJsonObject> JsonObject) {
    if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Id)) {
        FGuid::Parse(JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Id), Id);
    }

    if (JsonObject->HasTypedField<EJson::Number>(ArtheonWebSocketsJsonFields::Type)) {
        Type = JsonObject->GetNumberField(ArtheonWebSocketsJsonFields::Type);
    }

    if (JsonObject->HasTypedField<EJson::Number>(ArtheonWebSocketsJsonFields::Topic)) {
        Topic = JsonObject->GetNumberField(ArtheonWebSocketsJsonFields::Topic);
    }

    if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Method)) {
        Method = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Method);
    }

    if (JsonObject->HasTypedField<EJson::Object>(ArtheonWebSocketsJsonFields::Payload)) {
        const auto JsonPayload = JsonObject->GetObjectField(ArtheonWebSocketsJsonFields::Payload);

        if (Payload == nullptr) {
            Payload = MakeShareable(new FArtheonWebSocketsPayload);
        }
        
        Payload->ParseJson(JsonPayload);
    }

    if (JsonObject->HasTypedField<EJson::Object>(ArtheonWebSocketsJsonFields::Args)) {
        const auto JsonArgs = JsonObject->GetObjectField(ArtheonWebSocketsJsonFields::Args);

        if (Args == nullptr) {
            Args = MakeShareable(new FArtheonWebSocketsPayload);
        }

        Args->ParseJson(JsonArgs);
    }
}
