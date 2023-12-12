// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "RpcWebSocketsTextChatChannel.h"

#include "VeShared.h"
#include "VeRpc.h"
#include "RpcWebSocketsServiceSubsystem.h"

FRpcWebSocketsTextChatChannel::FRpcWebSocketsTextChatChannel(ERpcWebSocketsTextChatCategory InCategory)
	: Category(InCategory) {}

bool FRpcWebSocketsTextChatChannel::SendMessage(const FString& Message) {
	if (!CanSendMessage) {
		return false;
	}

	FGuid ChannelId;
	if (Id.IsValid()) {
		ChannelId = Id;
	} else if (Category == ERpcWebSocketsTextChatCategory::Private && UserId.IsValid()) {
		AddMessageAfterSubscribe(Message);
		Subscribe();
		return false;
	} else {
		return false;
	}

	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return false;
	}

	const auto BodyJsonObject = MakeShared<FJsonObject>();

	// ChannelId
	BodyJsonObject->SetStringField(RpcTextChatJsonFields::ChannelId, ChannelId.ToString(EGuidFormats::DigitsWithHyphens));

	// Message
	BodyJsonObject->SetStringField(RpcTextChatJsonFields::Message, Message);

	// ApiKey
	const FString ApiKey = WebSocketsRpcServiceSubsystem->GetApiKey();
	BodyJsonObject->SetStringField(RpcTextChatJsonFields::Key, ApiKey);

	const FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::TextChat,
		RpcTextChatMethods::ChannelSendMethod,
		BodyJsonObject
		);

	const auto Callback = MakeShared<FRpcWebSocketsMessageCallback>();
	Callback->BindLambda([](const FString& Method, const TSharedPtr<FJsonObject>& CbBodyJsonObject) {
		if (FString Status, StatusMessage; !FRpcWebSocketsServiceSubsystem::CheckStatus(CbBodyJsonObject, Status, StatusMessage)) {
			VeLogWarning("[TextChat] Error send message, status: {%s}, message: {%s}.", *Status, *StatusMessage);
		}
	});

	if (WebSocketsRpcServiceSubsystem->SendMessage(RpcMessage, Callback)) {
		return true;
	}

	VeLogError("[TextChat] Failed to sent the message.");
	return false;
}

bool FRpcWebSocketsTextChatChannel::AddMessage(const TSharedRef<FRpcWebSocketsTextChatMessage>& Message) {
	if (Category != ERpcWebSocketsTextChatCategory::All) {
		if (Message->Category != Category) {
			return false;
		}
		// if (Message->ChannelId != Id) {
		// 	return false;
		// }

		if (Category == ERpcWebSocketsTextChatCategory::Private) {
			if (Id.IsValid()) {
				if (Message->ChannelId != Id) {
					return false;
				}
			} else if (UserId.IsValid()) {
				if (Message->Sender.Id != UserId) {
					return false;
				}
				Id = Message->ChannelId;
			}
		}
	}

	// Limit records
	if (MaxRecords > 0) {
		if (const int32 Overflow = Messages.Num() - MaxRecords + 1; Overflow > 0) {
			Messages.RemoveAt(0, Overflow, false);
		}
	}

	Messages.Add(Message);

	OnMessageReceived.Broadcast(AsShared(), Message);

	return true;
}

void FRpcWebSocketsTextChatChannel::ChangeUnreadNum(int32 NumMessages) {
	if (NumMessages) {
		UnreadNum += NumMessages;
		OnUnreadNumChanged.Broadcast(AsShared());
	}
}

void FRpcWebSocketsTextChatChannel::ClearUnreadNum() {
	if (UnreadNum) {
		UnreadNum = 0;
		OnUnreadNumChanged.Broadcast(AsShared());
	}
}

//------------------------------------------------------------------------------
#pragma region Subscribing

