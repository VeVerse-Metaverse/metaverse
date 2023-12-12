// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIUserVoiceWidget.h"

#include "Components/Image.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIUserVoiceWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();
}

void UUIUserVoiceWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();
}

void UUIUserVoiceWidget::NativeConstruct() {
	Super::NativeConstruct();

	SetVoiceStatus(EVoiceWidgetStatus::Inactive);
}

void UUIUserVoiceWidget::SetVoiceStatus(const bool InVoiceActive) const {
	if (InVoiceActive) {
		VoiceActiveWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		VoiceActiveLowWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceInactiveWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceMutedWidget->SetVisibility(ESlateVisibility::Hidden);
	} else {
		VoiceActiveWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceActiveLowWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceInactiveWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		VoiceMutedWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUIUserVoiceWidget::SetVoiceStatus(const EVoiceWidgetStatus InVoiceStatus) const {
	switch (InVoiceStatus) {
	case EVoiceWidgetStatus::Active:
		VoiceActiveWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		VoiceActiveLowWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceInactiveWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceMutedWidget->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EVoiceWidgetStatus::ActiveLow:
		VoiceActiveWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceActiveLowWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		VoiceInactiveWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceMutedWidget->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EVoiceWidgetStatus::Inactive:
		VoiceActiveWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceActiveLowWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceInactiveWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		VoiceMutedWidget->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EVoiceWidgetStatus::Muted:
		VoiceActiveWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceActiveLowWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceInactiveWidget->SetVisibility(ESlateVisibility::Hidden);
		VoiceMutedWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	default:
		break;
	}
}

#undef LOCTEXT_NAMESPACE
