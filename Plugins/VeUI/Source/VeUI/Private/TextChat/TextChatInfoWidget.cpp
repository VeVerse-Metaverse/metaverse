// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "TextChat/TextChatInfoWidget.h"

#include "RpcWebSocketsTextChatSubsystem.h"
#include "UINotifyManagerWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "VeApi.h"
#include "VeRpc.h"
#include "Components/VerticalBoxSlot.h"
#include "TextChat/TextChatNotificationManagerWidget.h"


void UTextChatInfoWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
}

void UTextChatInfoWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->OnMessageReceived.AddUObject(this, &UTextChatInfoWidget::TextChatSubsystem_OnMessageReceivedCallback);
		TextChatSubsystem->OnUnreadCountChanged.AddUObject(this, &UTextChatInfoWidget::TextChatSubsystem_OnUnreadCountChangedCallback);
		TextChatSubsystem_OnUnreadCountChangedCallback();
	}
}

void UTextChatInfoWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->OnMessageReceived.RemoveAll(this);
	}
}

void UTextChatInfoWidget::TextChatSubsystem_OnUnreadCountChangedCallback() {
	if (IsValid(AmountTextWidget)) {
		GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
		if (TextChatSubsystem) {
			FNumberFormattingOptions NumberFormatOptions;
			NumberFormatOptions.MinimumIntegralDigits = 1;
			NumberFormatOptions.MaximumIntegralDigits = 324;
			AmountTextWidget->SetText(FText::AsNumber(TextChatSubsystem->GetUnreadMessagesCount(), &NumberFormatOptions));
		}
	}
}

void UTextChatInfoWidget::TextChatSubsystem_OnMessageReceivedCallback(const TSharedPtr<FRpcWebSocketsTextChatMessage>& Message) {
	if (!GetIsEnabled()) {
		return;
	}
	NotificationManagerWidget->AddMessage(Message);
}
