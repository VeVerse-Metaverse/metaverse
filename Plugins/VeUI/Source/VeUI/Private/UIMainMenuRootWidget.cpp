// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIMainMenuRootWidget.h"

#include "UIPageManagerWidget.h"
#include "UIPagePanelWidget.h"
#include "UISpaceDetailPageWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIMainMenuRootWidget::NativeOnInitialized() {
	if (IsValid(PagePanelWidget) && IsValid(PageManagerWidget)) {
		PagePanelWidget->SetPageManager(PageManagerWidget);
	}

	Super::NativeOnInitialized();
}

void UUIMainMenuRootWidget::ShowMenuWorldDetail(const FGuid& InWorldId) {
	if (PageManagerWidget) {
		PageManagerWidget->OpenPage<UUISpaceDetailPageWidget>(WorldDetailPageClass, InWorldId.ToString(EGuidFormats::DigitsWithHyphensLower));
	}
}

#undef LOCTEXT_NAMESPACE
