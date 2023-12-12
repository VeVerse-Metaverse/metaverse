// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "Components/UIMultilineInputWidget.h"

#include "Animation/UMGSequencePlayer.h"
#include "Components/MultiLineEditableText.h"

void UUIMultilineInputWidget::NativePreConstruct() {
	if (IsValid(EditableTextWidget)) {
		EditableTextWidget->SetHintText(HintText);
	}

	if (bShowClearButton) {
		ClearButtonWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	} else {
		ClearButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	Super::NativePreConstruct();
}

void UUIMultilineInputWidget::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) {
	AnimateFocusIn();

	if (OnInputFocusChanged.IsBound()) {
		OnInputFocusChanged.Broadcast(true);
	}

	Super::NativeOnAddedToFocusPath(InFocusEvent);
}

void UUIMultilineInputWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) {
	AnimateFocusOut();

	if (OnInputFocusChanged.IsBound()) {
		OnInputFocusChanged.Broadcast(false);
	}

	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
}

void UUIMultilineInputWidget::NativeOnInitialized() {
	if (IsValid(ClearButtonWidget)) {
		if (!ClearButtonWidget->GetOnButtonClicked().IsBoundToObject(this)) {
			ClearButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				EditableTextWidget->SetText(FText::FromString(TEXT(" ")));
				EditableTextWidget->SetText(FText());
			});
		}
	}

	if (IsValid(EditableTextWidget)) {
		if (!EditableTextWidget->OnTextChanged.IsAlreadyBound(this, &UUIMultilineInputWidget::OnTextChangedCallback)) {
			EditableTextWidget->OnTextChanged.AddDynamic(this, &UUIMultilineInputWidget::OnTextChangedCallback);
		}

		if (!EditableTextWidget->OnTextCommitted.IsAlreadyBound(this, &UUIMultilineInputWidget::OnTextCommittedCallback)) {
			EditableTextWidget->OnTextCommitted.AddDynamic(this, &UUIMultilineInputWidget::OnTextCommittedCallback);
		}
	}

	Super::NativeOnInitialized();
}

UUMGSequencePlayer* UUIMultilineInputWidget::AnimateFocusIn() {
	if (FocusInSound) {
		PlaySound(FocusInSound);
	}

	if (FocusInAnimation) {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(FocusInAnimation);
		return SequencePlayer;
	}

	return nullptr;
}

UUMGSequencePlayer* UUIMultilineInputWidget::AnimateFocusOut() {
	if (FocusOutSound) {
		PlaySound(FocusOutSound);
	}

	if (FocusOutAnimation) {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(FocusOutAnimation);
		return SequencePlayer;
	}

	return nullptr;
}

void UUIMultilineInputWidget::SetHintText(const FText& InText) const {
	EditableTextWidget->SetHintText(InText);
}

FText UUIMultilineInputWidget::GetText() const {
	return EditableTextWidget->GetText();
}

void UUIMultilineInputWidget::SetText(const FText& InText) const {
	EditableTextWidget->SetText(InText);
}

void UUIMultilineInputWidget::SetWidgetFocus() const {
	EditableTextWidget->SetKeyboardFocus();
}

void UUIMultilineInputWidget::OnTextChangedCallback(const FText& InText) {
	if (TextChangeSound) {
		PlaySound(TextChangeSound);
	}

	OnTextChanged.Broadcast(InText);
}

void UUIMultilineInputWidget::OnTextCommittedCallback(const FText& InText, const ETextCommit::Type InCommitMethod) {
	if (TextCommitSound && InCommitMethod == ETextCommit::OnEnter) {
		PlaySound(TextCommitSound);
	}

	OnTextCommitted.Broadcast(InText, InCommitMethod);
}
