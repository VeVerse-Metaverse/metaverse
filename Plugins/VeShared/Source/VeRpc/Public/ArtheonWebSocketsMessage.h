// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ArtheonWebSocketsPayload.h"

/** Generic WebSockets message */
struct VERPC_API FArtheonWebSocketsMessage : FArtheonJsonInterface {
    FArtheonWebSocketsMessage();

    FGuid Id;
    int32 Type;
    int32 Topic;
    FString Method;

    /** Valid for RPC responses and push messages. */
    TSharedPtr<FArtheonWebSocketsPayload> Payload;

    /** Valid for RPC requests. */
    TSharedPtr<FArtheonWebSocketsPayload> Args;

    virtual TSharedPtr<FJsonObject> ToJson() override;
    virtual void ParseJson(const TSharedPtr<FJsonObject> JsonObject) override;
};
