// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Debug/UIDownloadDebuggerItemWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"

void UUIDownloadDebuggerItemWidget::SetInfo(const FGuid& Id, const FString& Url) const {
	if (IdTextWidget) {
		IdTextWidget->SetText(FText::FromString(Id.ToString(EGuidFormats::DigitsWithHyphensLower)));
	}

	if (UrlTextWidget) {
		UrlTextWidget->SetText(FText::FromString(Url));
	}
}

void UUIDownloadDebuggerItemWidget::SetProgress(const float Progress, const int64 TotalSize) const {
	if (ProgressWidget) {
		ProgressWidget->SetWidthOverride(Progress * 100.0f);
	}

	if (ProgressTextWidget) {
		ProgressTextWidget->SetText(FText::Format(FText::FromString(TEXT("{0} of {1}")), FText::AsPercent(Progress), FText::AsMemory(TotalSize)));
	}
}

void UUIDownloadDebuggerItemWidget::SetResult(const FFileDownloadResult& Result) const {
	if (ResultTextWidget) {
		if (Result.bSuccessful) {
			ResultTextWidget->SetText(FText::Format(FText::FromString(TEXT("OK: {0}")), FText::FromString(Result.Path)));
		} else {
			ResultTextWidget->SetText(FText::Format(FText::FromString(TEXT("Error: {0}")), FText::FromString(Result.Error)));
		}
	}
}
