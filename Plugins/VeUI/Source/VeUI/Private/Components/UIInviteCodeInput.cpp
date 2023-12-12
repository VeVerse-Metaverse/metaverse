// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "Components/UIInviteCodeInput.h"

void UUIInviteCodeInput::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (!Character0InputWidget->OnInviteTextChanged.IsBoundToObject(this)) {
		Character0InputWidget->OnInviteTextChanged.AddUObject(this, &UUIInviteCodeInput::OnCharacter0TextChangedCallback);
	}

	if (!Character1InputWidget->OnInviteTextChanged.IsBoundToObject(this)) {
		Character1InputWidget->OnInviteTextChanged.AddUObject(this, &UUIInviteCodeInput::OnCharacter1TextChangedCallback);
	}

	if (!Character2InputWidget->OnInviteTextChanged.IsBoundToObject(this)) {
		Character2InputWidget->OnInviteTextChanged.AddUObject(this, &UUIInviteCodeInput::OnCharacter2TextChangedCallback);
	}

	if (!Character3InputWidget->OnInviteTextChanged.IsBoundToObject(this)) {
		Character3InputWidget->OnInviteTextChanged.AddUObject(this, &UUIInviteCodeInput::OnCharacter3TextChangedCallback);
	}

	if (!Character4InputWidget->OnInviteTextChanged.IsBoundToObject(this)) {
		Character4InputWidget->OnInviteTextChanged.AddUObject(this, &UUIInviteCodeInput::OnCharacter4TextChangedCallback);
	}
}

void UUIInviteCodeInput::NativePreConstruct() {
	CharacterInputWidgets.Empty();
	CharacterInputWidgets.Add(Character0InputWidget);
	CharacterInputWidgets.Add(Character1InputWidget);
	CharacterInputWidgets.Add(Character2InputWidget);
	CharacterInputWidgets.Add(Character3InputWidget);
	CharacterInputWidgets.Add(Character4InputWidget);

	for (auto Character : CharacterInputWidgets) {
		if (Character) {
			Character->SetHintText(InputHintText);
		}
	}

	if (IsValid(TitleWidget)) {
		if (bShowTitle) {
			TitleWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		} else {
			TitleWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		TitleWidget->SetText(TitleText);
	}

	if (IsValid(MessageWidget)) {
		MessageWidget->SetText(MessageText);
	}

	Super::NativePreConstruct();
}

void UUIInviteCodeInput::ShowTitleWidget() {
	TitleWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUIInviteCodeInput::HideTitleWidget() {
	TitleWidget->SetVisibility(ESlateVisibility::Collapsed);
}

FText UUIInviteCodeInput::GetText() const {
	FString StringOut;
	for (auto Character : CharacterInputWidgets) {
		if (Character) {
			StringOut.Append(Character->GetText().ToString());
		}
	}
	return FText::FromString(StringOut);
}

void UUIInviteCodeInput::SetText(FText InText) {
	if (InText.IsEmpty()) {
		return;
	}
	const auto InString = InText.ToString();
	TArray<FString> InStringArray;
	InString.ParseIntoArray(InStringArray, *FString());
	if (InStringArray.Num() != CharacterInputWidgets.Num()) {
		return;
	}
	for (int i = 0; CharacterInputWidgets.Num() - 1; i++) {
		if (InStringArray.IsValidIndex(i)) {
			if (CharacterInputWidgets[i]) {
				CharacterInputWidgets[i]->SetText(FText::FromString(InStringArray[i]));
			}
		}
	}
}

void UUIInviteCodeInput::ClearText() {
	for (auto Character : CharacterInputWidgets) {
		if (Character) {
			Character->SetText(FText());
		}
	}
}

void UUIInviteCodeInput::OnCharacter0TextChangedCallback(const FText& InText) {
	if (InText.IsEmpty()) { return; }
	Character1InputWidget->SetWidgetKeyboardFocus();
}

void UUIInviteCodeInput::OnCharacter1TextChangedCallback(const FText& InText) {
	if (InText.IsEmpty()) { return; }
	Character2InputWidget->SetWidgetKeyboardFocus();
}

void UUIInviteCodeInput::OnCharacter2TextChangedCallback(const FText& InText) {
	if (InText.IsEmpty()) { return; }
	Character3InputWidget->SetWidgetKeyboardFocus();
}

void UUIInviteCodeInput::OnCharacter3TextChangedCallback(const FText& InText) {
	if (InText.IsEmpty()) { return; }
	Character4InputWidget->SetWidgetKeyboardFocus();
}

void UUIInviteCodeInput::OnCharacter4TextChangedCallback(const FText& InText) {
	if (InText.IsEmpty()) { return; }
	// this->SetKeyboardFocus();
}
