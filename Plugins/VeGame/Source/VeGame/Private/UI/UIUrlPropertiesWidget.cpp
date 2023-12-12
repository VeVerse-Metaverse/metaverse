// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIUrlPropertiesWidget.h"

void UUIUrlPropertiesWidget::SetUrl(const FString& Url) {
	if (IsValid(UrlInputWidget)) {
		UrlInputWidget->SetText(FText::FromString(Url));
	}
	
	SetSaveEnabled(true);
}

FString UUIUrlPropertiesWidget::GetUrl() const {
	if (IsValid(UrlInputWidget)) {
		return UrlInputWidget->GetText().ToString();
	}
	return FString();
}
