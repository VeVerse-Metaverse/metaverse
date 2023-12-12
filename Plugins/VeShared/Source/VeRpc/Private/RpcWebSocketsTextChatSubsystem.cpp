// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "RpcWebSocketsTextChatSubsystem.h"

#include "VeRpc.h"
#include "RpcWebSocketsServiceSubsystem.h"
#include "VeShared.h"
#include "VeSocial.h"
#include "SocialFriendsSubsystem.h"
#include "RpcWebSocketsTextChatChannel.h"
#include "VeGameFramework.h"

#define LOCTEXT_NAMESPACE "VeRpc"

#include "VeWorldSettings.h"


const FName FRpcWebSocketsTextChatSubsystem::Name = FName("TextChatSubsystem");

void FRpcWebSocketsTextChatSubsystem::Initialize() {
	// UE_LOG(LogTemp, Log, TEXT(">>> [%p] FRpcWebSocketsTextChatSubsystem::Initialize"), this);

	if (IsRunningDedicatedServer()) {
		return;
	}

	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (WebSocketsRpcServiceSubsystem) {
		WebSocketsRpcServiceSubsystem->OnLoggedIn.AddRaw(this, &FRpcWebSocketsTextChatSubsystem::RpcServiceSubsystem_OnEnabledCallback);
		WebSocketsRpcServiceSubsystem->OnLoggedOut.AddRaw(this, &FRpcWebSocketsTextChatSubsystem::RpcServiceSubsystem_OnDisabledCallback);
		WebSocketsRpcServiceSubsystem->OnPushed.AddRaw(this, &FRpcWebSocketsTextChatSubsystem::RpcServiceSubsystem_OnMessageReceivedCallback);
	}

	FWorldDelegates::OnWorldInitializedActors.AddLambda([=](const UWorld::FActorsInitializedParams& InInitializationValues) {
		auto OnServerId = AVeWorldSettings::FServerIdDelegate::CreateRaw(this, &FRpcWebSocketsTextChatSubsystem::GameState_OnServerIdChanged);
		auto OnWorldId = AVeWorldSettings::FWorldIdDelegate::CreateRaw(this, &FRpcWebSocketsTextChatSubsystem::GameState_OnWorldIdChanged);

		bool Binds = false;
		if (auto* InWorld = InInitializationValues.World) {
			if (auto* VeWorldSettings = Cast<AVeWorldSettings>(InWorld->GetWorldSettings())) {
				VeWorldSettings->OnReady_ServerId(OnServerId);
				VeWorldSettings->OnReady_WorldId(OnWorldId);
				Binds = true;
			}
		}

		if (!Binds) {
			OnServerId.ExecuteIfBound(FGuid());
			OnWorldId.ExecuteIfBound(FGuid());
		}
	});

	GET_MODULE_SUBSYSTEM(FriendsSubsystem, Social, Friends);
	if (FriendsSubsystem) {
		FriendsSubsystem->GetOnFriendChanged().AddRaw(this, &FRpcWebSocketsTextChatSubsystem::FriendsSubsystem_OnFriendChangedCallback);
		FriendsSubsystem->GetOnFriendListChanged().AddRaw(this, &FRpcWebSocketsTextChatSubsystem::FriendsSubsystem_OnFriendListChangedCallback);
	}

	VeLog("[TextChat] Text chat initialized.");
}

void FRpcWebSocketsTextChatSubsystem::Shutdown() {
	if (IsRunningDedicatedServer()) {
		return;
	}

	SelectedChannels.Empty();
	Channels.Empty();

	VeLog("[TextChat] Text chat shutdown.");
}

