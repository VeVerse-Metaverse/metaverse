// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeShared.h"
#include "RpcWebSocketsTextChatTypes.h"
#include "ApiUserMetadata.h"
#include "ArtheonWebSocketsPayload.h"

class FRpcWebSocketsTextChatChannel;

class VERPC_API FRpcWebSocketsTextChatSubsystem final : public IModuleSubsystem {
	class FRpcWebSocketsTextChatEvent : public TMulticastDelegate<void()> {
		friend class FRpcWebSocketsTextChatSubsystem;
	};

	class FRpcWebSocketsTextChatMessageEvent : public TMulticastDelegate<void(const TSharedPtr<FRpcWebSocketsTextChatMessage>& Message)> {
		friend class FRpcWebSocketsTextChatSubsystem;
	};

	class FRpcWebSocketsTextChatChannelEvent : public TMulticastDelegate<void(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel)> {
		friend class FRpcWebSocketsTextChatSubsystem;
	};

	class FRpcWebSocketsTextChatChannelSelectedEvent : public TMulticastDelegate<void(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel, bool Selected, bool CanDeselect)> {
		friend class FRpcWebSocketsTextChatSubsystem;
	};

	class FRpcWebSocketsTextChatHideEvent : public TMulticastDelegate<void(float Delay)> {
		friend class FRpcWebSocketsTextChatSubsystem;
	};

public:
	const static FName Name;

	/** Channels container */
	TArray<TSharedRef<FRpcWebSocketsTextChatChannel>> Channels;

	FRpcWebSocketsTextChatSubsystem() = default;
	virtual void Initialize() override;
	virtual void Shutdown() override;

	bool IsEnabled() const {
		return bEnabled;
	}

	FRpcWebSocketsTextChatEvent OnEnabled;
	FRpcWebSocketsTextChatEvent OnDisabled;
	FRpcWebSocketsTextChatEvent OnChannelListChanged;
	FRpcWebSocketsTextChatChannelEvent OnChannelListAdded;
	FRpcWebSocketsTextChatMessageEvent OnMessageReceived;

	bool SendMessage(const FString& Message, TSharedPtr<FRpcWebSocketsTextChatChannel> Channel = nullptr);

	TSharedPtr<FRpcWebSocketsTextChatChannel> FindChannelById(const FGuid& ChannelId);
	TSharedPtr<FRpcWebSocketsTextChatChannel> FindChannelByUserId(const FGuid& UserId);

	void ReceiveAiChatMessage(const FString &Name, const FString &Message);

protected:
	void RpcServiceSubsystem_OnEnabledCallback();
	void RpcServiceSubsystem_OnDisabledCallback();
	void RpcServiceSubsystem_OnMessageReceivedCallback(ERpcWebSocketsMessageTopic Topic, const FString& Method, const TSharedPtr<FJsonObject>& JsonObject);
	void GameState_OnServerIdChanged(const FGuid& InServerId);
	void GameState_OnWorldIdChanged(const FGuid& InWorldId);
	void FriendsSubsystem_OnFriendChangedCallback(const TSharedPtr<FApiUserMetadata>& User);
	void FriendsSubsystem_OnFriendListChangedCallback();

private:
	bool bEnabled = false;
	TWeakPtr<FRpcWebSocketsTextChatChannel> Channel_All;
	TWeakPtr<FRpcWebSocketsTextChatChannel> Channel_System;
	TWeakPtr<FRpcWebSocketsTextChatChannel> Channel_General;
	TWeakPtr<FRpcWebSocketsTextChatChannel> Channel_Server;
	TWeakPtr<FRpcWebSocketsTextChatChannel> Channel_Space;

	//--------------------------------------------------------------------------
#pragma region Active Channel

public:
	FRpcWebSocketsTextChatChannelEvent OnActiveChannelChanged;

	TSharedPtr<FRpcWebSocketsTextChatChannel> GetActiveChannel() const {
		return ActiveChannel.Pin();
	}

	void ActivateChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel);
	void DeactivateChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel);
	TSharedPtr<FRpcWebSocketsTextChatChannel> FindChannelForActivateHelper(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) const;

private:
	TWeakPtr<FRpcWebSocketsTextChatChannel> ActiveChannel;

#pragma endregion Active Channel
	//--------------------------------------------------------------------------
#pragma region Selected Channels

public:
	TArray<TSharedRef<FRpcWebSocketsTextChatChannel>> SelectedChannels;

	FRpcWebSocketsTextChatChannelSelectedEvent OnChannelSelectChanged;
	bool CanDeselectChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) const;
	bool IsSelectedChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) const;
	void SelectChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel);
	void DeselectChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel);

#pragma endregion Selected Channels
	//--------------------------------------------------------------------------
#pragma region Widgets

public:
	void ShowFullMode();
	void HideFullMode(float Delay = 0.f);

	int32 GetUnreadMessagesCount() const {
		return UnreadMessagesCount;
	};

	FRpcWebSocketsTextChatEvent OnShowFullMode;
	FRpcWebSocketsTextChatHideEvent OnHideFullMode;
	FRpcWebSocketsTextChatEvent OnUnreadCountChanged;

protected:
	bool bFullModeEnabled = false;

	void UnreadMessagesIncrement();

private:
	int32 UnreadMessagesCount;

#pragma endregion Widgets
	//--------------------------------------------------------------------------

};
