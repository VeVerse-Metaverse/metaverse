// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "TextChat/TextChatNotificationManagerWidget.h"

#include "Components/VerticalBoxSlot.h"
#include "TextChat/TextChatNotificationItemWidget.h"


void UTextChatNotificationManagerWidget::AddMessage(const TSharedPtr<FRpcWebSocketsTextChatMessage>& Message) {
	if (NotificationClass) {
		TObjectPtr<UTextChatNotificationItemWidget> Notification = CreateWidget<UTextChatNotificationItemWidget>(NotificationClass);
		Notification->SetMessage(Message);
		AddItem(Notification);
	}
}

void UTextChatNotificationManagerWidget::NativeOnChildAdded(UPanelSlot* InSlot) {
	Super::NativeOnChildAdded(InSlot);
	if (auto* WidgetSlot = Cast<UVerticalBoxSlot>(InSlot)) {
		WidgetSlot->SetPadding(NotificationPadding);
		WidgetSlot->SetHorizontalAlignment(NotificationHorizontalAlignment); 
		WidgetSlot->SetVerticalAlignment(NotificationVerticalAlignment);
	}
}
