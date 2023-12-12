// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIBreadcrumbButtonWidget.h"


void UUIBreadcrumbButtonWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	if (ButtonWidget) {
		ButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
			OnButtonClicked.Broadcast();
		});
	}
}

void UUIBreadcrumbButtonWidget::SetText(const FText& InText) const {
	if (ButtonWidget) {
		ButtonWidget->ButtonText = InText;
	}
}
