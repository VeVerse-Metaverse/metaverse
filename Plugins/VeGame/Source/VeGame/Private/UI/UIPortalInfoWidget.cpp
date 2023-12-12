// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIPortalInfoWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIPortalInfoWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (DownloadButtonWidget) {
		DownloadButtonWidget->OnReleased.AddDynamic(this, &UUIPortalInfoWidget::DownloadButton_OnClicked);
	}

	UpdateState(EVePortalState::NoPak);
	UpdateName(TEXT(""));
	HideDownloadButton();
	HideLoadingProgress();
}

void UUIPortalInfoWidget::UpdateName(const FString& InName) const {
	if (!NameTextWidget) {
		return;
	}

	if (InName.IsEmpty()) {
		if (PortalTextWidget) {
			PortalTextWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	} else {
		if (PortalTextWidget) {
			PortalTextWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}

	NameTextWidget->SetText(FText::FromString(InName));
}

void UUIPortalInfoWidget::UpdateNameColor(const FLinearColor InColor) const {
	if (!NameTextWidget) {
		return;
	}

	NameTextWidget->SetColorAndOpacity(FSlateColor(InColor));
}

void UUIPortalInfoWidget::UpdateStateColor(const FLinearColor InColor) const {
	if (!StateTextWidget) {
		return;
	}

	StateTextWidget->SetColorAndOpacity(FSlateColor(InColor));
}

void UUIPortalInfoWidget::UpdateState(const EVePortalState InState) const {
	if (!StateTextWidget) {
		return;
	}

	switch (InState) {
	case EVePortalState::Disabled:
		StateTextWidget->SetText(FText::FromString(TEXT("DISABLED")));
		break;
	case EVePortalState::Offline:
		StateTextWidget->SetText(FText::FromString(TEXT("OFFLINE")));
		break;
	case EVePortalState::Loading:
		StateTextWidget->SetText(FText::FromString(TEXT("LOADING")));
		break;
	case EVePortalState::WaitServer:
		StateTextWidget->SetText(FText::FromString(TEXT("CONNECTING")));
		break;
	case EVePortalState::Downloading:
		StateTextWidget->SetText(FText::FromString(TEXT("DOWNLOADING")));
		break;
	case EVePortalState::Online:
	case EVePortalState::Local:
		StateTextWidget->SetText(FText::FromString(TEXT("ONLINE")));
		break;
	case EVePortalState::ConfirmDownload:
		StateTextWidget->SetText(FText::FromString(TEXT("DOWNLOAD")));
		break;
	case EVePortalState::Error:
		StateTextWidget->SetText(FText::FromString(TEXT("ERROR")));
		break;
	case EVePortalState::Editor:
		StateTextWidget->SetText(FText::FromString(TEXT("SDK")));
		break;
	case EVePortalState::NoPak:
		StateTextWidget->SetText(FText::FromString(TEXT("UNAVAILABLE")));
		break;
	}
}

void UUIPortalInfoWidget::UpdateLoadingProgress(const float InPercent) const {
	if (IsValid(LoadingProgressWidget)) {
		LoadingProgressWidget->SetProgressRatio(InPercent);
		StateTextWidget->SetText(FText::Format(FText::FromString(TEXT("DOWNLOADING {0}")), FText::AsPercent(InPercent)));
	}
}

void UUIPortalInfoWidget::UpdateDownloadSize(const int64 InDownloadSize) {
	if (IsValid(ConfirmButtonTextBlock)) {
		if (InDownloadSize > 0) {
			ConfirmButtonTextBlock->SetText(FText::Format(LOCTEXT("ConfirmButtonDownloadSize", "Download {0}"), FText::AsMemory(InDownloadSize)));
		} else {
			ConfirmButtonTextBlock->SetText(FText::FromString(TEXT("Unknown")));
		}
	}
}

void UUIPortalInfoWidget::ShowLoadingProgress() const {
	if (IsValid(LoadingProgressWidget)) {
		LoadingProgressWidget->Show();
	}
}

void UUIPortalInfoWidget::HideLoadingProgress() const {
	if (IsValid(LoadingProgressWidget)) {
		LoadingProgressWidget->Hide();
	}
}

void UUIPortalInfoWidget::ShowDownloadButton() const {
	if (IsValid(DownloadButtonWidget)) {
		DownloadButtonWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UUIPortalInfoWidget::HideDownloadButton() const {
	if (IsValid(DownloadButtonWidget)) {
		DownloadButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUIPortalInfoWidget::DownloadButton_OnClicked() {
	OnDownloadClicked.Broadcast();
}

#undef LOCTEXT_NAMESPACE
