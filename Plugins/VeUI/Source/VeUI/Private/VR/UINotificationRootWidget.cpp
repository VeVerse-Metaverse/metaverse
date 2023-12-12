// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UINotificationRootWidget.h"

#include "VR/UINotificationWidget.h"
#include "VeShared.h"

UUINotificationRootWidget::UUINotificationRootWidget(): Super() {
	NotificationWidgetClass = UUINotificationWidget::StaticClass();
	MaxVisibleNotifications = 3;
}

void UUINotificationRootWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UUINotificationRootWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (VisibleNotifications.Num() < MaxVisibleNotifications) {
		if (NotificationQueue.Num() > 0) {
			UUINotificationWidget* NotificationWidget = NotificationQueue[0];
			if (NotificationWidget) {
				NotificationWidget->Show();
				// Add to visible container.
				NotificationContainer->AddChild(NotificationWidget);
				VisibleNotifications.AddUnique(NotificationWidget);
				NotificationQueue.RemoveAt(0);
			}
		}
	}

	if (VisibleNotifications.Num() > 0) {
		TArray<UUINotificationWidget*> PendingDeleteNotifications;

		for (UUINotificationWidget* NotificationWidget : VisibleNotifications) {
			NotificationWidget->Lifetime += InDeltaTime;

			if (NotificationWidget->NotificationData.Lifetime > 0 && NotificationWidget->Lifetime > NotificationWidget->NotificationData.Lifetime) {
				NotificationWidget->HideAndDestroy();
				PendingDeleteNotifications.AddUnique(NotificationWidget);
			}
		}

		if (PendingDeleteNotifications.Num() > 0) {
			for (UUINotificationWidget* NotificationWidget : PendingDeleteNotifications) {
				VisibleNotifications.Remove(NotificationWidget);
			}

			PendingDeleteNotifications.Empty();
		}
	}

}

void UUINotificationRootWidget::AddNotification(const FUINotificationData& InNotificationData) {
	APlayerController* PlayerController = GetOwningPlayer();

	UUINotificationWidget* Widget = CreateWidget<UUINotificationWidget>(NotificationWidgetClass);
	Widget->SetNotificationData(InNotificationData);
	Widget->Hide();

	if (!Widget->OnNotificationCloseButtonClicked.IsBoundToObject(this)) {
		Widget->OnNotificationCloseButtonClicked.AddWeakLambda(this, [this](UUINotificationWidget* TargetWidget) {
			VisibleNotifications.Remove(TargetWidget);
			NotificationQueue.Remove(TargetWidget);

			TargetWidget->HideAndDestroy();
		});
	}

	// Enqueue the notification.
	NotificationQueue.AddUnique(Widget);
}
