// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIEditorCogAiPropertiesWidget.h"

#include "VePlaceableMetadata.h"


void UUIEditorCogAiPropertiesWidget::NativeOnInitialized() {
	// Listen for URL commit events.
	if (IsValid(UrlAvatarWidget)) {
		if (!UrlAvatarWidget->OnTextCommitted.IsBound()) {
			UrlAvatarWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				SetSaveEnabled(!InText.IsEmpty());
				NativeOnChanged();
			});
		}
	}

	if (IsValid(OpenAIKeyWidget)) {
		if (!OpenAIKeyWidget->OnTextCommitted.IsBound()) {
			OpenAIKeyWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				SetSaveEnabled(!InText.IsEmpty());
				NativeOnChanged();
			});
		}
	}

	if (IsValid(AiNameWidget)) {
		if (!AiNameWidget->OnTextCommitted.IsBound()) {
			AiNameWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				SetSaveEnabled(!InText.IsEmpty());
				NativeOnChanged();
			});
		}
	}

	if (IsValid(AiDescriptionWidget)) {
		if (!AiDescriptionWidget->OnTextCommitted.IsBound()) {
			AiDescriptionWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				SetSaveEnabled(!InText.IsEmpty());
				NativeOnChanged();
			});
		}
	}

	if (IsValid(PersonalityTraitsWidget)) {
		if (!PersonalityTraitsWidget->OnTextCommitted.IsBound()) {
			PersonalityTraitsWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				SetSaveEnabled(!InText.IsEmpty());
				NativeOnChanged();
			});
		}
	}

	if (IsValid(AiVoiceWidget)) {
		AiVoiceWidget->OnSelectionChanged.AddDynamic(this, &UUIEditorCogAiPropertiesWidget::NativeOnChangedVoice);
	}

	Super::NativeOnInitialized();
}


void UUIEditorCogAiPropertiesWidget::NativeOnChangedVoice(FString SelectedItem, ESelectInfo::Type SelectionType) {
	NativeOnChanged();
}

void UUIEditorCogAiPropertiesWidget::SetProperties(const FString& InUrlAvatar, const FString& InOpenAIKey, const FString& InAiName, const FString& InAiDescription, const FString& InPersonalityTraits, const FString& InAiVoice) {
	UrlAvatarWidget->SetText(FText::FromString(InUrlAvatar));
	OpenAIKeyWidget->SetText(FText::FromString(InOpenAIKey));
	AiNameWidget->SetText(FText::FromString(InAiName));
	AiDescriptionWidget->SetText(FText::FromString(InAiDescription));
	PersonalityTraitsWidget->SetText(FText::FromString(InPersonalityTraits));
	if (!InAiVoice.IsEmpty()) {
		if (InAiVoice == "Female (Deep)") {
			AiVoiceWidget->SetSelectedIndex(0);
		} else if (InAiVoice == "Female (Neutral)") {
			AiVoiceWidget->SetSelectedIndex(1);
		} else if (InAiVoice == "Female (High)") {
			AiVoiceWidget->SetSelectedIndex(2);
		} else if (InAiVoice == "Female (Intense)") {
			AiVoiceWidget->SetSelectedIndex(3);
		} else if (InAiVoice == "Female (Bright)") {
			AiVoiceWidget->SetSelectedIndex(4);
		} else if (InAiVoice == "Male (Intense)") {
			AiVoiceWidget->SetSelectedIndex(5);
		} else if (InAiVoice == "Male (Calm)") {
			AiVoiceWidget->SetSelectedIndex(6);
		} else if (InAiVoice == "Male (Bright)") {
			AiVoiceWidget->SetSelectedIndex(7);
		} else if (InAiVoice == "Male (Deep)") {
			AiVoiceWidget->SetSelectedIndex(8);
		}
	}
}

FString UUIEditorCogAiPropertiesWidget::GetUrl() {
	// Get text from widget.
	if (!UrlAvatarWidget->GetText().IsEmpty()) {
		return UrlAvatarWidget->GetText().ToString();
	}
	return "";
}

FString UUIEditorCogAiPropertiesWidget::GetOpenAiKey() {
	// Get text from widget.
	if (!OpenAIKeyWidget->GetText().IsEmpty()) {
		return OpenAIKeyWidget->GetText().ToString();
	}
	return "";
}

FString UUIEditorCogAiPropertiesWidget::GetAiName() {
	// Get text from widget.
	if (!AiNameWidget->GetText().IsEmpty()) {
		return AiNameWidget->GetText().ToString();
	}
	return "";
}

FString UUIEditorCogAiPropertiesWidget::GetAiDescription() {
	// Get text from widget.
	if (!AiDescriptionWidget->GetText().IsEmpty()) {
		return AiDescriptionWidget->GetText().ToString();
	}
	return "";
}

FString UUIEditorCogAiPropertiesWidget::GetPersonalityTraits() {
	// Get text from widget.
	if (!PersonalityTraitsWidget->GetText().IsEmpty()) {
		return PersonalityTraitsWidget->GetText().ToString();
	}
	return "";
}

FString UUIEditorCogAiPropertiesWidget::GetAiVoice() {
	// Get text from widget.
	if (!AiVoiceWidget->GetSelectedOption().IsEmpty()) {
		return AiVoiceWidget->GetSelectedOption();
	}
	return "";
}
