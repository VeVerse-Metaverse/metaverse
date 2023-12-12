// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "Components/UIInputWidget.h"

#include "Subsystems/UIFocusSubsystem.h"
#include "Subsystems/UIKeySubsystem.h"
#include "Animation/UMGSequencePlayer.h"
#include "VeUI.h"

void UUIInputWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	if (ClearButtonWidget) {
		ClearButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			EditableTextWidget->SetText(FText::FromString(TEXT(" ")));
			EditableTextWidget->SetText(FText());
		});
	}

	if (EditableTextWidget) {
		EditableTextWidget->OnTextChanged.AddDynamic(this, &UUIInputWidget::OnTextChangedCallback);
		EditableTextWidget->OnTextCommitted.AddDynamic(this, &UUIInputWidget::OnTextCommittedCallback);
	}
}

void UUIInputWidget::NativePreConstruct() {
	if (EditableTextWidget) {
		EditableTextWidget->SetIsPassword(bIsPassword);
		EditableTextWidget->SetHintText(HintText);
		EditableTextWidget->SetClearKeyboardFocusOnCommit(false);
	}

	if (bShowClearButton) {
		ClearButtonWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	} else {
		ClearButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	Super::NativePreConstruct();
}

void UUIInputWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (EditableTextWidget && EditedTextSaved) {
		EditableTextWidget->SetText(EditedText);
		EditedText = FText();
		EditedTextSaved = false;
	}
}

void UUIInputWidget::NativeDestruct() {
	Super::NativeDestruct();
	GET_MODULE_SUBSYSTEM(FocusSubsystem, UI, Focus);
	if (FocusSubsystem) {
		FocusSubsystem->SetFocusedWidget(nullptr);
	}
	if (EditableTextWidget) {
		EditedText = EditableTextWidget->GetText();
		EditedTextSaved = true;
	}
}

void UUIInputWidget::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) {
	AnimateFocusIn();

	if (OnInputFocusChanged.IsBound()) {
		OnInputFocusChanged.Broadcast(true);
	}

	// Update focused widget.
	GET_MODULE_SUBSYSTEM(FocusSubsystem, UI, Focus);
	if (FocusSubsystem) {
		FocusSubsystem->SetFocusedWidget(this);
	}

	Super::NativeOnAddedToFocusPath(InFocusEvent);
}

void UUIInputWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) {
	AnimateFocusOut();

	if (OnInputFocusChanged.IsBound()) {
		OnInputFocusChanged.Broadcast(false);
	}

	// Clicked on the other widget with the mouse.
	// if (InFocusEvent.GetCause() == EFocusCause::Mouse) {
	// 	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
	// 		if (const FUIFocusSubsystemPtr FocusSubsystemPtr = UIModule->GetFocusSubsystem()) {
	// 			const uint32 UserIndex = FSlateApplication::Get().GetUserIndexForKeyboard();
	// 			// If not a keyboard was focused, we can reset focused widget.
	// 			if (!FocusSubsystemPtr->CheckIfKeyboardIsFocused(UserIndex)) {
	// 				FocusSubsystemPtr->SetFocusedWidget(nullptr);
	// 			}
	// 		}
	// 	}
	// }

	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
}

void UUIInputWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent) {
	Super::NativeOnFocusLost(InFocusEvent);
}

UUMGSequencePlayer* UUIInputWidget::AnimateFocusIn() {
	if (FocusInSound) {
		PlaySound(FocusInSound);
	}

	if (FocusInAnimation) {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(FocusInAnimation);
		return SequencePlayer;
	}

	return nullptr;
}

UUMGSequencePlayer* UUIInputWidget::AnimateFocusOut() {
	if (FocusOutSound) {
		PlaySound(FocusOutSound);
	}

	if (FocusOutAnimation) {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(FocusOutAnimation);
		return SequencePlayer;
	}

	return nullptr;
}

void UUIInputWidget::SetIsPassword(const bool InIsPassword) {
	bIsPassword = InIsPassword;

	if (EditableTextWidget) {
		EditableTextWidget->SetIsPassword(InIsPassword);
	}
}

void UUIInputWidget::SetHintText(const FText& InText) const {
	if (EditableTextWidget) {
		EditableTextWidget->SetHintText(InText);
	}
}

FText UUIInputWidget::GetText() const {
	if (EditableTextWidget) {
		return EditableTextWidget->GetText();
	}
	return FText();
}

void UUIInputWidget::SetText(const FText& InText) {
	if (EditableTextWidget) {
		EditableTextWidget->SetText(InText);
	}
}

void UUIInputWidget::ClearText() {
	EditableTextWidget->SetText(FText::FromString(TEXT("1")));
	EditableTextWidget->SetText(FText::FromString(TEXT("")));
}

void UUIInputWidget::SetIsReadOnly(const bool bReadonly) {
	if (EditableTextWidget) {
		EditableTextWidget->SetIsReadOnly(bReadonly);
	}
}

void UUIInputWidget::SetWidgetFocus() const {
	if (EditableTextWidget) {
		EditableTextWidget->SetFocus();
	}
}

void UUIInputWidget::SetWidgetKeyboardFocus() const {
	if (EditableTextWidget) {
		EditableTextWidget->SetKeyboardFocus();
	}
}

bool UUIInputWidget::ReceiveKeyEvent(const FKeyEvent& KeyEvent) {
	if (EditableTextWidget) {
		FString Original = EditableTextWidget->GetText().ToString();

		bool bHasCharCode;
		uint32 CharCode;

		FUIKeySubsystem::GetCharCode(KeyEvent.GetKey(), bHasCharCode, CharCode);

		if (bHasCharCode) {
			const TCHAR Char = StaticCast<TCHAR>(CharCode);
			Original.AppendChar(Char);
			EditableTextWidget->SetText(FText::FromString(Original));
			return true;
		}
	}

	return false;
}

// bool UUIInputWidget::ReceiveCharacterInput(const FString& InCharacter) {
// 	if (EditableTextWidget) {
// 		const FString Original = EditableTextWidget->GetText().ToString();
// 		EditableTextWidget->SetText(FText::FromString(Original + InCharacter));
// 		return true;
// 	}
// 	return false;
// }

void UUIInputWidget::OnTextChangedCallback(const FText& InText) {
	if (TextChangeSound) {
		PlaySound(TextChangeSound);
	}

	OnTextChanged.Broadcast(InText);
}

void UUIInputWidget::OnTextCommittedCallback(const FText& InText, const ETextCommit::Type InCommitMethod) {
	if (TextCommitSound && InCommitMethod == ETextCommit::OnEnter) {
		PlaySound(TextCommitSound);
	}

	OnTextCommitted.Broadcast(InText, InCommitMethod);
}
