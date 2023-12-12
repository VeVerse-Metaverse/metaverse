// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIEditorMediaUrlPropertiesWidget.h"

void UUIEditorMediaUrlPropertiesWidget::NativeOnInitialized() {

	if (IsValid(UrlInputWidget)) {
		if (!UrlInputWidget->OnTextCommitted.IsBound()) {
			UrlInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				OnUrlTextCommittedCallback(InText, InCommitMethod);
			});
		}
	}

	Super::NativeOnInitialized();
}

void UUIEditorMediaUrlPropertiesWidget::OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod) {

	if (!InText.IsEmpty()) {
		Url = InText.ToString();
	}
}

void UUIEditorMediaUrlPropertiesWidget::SetUrl(const FString& InUrl) {

	if (!InUrl.IsEmpty()) {
		UrlInputWidget->SetText(FText::FromString(InUrl));
	}
	Url = InUrl;
}

FString UUIEditorMediaUrlPropertiesWidget::GetUrl() {
	return Url;
}