void FRpcWebSocketsTextChatSubsystem::RpcServiceSubsystem_OnEnabledCallback() {
	bEnabled = true;

	Channels.Empty();

	// All
	auto Channel = Channels.Add_GetRef(MakeShared<FRpcWebSocketsTextChatChannel>(ERpcWebSocketsTextChatCategory::All));
	Channel->CanSendMessage = false;
	Channel_All = Channel;
	SelectChannel(Channel);
	// ActivateChannel(Channel);

	// System (add messages to 'All' channel)
	// Channel = Channels.Add_GetRef(MakeShared<FRpcTextChatChannel>(ERpcWebSocketsTextChatCategory::System));
	// Channel->Id = FGuid(RpcTextChatChannelId::System);
	// Channel->CanSendMessage = false;
	// Channel_System = Channel;

	// General
	Channel = Channels.Add_GetRef(MakeShared<FRpcWebSocketsTextChatChannel>(ERpcWebSocketsTextChatCategory::General));
	Channel_General = Channel;
	Channel->Id = FGuid(RpcTextChatChannelId::General);
	Channel->Subscribe();
	SelectChannel(Channel);
	ActivateChannel(Channel);

	// Space
	Channel = Channels.Add_GetRef(MakeShared<FRpcWebSocketsTextChatChannel>(ERpcWebSocketsTextChatCategory::Space));
	Channel_Space = Channel;
	// Channel->Subscribe();

	// Server
	Channel = Channels.Add_GetRef(MakeShared<FRpcWebSocketsTextChatChannel>(ERpcWebSocketsTextChatCategory::Server));
	Channel_Server = Channel;
	// Channel->Subscribe();

	// Refresh friend channels
	FriendsSubsystem_OnFriendListChangedCallback();

	OnChannelListChanged.Broadcast();

	OnEnabled.Broadcast();
	VeLog("[TextChat] Text chat enabled.");
}

void FRpcWebSocketsTextChatSubsystem::RpcServiceSubsystem_OnDisabledCallback() {
	bEnabled = false;

	ActivateChannel(nullptr);

	SelectedChannels.Empty();

	Channels.Empty();
	OnChannelListChanged.Broadcast();

	OnDisabled.Broadcast();
	VeLog("[TextChat] Text chat disabled.");
}

bool FRpcWebSocketsTextChatSubsystem::SendMessage(const FString& Message, TSharedPtr<FRpcWebSocketsTextChatChannel> Channel) {
	if (!IsEnabled()) {
		return false;
	}

	if (!Channel) {
		if (!ActiveChannel.IsValid()) {
			return false;
		}
		Channel = ActiveChannel.Pin();
	}

	return Channel->SendMessage(Message);
}

TSharedPtr<FRpcWebSocketsTextChatChannel> FRpcWebSocketsTextChatSubsystem::FindChannelById(const FGuid& ChannelId) {
	for (const auto& Channel : Channels) {
		if (Channel->Id == ChannelId) {
			return Channel;
		}
	}
	return nullptr;
}

TSharedPtr<FRpcWebSocketsTextChatChannel> FRpcWebSocketsTextChatSubsystem::FindChannelByUserId(const FGuid& UserId) {
	for (const auto& Channel : Channels) {
		if (Channel->UserId == UserId) {
			return Channel;
		}
	}
	return nullptr;
}

void FRpcWebSocketsTextChatSubsystem::ReceiveAiChatMessage(const FString& InName, const FString& InMessage) {
	// Parse message
	const auto TextChatMessage = MakeShared<FRpcWebSocketsTextChatMessage>();
	TextChatMessage->Sender = FRpcWebSocketsUser();
	TextChatMessage->Sender.Name = InName;
	TextChatMessage->Text = InMessage;
	TextChatMessage->ChannelId = FGuid(RpcTextChatChannelId::System);

	const auto ActiveChannelRef(ActiveChannel.Pin());
	const bool ActiveChannelIsAll = ActiveChannelRef.IsValid() && ActiveChannelRef->Category == ERpcWebSocketsTextChatCategory::All;

	// Add message to channels
	for (const auto& Channel : Channels) {
		if (Channel->AddMessage(TextChatMessage)) {
			if (!ActiveChannelIsAll) {
				if (Channel->Category != ERpcWebSocketsTextChatCategory::All && Channel != ActiveChannel) {
					Channel->ChangeUnreadNum(1);
				}
			}
		}
	}

	UnreadMessagesIncrement();

	OnMessageReceived.Broadcast(TextChatMessage);
}

