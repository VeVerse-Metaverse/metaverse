// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UINotificationItemWidget.h"


void UUINotificationItemWidget::NativeConstruct() {
	if (!CloseButtonWidget->GetOnButtonClicked().IsBoundToObject(this)) {
		CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			Hide();
		});
	}

	Super::NativeConstruct();
}

void UUINotificationItemWidget::SetNotificationData(const FUINotificationData InNotificationData) {
	NotificationData = InNotificationData;

	TimeToLive = NotificationData.Lifetime;

	if (HeaderWidget) {
		HeaderWidget->SetText(NotificationData.Header);
	}

	if (MessageWidget) {
		MessageWidget->SetText(NotificationData.Message);
	}

	UpdateStatusIcons();
}

void UUINotificationItemWidget::UpdateStatusIcons() const {
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

void UUINotificationItemWidget::HideStatusIcons() const {
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

void UUINotificationItemWidget::NativeOnShowStarted() {
	Super::NativeOnShowStarted();

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
}
