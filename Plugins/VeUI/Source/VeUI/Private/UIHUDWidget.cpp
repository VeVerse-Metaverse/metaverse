// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UIHUDWidget.h"

#include "VeShared.h"
#include "VeUI.h"
#include "UINotificationIconWidget.h"
#include "UIPawnWidgetBase.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

void UUIHUDWidget::NativeConstruct() {
	Super::NativeConstruct();

	HideNotification();
	HideVoiceIndicator();
}

void UUIHUDWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
}

void UUIHUDWidget::ShowVoiceIndicator() const {
	if (VoiceIconWidget) {
		VoiceIconWidget->Show();
	}
}

void UUIHUDWidget::HideVoiceIndicator() const {
	if (VoiceIconWidget) {
		VoiceIconWidget->Hide();
	}
}

void UUIHUDWidget::ShowNotification(const bool bLoop) const {
	if (NotificationIconWidget) {
		NotificationIconWidget->Show();
		NotificationIconWidget->PlayRippleAnimation(bLoop);
	}
}

void UUIHUDWidget::HideNotification() const {
	if (NotificationIconWidget) {
		NotificationIconWidget->Hide();
	}
}

void UUIHUDWidget::ShowInspectWidget() {
	if (IsValid(InspectWidget)) {
		InspectWidget->Show();
	}
}

void UUIHUDWidget::HideInspectWidget() {
	if (IsValid(InspectWidget)) {
		InspectWidget->Hide();
	}
}