void FRpcWebSocketsTextChatSubsystem::RpcServiceSubsystem_OnMessageReceivedCallback(ERpcWebSocketsMessageTopic Topic, const FString& Method, const TSharedPtr<FJsonObject>& JsonObject) {
	if (Topic != ERpcWebSocketsMessageTopic::TextChat) {
		return;
	}

	FString Status, StatusMessage;
	if (!FRpcWebSocketsServiceSubsystem::CheckStatus(JsonObject, Status, StatusMessage)) {
		VeLogWarning("[TextChat] Failure message status. Status: {%s}, Message: {%s}.", *Status, *StatusMessage);
		return;
	}

	auto Message = MakeShared<FRpcWebSocketsTextChatMessage>();
	Message->ParseJson(JsonObject);

	// Create private channel if not exists
	if (Message->Category == ERpcWebSocketsTextChatCategory::Private) {
		auto Channel = FindChannelById(Message->ChannelId);
		if (!Channel) {
			Channel = FindChannelByUserId(Message->Sender.Id);
			if (Channel) {
				if (!Channel->Id.IsValid()) {
					Channel->Id = Message->ChannelId;
				}
			} else {
				auto NewChannel = Channels.Add_GetRef(MakeShared<FRpcWebSocketsTextChatChannel>(ERpcWebSocketsTextChatCategory::Private));
				NewChannel->Id = Message->ChannelId;
				NewChannel->Title = Message->Sender.Name;
				NewChannel->UserId = Message->Sender.Id;

				OnChannelListAdded.Broadcast(NewChannel);

				SelectChannel(NewChannel);
			}
		}
	}

	const auto ActiveChannelRef(ActiveChannel.Pin());
	const bool ActiveChannelIsAll = ActiveChannelRef.IsValid() && ActiveChannelRef->Category == ERpcWebSocketsTextChatCategory::All;

	// Add message to channels
	for (const auto& Channel : Channels) {
		if (Channel->AddMessage(Message)) {
			if (!ActiveChannelIsAll) {
				if (Channel->Category != ERpcWebSocketsTextChatCategory::All && Channel != ActiveChannel) {
					Channel->ChangeUnreadNum(1);
				}
			}
		}
	}

	UnreadMessagesIncrement();

	OnMessageReceived.Broadcast(Message);
}

void FRpcWebSocketsTextChatSubsystem::GameState_OnServerIdChanged(const FGuid& InServerId) {
	VeLogVerbose("[TextChat] Server changed {%s}", *InServerId.ToString(EGuidFormats::DigitsWithHyphensLower));

	if (!Channel_Server.IsValid()) {
		return;
	}

	auto Channel = Channel_Server.Pin();

	if (Channel->Id.IsValid()) {
		Channel->Unsubscribe();
	}

	Channel->Id = InServerId;

	if (Channel->Id.IsValid()) {
		Channel->Subscribe();
	}
}

void FRpcWebSocketsTextChatSubsystem::GameState_OnWorldIdChanged(const FGuid& InWorldId) {
	VeLogVerbose("[TextChat] World changed {%s}", *InWorldId.ToString(EGuidFormats::DigitsWithHyphensLower));

	if (!Channel_Space.IsValid()) {
		return;
	}

	auto Channel = Channel_Space.Pin();

	if (Channel->Id.IsValid()) {
		Channel->Unsubscribe();
	}

	Channel->Id = InWorldId;

	if (Channel->Id.IsValid()) {
		Channel->Subscribe();
	}
}

void FRpcWebSocketsTextChatSubsystem::FriendsSubsystem_OnFriendChangedCallback(const TSharedPtr<FApiUserMetadata>& User) {
	if (!bEnabled) {
		return;
	}

}

void FRpcWebSocketsTextChatSubsystem::FriendsSubsystem_OnFriendListChangedCallback() {
	if (!bEnabled) {
		return;
	}

	GET_MODULE_SUBSYSTEM(FriendsSubsystem, Social, Friends);
	if (FriendsSubsystem) {

		// Clear private user channels
		TArray<TSharedRef<FRpcWebSocketsTextChatChannel>> NewChannels;
		for (const auto& Channel : Channels) {
			if (Channel->Category != ERpcWebSocketsTextChatCategory::Private) {
				NewChannels.Add(Channel);
			}
		}
		Channels = NewChannels;

		// Add private user channels
		for (const auto& User : FriendsSubsystem->Users) {
			if (User) {
				auto Channel = Channels.Add_GetRef(MakeShared<FRpcWebSocketsTextChatChannel>(ERpcWebSocketsTextChatCategory::Private));
				Channel->UserId = User->Id;
				Channel->Title = User->Name;
			}
		}

		OnChannelListChanged.Broadcast();
	}
}

