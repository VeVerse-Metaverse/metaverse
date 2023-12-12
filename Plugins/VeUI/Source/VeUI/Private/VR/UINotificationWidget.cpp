// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UINotificationWidget.h"

void UUINotificationWidget::NativeConstruct() {
	if (!CloseButtonWidget->GetOnButtonClicked().IsBoundToObject(this)) {
		CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnNotificationCloseButtonClicked.IsBound()) {
				OnNotificationCloseButtonClicked.Broadcast(this);
			}
		});
	}

	Super::NativeConstruct();
}

void UUINotificationWidget::Show() {
	switch (NotificationData.Type) {
	case EUINotificationType::Info:
		if (InfoFadeInSound) {
			PlaySound(InfoFadeInSound);
		}
		break;
	case EUINotificationType::Success:
		if (SuccessFadeInSound) {
			PlaySound(SuccessFadeInSound);
		}
		break;
	case EUINotificationType::Warning:
		if (WarningFadeInSound) {
			PlaySound(WarningFadeInSound);
		}
		break;
	case EUINotificationType::Failure:
		if (FailureFadeInSound) {
			PlaySound(FailureFadeInSound);
		}
		break;
	}

	return Super::Show();
}

// void UUINotificationWidget::HideAndDestroy(const bool bAnimate, const bool bPlaySound) {
// 	Super::HideAndDestroy(bAnimate, bPlaySound);
//
// 	if (OnNotificationClosed.IsBound()) {
// 		OnNotificationClosed.Broadcast(this);
// 	}
// }

void UUINotificationWidget::SetNotificationData(const FUINotificationData InNotificationData) {
	NotificationData = InNotificationData;

	if (HeaderWidget) {
		HeaderWidget->SetText(NotificationData.Header);
	}

	if (MessageWidget) {
		MessageWidget->SetText(NotificationData.Message);
	}

	UpdateStatusIcons();
}

void UUINotificationWidget::HideAndDestroy() {
	Hide();
	RemoveFromParent();
}

void UUINotificationWidget::UpdateStatusIcons() const {
	HideStatusIcons();

	switch (NotificationData.Type) {
	case EUINotificationType::Info:
		if (IconInfoWidget) {
			IconInfoWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	case EUINotificationType::Success:
		if (IconSuccessWidget) {
			IconSuccessWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	case EUINotificationType::Warning:
		if (IconWarningWidget) {
			IconWarningWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	case EUINotificationType::Failure:
		if (IconFailureWidget) {
			IconFailureWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	}
}

void UUINotificationWidget::HideStatusIcons() const {
	if (IconInfoWidget) {
		IconInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IconSuccessWidget) {
		IconSuccessWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IconWarningWidget) {
		IconWarningWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IconFailureWidget) {
		IconFailureWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
