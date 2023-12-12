// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "TextChat/TextChatChannelItemWidget.h"

#include "RpcWebSocketsTextChatSubsystem.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "VeRpc.h"


void UTextChatChannelItemWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(RootWidget)) {
		RootWidget->SetContentColorAndOpacity(ColorDefault);
	}

	if (IsValid(SelectButtonWidget)) {
		SelectButtonWidget->OnClicked.AddDynamic(this, &UTextChatChannelItemWidget::SelectButton_OnClickedCallback);
	}

	if (IsValid(DeselectButtonWidget)) {
		DeselectButtonWidget->OnClicked.AddDynamic(this, &UTextChatChannelItemWidget::DeselectButton_OnClickedCallback);
	}
}

void UTextChatChannelItemWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->OnChannelSelectChanged.AddUObject(this, &UTextChatChannelItemWidget::TextChatSubsystem_OnChannelSelectChangedCallback);
		TextChatSubsystem->OnActiveChannelChanged.AddUObject(this, &UTextChatChannelItemWidget::TextChatSubsystem_OnActiveChannelChangedCallback);
		TextChatSubsystem_OnActiveChannelChangedCallback(TextChatSubsystem->GetActiveChannel());
	}
}

void UTextChatChannelItemWidget::NativeDestruct() {
	Super::NativeDestruct();
	
	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->OnChannelSelectChanged.RemoveAll(this);
		TextChatSubsystem->OnActiveChannelChanged.RemoveAll(this);
	}
}

void UTextChatChannelItemWidget::SetChannel(const TSharedRef<FRpcWebSocketsTextChatChannel>& InChannel) {

	if (Channel) {
		Channel->OnUnreadNumChanged.RemoveAll(this);
	}
	
	Channel = InChannel;

	FString ChannelName;
	switch(Channel->Category) {
	case ERpcWebSocketsTextChatCategory::System:
		ChannelName = TEXT("System");
		break;
	case ERpcWebSocketsTextChatCategory::General:
		ChannelName = TEXT("General");
		break;
	case ERpcWebSocketsTextChatCategory::Space:
		ChannelName = TEXT("Space");
		break;
	case ERpcWebSocketsTextChatCategory::Server:
		ChannelName = TEXT("Server");
		break;
	case ERpcWebSocketsTextChatCategory::Private:
		ChannelName = Channel->Title;
		break;
	default:
		ChannelName = TEXT("All");
	}

	if (IsValid(NameTextWidget)) {
		NameTextWidget->SetText(FText::FromString(ChannelName));
	}

	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		const bool Selected = TextChatSubsystem->IsSelectedChannel(InChannel);
		const bool CanDeselect = TextChatSubsystem->CanDeselectChannel(InChannel);
		DeselectButtonWidget->SetVisibility((Selected && CanDeselect) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	Channel->OnUnreadNumChanged.AddUObject(this, &UTextChatChannelItemWidget::Channel_OnUnreadNumChangedCallback);
	Channel_OnUnreadNumChangedCallback(Channel);
}

void UTextChatChannelItemWidget::SetStatus(ERpcTextChatChannelStatus InStatus) {
	Status = InStatus;
	if (IsValid(RootWidget)) {
		switch (Status) {
		case ERpcTextChatChannelStatus::Active:
			RootWidget->SetContentColorAndOpacity(ColorActive);
			break;
		case ERpcTextChatChannelStatus::Unread:
			RootWidget->SetContentColorAndOpacity(ColorUnread);
			break;
		default:
			RootWidget->SetContentColorAndOpacity(ColorDefault);
		}
	}
}

void UTextChatChannelItemWidget::TextChatSubsystem_OnChannelSelectChangedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& InChannel, bool Selected, bool CanDeselect) {
	if (InChannel == Channel) {
		DeselectButtonWidget->SetVisibility((Selected && CanDeselect) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UTextChatChannelItemWidget::TextChatSubsystem_OnActiveChannelChangedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& InChannel) {
	if (InChannel == Channel) {
		SetStatus(ERpcTextChatChannelStatus::Active);
	} else {
		if (Channel->GetUnreadNum()) {
			SetStatus(ERpcTextChatChannelStatus::Unread);
		} else {
			SetStatus(ERpcTextChatChannelStatus::Default);
		}
	}
}

void UTextChatChannelItemWidget::Channel_OnUnreadNumChangedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& InChannel) {
	if (Status != ERpcTextChatChannelStatus::Active) {
		if (Channel->GetUnreadNum()) {
			SetStatus(ERpcTextChatChannelStatus::Unread);
		} else {
			SetStatus(ERpcTextChatChannelStatus::Default);
		}
	}
}

void UTextChatChannelItemWidget::SelectButton_OnClickedCallback() {
	OnSelectClicked.Broadcast(this);
}

void UTextChatChannelItemWidget::DeselectButton_OnClickedCallback() {
	OnDeselectClicked.Broadcast(this);
}
