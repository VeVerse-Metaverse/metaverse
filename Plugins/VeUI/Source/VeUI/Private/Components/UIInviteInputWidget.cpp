// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "Components/UIInviteInputWidget.h"

#include "Animation/UMGSequencePlayer.h"
#include "Internationalization/Regex.h"

void UUIInviteInputWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
	if (IsValid(EditableTextWidget)) {
		if (!OnTextChanged.IsBoundToObject(this)) {
			OnTextChanged.AddUObject(this, &UUIInviteInputWidget::OnInputTextChangedCallback);
		}

		if (!OnTextCommitted.IsBoundToObject(this)) {
			OnTextCommitted.AddUObject(this, &UUIInviteInputWidget::OnInputTextCommittedCallback);
		}
	}	
}

void UUIInviteInputWidget::OnInputTextChangedCallback(const FText& InText) {
	if (InText.ToString() == PreviousText) {
		return;
	}
	const FText NewInput = CheckInputText(InText);
	if (NewInput.IsEmpty()) {
		SetText(FText::FromString(PreviousText));
		return;
	}
	SetText(NewInput);
	OnInviteTextChanged.Broadcast(NewInput);
}

void UUIInviteInputWidget::OnInputTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod) {
	const FText NewInput = CheckInputText(InText);
	if (NewInput.IsEmpty()) {
		SetText(FText::FromString(PreviousText));
		return;
	}
	SetText(NewInput);
	OnInviteTextCommitted.Broadcast(InText);
}

FText UUIInviteInputWidget::CheckInputText(const FText& InText) {
	const FString InStr = InText.ToString();
	FString NewStr = InStr.Replace(*PreviousText, TEXT(""), ESearchCase::CaseSensitive);
	if(NewStr.IsEmpty()) {
		NewStr = PreviousText;
	}
	const FString LastStr = NewStr.Right(1).ToUpper();
	const FRegexPattern RegexPattern(TEXT("[a-zA-Z0-9]"));
	FRegexMatcher RegexMatcher(RegexPattern, LastStr);
	if (!RegexMatcher.FindNext()) {
		return FText::FromString(FString());
	}
	PreviousText = LastStr;
	return FText::FromString(LastStr);
}
