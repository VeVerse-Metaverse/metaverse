// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIInspectorUrlPropertiesWidget.h"

void UUIInspectorUrlPropertiesWidget::NativeOnInitialized() {

	Super::NativeOnInitialized();

	if (IsValid(OpenButtonWidget)) {
		OpenButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			FPlatformProcess::LaunchURL(*Url, nullptr, nullptr);
		});
	}
}

void UUIInspectorUrlPropertiesWidget::SetUrl(const FString& InUrl) {
	Url = InUrl;
}