//------------------------------------------------------------------------------
#pragma region Active Channel

void FRpcWebSocketsTextChatSubsystem::ActivateChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) {
	if (ActiveChannel == Channel) {
		return;
	}

	ActiveChannel = Channel;
	OnActiveChannelChanged.Broadcast(Channel);

	if (Channel.IsValid() && Channel->Category != ERpcWebSocketsTextChatCategory::All) {
		Channel->ClearUnreadNum();
	}
}

void FRpcWebSocketsTextChatSubsystem::DeactivateChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) {
	if (Channel != ActiveChannel) {
		return;
	}

	auto NewActiveChannel = FindChannelForActivateHelper(Channel);
	ActivateChannel(NewActiveChannel);
}

TSharedPtr<FRpcWebSocketsTextChatChannel> FRpcWebSocketsTextChatSubsystem::FindChannelForActivateHelper(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) const {
	int32 Num = SelectedChannels.Num();
	if (!Num) {
		return nullptr;
	}

	int32 ChannelIndex = INDEX_NONE;
	for (int32 i = 0; i < Num; ++i) {
		if (SelectedChannels[i] == Channel) {
			ChannelIndex = i;
			break;
		}
	}

	if (ChannelIndex == INDEX_NONE) {
		return Channel_All.Pin();
	}

	if (ChannelIndex + 1 < Num) {
		return SelectedChannels[ChannelIndex + 1];
	}

	if (ChannelIndex - 1 >= 0) {
		return SelectedChannels[ChannelIndex - 1];
	}

	return nullptr;
}

#pragma endregion Active Channel
//------------------------------------------------------------------------------
#pragma region Selected Channels

bool FRpcWebSocketsTextChatSubsystem::CanDeselectChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) const {
	return Channel != Channel_All;
}

bool FRpcWebSocketsTextChatSubsystem::IsSelectedChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) const {
	if (Channel) {
		return SelectedChannels.Find(Channel.ToSharedRef()) != INDEX_NONE;
	}
	return false;
}

void FRpcWebSocketsTextChatSubsystem::SelectChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) {
	if (!Channel) {
		return;
	}
	if (IsSelectedChannel(Channel)) {
		return;
	}
	const int32 Index = SelectedChannels.Add(Channel.ToSharedRef());
	if (Index != INDEX_NONE) {
		OnChannelSelectChanged.Broadcast(Channel, true, CanDeselectChannel(Channel));
	}
}

void FRpcWebSocketsTextChatSubsystem::DeselectChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) {
	if (!Channel) {
		return;
	}
	if (!CanDeselectChannel(Channel)) {
		return;
	}

	DeactivateChannel(Channel);
	if (SelectedChannels.RemoveSingle(Channel.ToSharedRef())) {
		OnChannelSelectChanged.Broadcast(Channel, false, true);
	}
}

#pragma endregion Selected Channels
//------------------------------------------------------------------------------
#pragma region Widgets

void FRpcWebSocketsTextChatSubsystem::ShowFullMode() {
	if (!bFullModeEnabled) {
		OnShowFullMode.Broadcast();

		UnreadMessagesCount = 0;
		OnUnreadCountChanged.Broadcast();

		bFullModeEnabled = true;
	}
}

void FRpcWebSocketsTextChatSubsystem::HideFullMode(float Delay) {
	if (bFullModeEnabled) {
		OnHideFullMode.Broadcast(Delay);
		bFullModeEnabled = false;
	}
}

void FRpcWebSocketsTextChatSubsystem::UnreadMessagesIncrement() {
	if (!bFullModeEnabled) {
		++UnreadMessagesCount;
		OnUnreadCountChanged.Broadcast();
	}
}

#pragma endregion Widgets
//------------------------------------------------------------------------------


#undef LOCTEXT_NAMESPACE
