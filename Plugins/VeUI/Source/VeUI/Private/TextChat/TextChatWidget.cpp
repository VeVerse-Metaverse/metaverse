// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "TextChat/TextChatWidget.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "TextChat/TextChatInputWidget.h"
#include "TextChat/TextChatMessagesWidget.h"
#include "TextChat/TextChatChannelListWidget.h"
#include "ArtheonWebSocketsService.h"
#include "ArtheonWebSocketsTypes.h"
#include "VePlayerControllerBase.h"
#include "VeApi.h"
#include "VeRpc.h"
#include "VeUI.h"

#include "RpcWebSocketsServiceSubsystem.h"
#include "RpcWebSocketsTextChatSubsystem.h"
#include "VePlayerStateBase.h"
#include "Components/EditableTextBox.h"
#include "TextChat/TextChatChannelItemWidget.h"


UTextChatWidget::UTextChatWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	bIsFocusable = true;
}

void UTextChatWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(DeselectedButtonWidget)) {
		DeselectedButtonWidget->OnClicked.AddDynamic(this, &UTextChatWidget::DeselectedButton_OnClickedCallback);
	}

	if (IsValid(InputWidget)) {
		InputWidget->GetOnMessageSent().AddUObject(this, &UTextChatWidget::InputWidget_OnMessageSentCallback);
	}

	if (IsValid(SelectedChannelsWidget)) {
		SelectedChannelsWidget->GetOnChannelSelectClicked().AddUObject(this, &UTextChatWidget::SelectedChannelsWidget_OnChannelSelectClickedCallback);
		SelectedChannelsWidget->GetOnChannelDeselectClicked().AddUObject(this, &UTextChatWidget::SelectedChannelsWidget_OnChannelDeselectClickedCallback);
	}

	if (IsValid(DeselectedChannelsWidget)) {
		DeselectedChannelsWidget->GetOnChannelSelectClicked().AddUObject(this, &UTextChatWidget::DeselectedChannelsWidget_OnChannelSelectClickedCallback);
	}

	DeselectedPanelSetVisibility(false);
}

void UTextChatWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->OnEnabled.AddUObject(this, &UTextChatWidget::TextChatSubsystem_OnEnabledCallback);
		TextChatSubsystem->OnDisabled.AddUObject(this, &UTextChatWidget::TextChatSubsystem_OnDisabledCallback);
		TextChatSubsystem->OnActiveChannelChanged.AddUObject(this, &UTextChatWidget::TextChatSubsystem_OnActiveChannelChangedCallback);
		if (TextChatSubsystem->IsEnabled()) {
			TextChatSubsystem_OnEnabledCallback();
		}
		TextChatSubsystem_OnActiveChannelChangedCallback(TextChatSubsystem->GetActiveChannel());
	}
}

void UTextChatWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->OnEnabled.RemoveAll(this);
		TextChatSubsystem->OnDisabled.RemoveAll(this);
		TextChatSubsystem->OnActiveChannelChanged.RemoveAll(this);
	}
}

UWidget* UTextChatWidget::GetWidgetToFocus() const {
	if (IsValid(InputWidget)) {
		return InputWidget->GetWidgetToFocus();
	}
	return nullptr;
}

void UTextChatWidget::Reset() {
	if (IsValid(MessagesWidget)) {
		MessagesWidget->SetAutoScroll(true);
	}
}

void UTextChatWidget::TextChatSubsystem_OnEnabledCallback() {}

void UTextChatWidget::TextChatSubsystem_OnDisabledCallback() { }

void UTextChatWidget::DeselectedButton_OnClickedCallback() {
	DeselectedPanelSetVisibility(!DeselectedPanelGetVisibility());
}

void UTextChatWidget::InputWidget_OnMessageSentCallback(const FText& Message) const {
	if (Message.IsEmpty()) {
		OnClose.Broadcast();
	} else {
		if (Message.ToString().StartsWith("/")) {
			if (auto* OwningPC = GetOwningPlayer(); IsValid(OwningPC)) {
				if (auto* OwningPCBase = Cast<AVePlayerControllerBase>(OwningPC); IsValid(OwningPCBase)) {
					OwningPCBase->ProcessChatCommand(Message.ToString());

					if (IsValid(MessagesWidget)) {
						// Try to fill sender info.
						FRpcWebSocketsUser Sender;
						if (const auto* PlayerState = OwningPCBase->GetPlayerState<AVePlayerStateBase>(); IsValid(PlayerState)) {
							Sender.Id = PlayerState->GetUserMetadata().Id;
							Sender.Name = PlayerState->GetUserMetadata().Name;
						} else {
							Sender.Name = TEXT("me");
						}

						// Fill the message metadata.
						const auto ChatMessage = MakeShared<FRpcWebSocketsTextChatMessage>();
						if (Message.ToString().StartsWith("/ai")) {
							ChatMessage->Category = ERpcWebSocketsTextChatCategory::AI;
						} else {
							ChatMessage->Category = ERpcWebSocketsTextChatCategory::System;
						}
						ChatMessage->Sender = Sender;
						ChatMessage->ChannelId = FGuid(RpcTextChatChannelId::System);
						MessagesWidget->AddMessage(ChatMessage);
					}
				}
			}
		} else {
			GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
			if (TextChatSubsystem) {
				TextChatSubsystem->SendMessage(Message.ToString());
			}
		}
	}
}

void UTextChatWidget::TextChatSubsystem_OnActiveChannelChangedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) {
	if (IsValid(MessagesWidget)) {
		MessagesWidget->SetChannel(Channel);
	}
	if (IsValid(InputWidget) && Channel) {
		InputWidget->SetIsEnabled(Channel->CanSendMessage);
	}
}

void UTextChatWidget::SelectedChannelsWidget_OnChannelSelectClickedCallback(UTextChatChannelItemWidget* ChannelItemWidget) {
	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->ActivateChannel(ChannelItemWidget->GetChannel());
	}
}

void UTextChatWidget::SelectedChannelsWidget_OnChannelDeselectClickedCallback(UTextChatChannelItemWidget* ChannelItemWidget) {
	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->DeselectChannel(ChannelItemWidget->GetChannel());
	}
}

void UTextChatWidget::DeselectedChannelsWidget_OnChannelSelectClickedCallback(UTextChatChannelItemWidget* ChannelItemWidget) {
	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->SelectChannel(ChannelItemWidget->GetChannel());
		TextChatSubsystem->ActivateChannel(ChannelItemWidget->GetChannel());
	}
}

FReply UTextChatWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) {
	if (auto* Widget = GetWidgetToFocus()) {
		Widget->SetFocus();
	}
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UTextChatWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) {
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	const TSharedPtr<SWidget> Widget = TakeWidget();
	if (NewWidgetPath.ContainsWidget(Widget.Get())) {
		if (!IsFocused) {
			IsFocused = true;
			OnFocusReceived.Broadcast();
		}
	} else {
		if (IsFocused) {
			IsFocused = false;
			OnFocusLost.Broadcast();
		}
	}
}

bool UTextChatWidget::DeselectedPanelGetVisibility() const {
	return DeselectedPanelWidget->GetVisibility() == ESlateVisibility::Visible;
}

void UTextChatWidget::DeselectedPanelSetVisibility(bool Visible) {
	if (IsValid(DeselectedPanelWidget)) {
		DeselectedPanelWidget->SetVisibility(Visible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
