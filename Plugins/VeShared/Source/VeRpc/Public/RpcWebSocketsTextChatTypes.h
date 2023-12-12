// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "RpcWebSocketsServiceTypes.h"
#include "VisualLogger/VisualLoggerTypes.h"
// #include "RpcTextChatTypes.generated.h"


namespace RpcTextChatChannelId {
	const auto System = TEXT("b8908111-0715-4c56-b98f-3bfffa03ea39");
	const auto General = TEXT("f16fdbe4-813c-4e9c-a1a1-c7c2cbb154d3");
}

namespace RpcTextChatMethods {
	const auto ChannelSendMethod = TEXT("channelSend");
	const auto ChannelSubscribeMethod = TEXT("channelSubscribe");
	const auto ChannelUnsubscribeMethod = TEXT("channelUnsubscribe");
}

namespace RpcTextChatJsonFields {
	const auto ChannelId = TEXT("channelId");
	const auto Message = TEXT("message");
	const auto Sender = TEXT("sender");
	const auto Category = TEXT("category");
	const auto Key = TEXT("key");
}

namespace RpcTextChatMessageCategory {
	const auto None = TEXT("none");
	const auto System = TEXT("system");
	const auto General = TEXT("general");
	const auto Space = TEXT("space");
	const auto Server = TEXT("server");
	const auto Private = TEXT("private");
}


enum class ERpcWebSocketsTextChatCategory : uint8 {
	None,
	System,
	General,
	Space,
	Server,
	Private,
	AI,
	All
};

//==============================================================================

class VERPC_API FRpcWebSocketsTextChatMessage : public TSharedFromThis<FRpcWebSocketsTextChatMessage> {
public:
	ERpcWebSocketsTextChatCategory Category = ERpcWebSocketsTextChatCategory::All;
	FGuid ChannelId;
	FRpcWebSocketsUser Sender;
	FString Text;

	FLinearColor GetFontColor() const;
	FString ToString() const;

	FRpcWebSocketsTextChatMessage() = default;

	void ParseJson(const TSharedPtr<FJsonObject> JsonObject);
};