bool FRpcWebSocketsTextChatChannel::Subscribe() {
	FGuid ChannelId;
	if (Id.IsValid()) {
		ChannelId = Id;
	} else if (Category == ERpcWebSocketsTextChatCategory::Private) {
		if (UserId.IsValid()) {
			ChannelId = UserId;
		} else {
			VeLogWarning("[TextChat] Can not subscribe private channel.");
			return false;
		}
	} else {
		VeLogWarning("[TextChat] Can not subscribe channel.");
		return false;
	}

	VeLogVerbose("[TextChat] Subscribe channel {%s}", *ChannelId.ToString(EGuidFormats::DigitsWithHyphens));

	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return false;
	}

	const auto BodyJsonObject = MakeShared<FJsonObject>();

	// ChannelId
	BodyJsonObject->SetStringField(RpcTextChatJsonFields::ChannelId, ChannelId.ToString(EGuidFormats::DigitsWithHyphens));

	// ApiKey
	const FString ApiKey = WebSocketsRpcServiceSubsystem->GetApiKey();
	BodyJsonObject->SetStringField(RpcTextChatJsonFields::Key, ApiKey);

	const FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::TextChat,
		RpcTextChatMethods::ChannelSubscribeMethod,
		BodyJsonObject
		);

	const auto Callback = MakeShared<FRpcWebSocketsMessageCallback>();
	Callback->BindLambda([this](const FString& Method, const TSharedPtr<FJsonObject>& CbBodyJsonObject) {
		FString Status, StatusMessage;
		bIsSubscribed = FRpcWebSocketsServiceSubsystem::CheckStatus(CbBodyJsonObject, Status, StatusMessage);
		if (!bIsSubscribed) {
			VeLogWarning("[TextChat] Error channel subscribe, status: {%s}, message: {%s}.", *Status, *StatusMessage);
		} else {
			if (Category == ERpcWebSocketsTextChatCategory::Private) {
				const FGuid ChannelId = FGuid(CbBodyJsonObject->GetStringField(RpcTextChatJsonFields::ChannelId));
				if (ChannelId.IsValid()) {
					Id = ChannelId;
				} else {
					VeLogWarning("[TextChat] Error channel id");
				}
			}
			SendMessagesAfterSubscribe();
		}
	});

	if (WebSocketsRpcServiceSubsystem->SendMessage(RpcMessage, Callback)) {
		return true;
	}

	VeLogError("[TextChat] Failed to subscribe to the channel.");
	return false;
}

bool FRpcWebSocketsTextChatChannel::Unsubscribe() {
	if (!Id.IsValid()) {
		VeLogWarning("[TextChat] Can not unsubscribe channel: ID is not valid.");
		return false;
	}

	VeLogVerbose("[TextChat] Unsubscribe channel {%s}", *Id.ToString(EGuidFormats::DigitsWithHyphens));

	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return false;
	}

	const auto BodyJsonObject = MakeShared<FJsonObject>();

	// ChannelId
	BodyJsonObject->SetStringField(RpcTextChatJsonFields::ChannelId, Id.ToString(EGuidFormats::DigitsWithHyphens));

	// ApiKey
	const FString ApiKey = WebSocketsRpcServiceSubsystem->GetApiKey();
	BodyJsonObject->SetStringField(RpcTextChatJsonFields::Key, ApiKey);

	const FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::TextChat,
		RpcTextChatMethods::ChannelUnsubscribeMethod,
		BodyJsonObject
		);

	const auto Callback = MakeShared<FRpcWebSocketsMessageCallback>();
	Callback->BindLambda([this](const FString& Method, const TSharedPtr<FJsonObject>& CbBodyJsonObject) {
		if (FString Status, StatusMessage; !FRpcWebSocketsServiceSubsystem::CheckStatus(CbBodyJsonObject, Status, StatusMessage)) {
			VeLogWarning("[TextChat] Error channel unsubscribe, status: {%s}, message: {%s}.", *Status, *StatusMessage);
		} else {
			bIsSubscribed = false;
		}
	});

	if (WebSocketsRpcServiceSubsystem->SendMessage(RpcMessage, Callback)) {
		return true;
	}

	VeLogError("[TextChat] Failed to unsubscribe to the channel.");
	return false;
}

void FRpcWebSocketsTextChatChannel::AddMessageAfterSubscribe(const FString& Message) {
	MessagesAfterSubscribe.Add(Message);
}

void FRpcWebSocketsTextChatChannel::SendMessagesAfterSubscribe() {
	for (const auto& Message : MessagesAfterSubscribe) {
		SendMessage(Message);
	}
	MessagesAfterSubscribe.Empty();
}

#pragma endregion Subscribing
//------------------------------------------------------------------------------
