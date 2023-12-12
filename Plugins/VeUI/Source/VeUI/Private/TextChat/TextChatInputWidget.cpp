// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "TextChat/TextChatInputWidget.h"

#include "Components/EditableTextBox.h"
#include "Components/Button.h"

UTextChatInputWidget::UTextChatInputWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {}

void UTextChatInputWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(TextBoxWidget)) {
		TextBoxWidget->SetText(FText());
		TextBoxWidget->SetClearKeyboardFocusOnCommit(false);
		TextBoxWidget->OnTextCommitted.AddDynamic(this, &UTextChatInputWidget::TextBoxWidget_OnTextCommittedCallback);
		TextBoxWidget->OnTextChanged.AddDynamic(this, &UTextChatInputWidget::TextBoxWidget_OnTextChangedCallback);
	}

	if (IsValid(SendButtonWidget)) {
		SendButtonWidget->OnClicked.AddDynamic(this, &UTextChatInputWidget::SendButtonWidget_OnClickedCallback);
	}
}

void UTextChatInputWidget::NativeOnSendMessage() {
	if (OnMessageSent.IsBound() && IsValid(TextBoxWidget)) {
		const FText Message = TextBoxWidget->GetText();
		OnMessageSent.Broadcast(Message);
	}
	Clear();
}

UWidget* UTextChatInputWidget::GetWidgetToFocus() const {
	return TextBoxWidget;
}

void UTextChatInputWidget::Clear() {
	if (IsValid(TextBoxWidget)) {
		TextBoxWidget->SetText(FText());
	}
}

void UTextChatInputWidget::TextBoxWidget_OnTextCommittedCallback(const FText& Text, ETextCommit::Type CommitMethod) {
	if (CommitMethod == ETextCommit::Type::OnEnter && GetIsEnabled()) {
		NativeOnSendMessage();
	}
}

void UTextChatInputWidget::TextBoxWidget_OnTextChangedCallback(const FText& Text) {
	if (IsValid(SendButtonWidget)) {
		if (Text.IsEmpty()) {
			SendButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			SendButtonWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UTextChatInputWidget::SendButtonWidget_OnClickedCallback() {
	if (!IsValid(TextBoxWidget)) {
		return;
	}
	NativeOnSendMessage();
	TextBoxWidget->SetFocus();
}
