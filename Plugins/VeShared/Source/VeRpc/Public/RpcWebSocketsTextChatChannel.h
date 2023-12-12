// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "RpcWebSocketsTextChatTypes.h"


class VERPC_API FRpcWebSocketsTextChatChannel : public TSharedFromThis<FRpcWebSocketsTextChatChannel> {
	friend class FRpcWebSocketsTextChatSubsystem;

	class FRpcWebSocketsTextChatChannelEvent : public TMulticastDelegate<void(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel)> {
		friend class FRpcWebSocketsTextChatContainer;
	};

	class FWebSocketsRpcTextChatChannelMessageEvent : public TMulticastDelegate<void(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel, const TSharedPtr<FRpcWebSocketsTextChatMessage>& Message)> {
		friend class FRpcWebSocketsTextChatContainer;
	};

public:
	ERpcWebSocketsTextChatCategory Category = ERpcWebSocketsTextChatCategory::All;
	FGuid Id;
	FString Title;
	FGuid UserId; // For private channel
	bool CanSendMessage = true;

	TArray<TSharedRef<FRpcWebSocketsTextChatMessage>> Messages;
	int32 MaxRecords = 100;

	FRpcWebSocketsTextChatChannel() = default;
	explicit FRpcWebSocketsTextChatChannel(ERpcWebSocketsTextChatCategory InCategory);

	FWebSocketsRpcTextChatChannelMessageEvent OnMessageReceived;
	FRpcWebSocketsTextChatChannelEvent OnUnreadNumChanged;

	int32 GetUnreadNum() const {
		return UnreadNum;
	}

	bool SendMessage(const FString& Message);

	bool AddMessage(const TSharedRef<FRpcWebSocketsTextChatMessage>& Message);

protected:
	void ChangeUnreadNum(int32 NumMessages);
	void ClearUnreadNum();

private:
	int32 UnreadNum = 0;

	//--------------------------------------------------------------------------
#pragma region Subscribing

public:
	bool IsSubscribed() const {
		return bIsSubscribed;
	}

	bool Subscribe();
	bool Unsubscribe();

protected:
	void AddMessageAfterSubscribe(const FString& Message);
	void SendMessagesAfterSubscribe();

private:
	bool bIsSubscribed = false;
	TArray<FString> MessagesAfterSubscribe;

#pragma endregion Subscribing
	//--------------------------------------------------------------------------
};
