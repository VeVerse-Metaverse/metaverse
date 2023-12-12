// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UINotificationManagerWidget.h"

#include "UINotificationItemWidget.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "VeUI.h"
#include "Components/VerticalBoxSlot.h"


UUINotificationManagerWidget::UUINotificationManagerWidget(): Super() {
	MaxVisibleNotifications = 3;
}

void UUINotificationManagerWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		NotificationSubsystem->OnNotification.AddUObject(this, &UUINotificationManagerWidget::AddNotification);
	}
}

void UUINotificationManagerWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		NotificationSubsystem->OnNotification.RemoveAll(this);
	}
}

void UUINotificationManagerWidget::AddNotification(const FUINotificationData& NotificationData) {
	if (NotificationClass) {
		const auto Notification = CreateWidget<UUINotificationItemWidget>(NotificationClass);
		Notification->SetNotificationData(NotificationData);
		AddItem(Notification);
	}
}

void UUINotificationManagerWidget::NativeOnChildAdded(UPanelSlot* InSlot) {
	Super::NativeOnChildAdded(InSlot);
	if (auto* WidgetSlot = Cast<UVerticalBoxSlot>(InSlot)) {
		WidgetSlot->SetPadding(NotificationPadding);
		WidgetSlot->SetHorizontalAlignment(NotificationHorizontalAlignment); 
		WidgetSlot->SetVerticalAlignment(NotificationVerticalAlignment);
	}
}
