// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "RpcWebSocketsServiceTypes.generated.h"

namespace RpcWebSocketsServiceMethods {
	const auto ConnectMethod = TEXT("connect");
	const auto UserChangeNameMethod = TEXT("userChangeName");
}

namespace RpcWebSocketsServiceJsonFields {
	const auto Id = TEXT("id");
	const auto Name = TEXT("name");
	const auto AvatarUrl = TEXT("avatarUrl");
	const auto Type = TEXT("type");
	const auto Topic = TEXT("topic");
	const auto Method = TEXT("method");
	const auto Args = TEXT("args");
	const auto Payload = TEXT("payload");
	const auto Key = TEXT("key");
	const auto Sender = TEXT("sender");
	const auto Status = TEXT("status");
	const auto Message = TEXT("message");
}

namespace RpcWebSocketsServiceStatus {
	const auto Ok = TEXT("ok");
	const auto Error = TEXT("error");
}

enum class ERpcWebSocketsMessageType : int32 {
	None = 0,
	Push = 1,
	Request = 2,
	Response = 4
};

enum class ERpcWebSocketsMessageTopic : int32 {
	None = 0,
	System = 1,
	TextChat = 2,
	Analytics = 4,
	Vivox = 8
};

//==============================================================================

USTRUCT(BlueprintType)
struct VERPC_API FRpcWebSocketsUser {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AvatarUrl;

	TSharedRef<FJsonObject> ToJson() const;
	void ParseJson(const TSharedPtr<FJsonObject> JsonObject);

	bool IsValid() const;
};

struct VERPC_API FRpcWebSocketsMessage {
	FGuid Id;
	ERpcWebSocketsMessageType Type;
	ERpcWebSocketsMessageTopic Topic;
	FString Method;

	TSharedPtr<FJsonObject> Body;

	FRpcWebSocketsMessage();
	FRpcWebSocketsMessage(ERpcWebSocketsMessageType InType, ERpcWebSocketsMessageTopic InTopic, const FString& InMethod, TSharedPtr<FJsonObject> InBody);

	TSharedRef<FJsonObject> ToJson() const;
	void ParseJson(const TSharedPtr<FJsonObject> JsonObject);
};
