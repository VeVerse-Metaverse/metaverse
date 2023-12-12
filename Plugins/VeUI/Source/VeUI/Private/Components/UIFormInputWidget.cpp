// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "Components/UIFormInputWidget.h"

#include "Animation/UMGSequencePlayer.h"
#include "Internationalization/Regex.h"

void UUIFormInputWidget::NativeOnInitializedShared() {
	if (InputWidget) {
		if (!InputWidget->OnTextCommitted.IsBoundToObject(this)) {
			InputWidget->OnTextCommitted.AddUObject(this, &UUIFormInputWidget::OnTextCommittedCallback);
		}

		if (!InputWidget->OnInputFocusChanged.IsBoundToObject(this)) {
			InputWidget->OnInputFocusChanged.AddWeakLambda(this, [this](const bool& bFocused) {
				if (OnInputFocusChanged.IsBound()) {
					OnInputFocusChanged.Broadcast(bFocused);
				}

				if (bFocused) {
					SetMessageText(FText());
				}
			});
		}

		if (!InputWidget->OnTextChanged.IsBoundToObject(this)) {
			InputWidget->OnTextChanged.AddUObject(this, &UUIFormInputWidget::OnTextChangedCallback);
		}
	}

	SetMessageText(FText(), false, false);

	Super::NativeOnInitializedShared();
}

void UUIFormInputWidget::NativePreConstruct() {
	if (InputWidget) {
		InputWidget->SetIsPassword(bIsPassword);
		InputWidget->SetHintText(InputHintText);
	}

	if (TitleWidget) {
		if (bShowTitle) {
			TitleWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		} else {
			TitleWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		TitleWidget->SetText(TitleText);
	}

	if (MessageWidget) {
		if (MessageText.IsEmpty()) {
			MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			MessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		MessageWidget->SetText(MessageText);
	}

	Super::NativePreConstruct();
}

void UUIFormInputWidget::ShowMessage(const bool bAnimate, const bool bPlaySound) {
	if (bPlaySound && MessageFadeInSound) {
		PlaySound(MessageFadeInSound);
	}

	if (bAnimate) {
		if (MessageFadeInAnimation) {
			UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(MessageFadeInAnimation);
			if (SequencePlayer) {
				if (!SequencePlayer->OnSequenceFinishedPlaying().IsBoundToObject(this)) {
					SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
						MessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						MessageWidget->SetText(MessageText);
					});
				}
				return;
			}
		} else if (MessageFadeOutAnimation) {
			UUMGSequencePlayer* SequencePlayer = PlayAnimationReverse(MessageFadeOutAnimation);
			if (SequencePlayer) {
				if (!SequencePlayer->OnSequenceFinishedPlaying().IsBoundToObject(this)) {
					SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
						MessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						MessageWidget->SetText(MessageText);
					});
				}
				return;
			}
		}
	}

	MessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	MessageWidget->SetText(MessageText);
}

void UUIFormInputWidget::HideMessage(const bool bAnimate, const bool bPlaySound) {
	if (bPlaySound && MessageFadeOutSound) {
		PlaySound(MessageFadeOutSound);
	}

	if (bAnimate) {
		if (MessageFadeOutAnimation) {
			UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(MessageFadeOutAnimation);
			if (SequencePlayer) {
				if (!SequencePlayer->OnSequenceFinishedPlaying().IsBoundToObject(this)) {
					SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
						MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
						MessageWidget->SetText(MessageText);
					});
				}
				return;
			}
		} else if (MessageFadeInAnimation) {
			UUMGSequencePlayer* SequencePlayer = PlayAnimationReverse(MessageFadeInAnimation);
			if (SequencePlayer) {
				if (!SequencePlayer->OnSequenceFinishedPlaying().IsBoundToObject(this)) {
					SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
						MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
						MessageWidget->SetText(MessageText);
					});
				}
				return;
			}
		}
	}

	MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
	MessageWidget->SetText(MessageText);
}

void UUIFormInputWidget::ShowTitleWidget() const {
	TitleWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUIFormInputWidget::HideTitleWidget() const {
	TitleWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UUIFormInputWidget::SetMessageText(const FText& InText, const bool bAnimate, const bool bPlaySound) {
	if (MessageWidget) {
		if (InText.IsEmpty()) {
			if (MessageText.IsEmpty()) {
				// Do nothing
			} else {
				HideMessage(bAnimate, bPlaySound);
			}
		} else {
			if (MessageText.IsEmpty()) {
				ShowMessage(bAnimate, bPlaySound);
			} else {
				ShowMessage(bAnimate, bPlaySound);
			}
		}
		//
		// if (!MessageText.IsEmpty() && InText.IsEmpty()) {
		// 	HideMessage(bAnimate, bPlaySound);
		// } else {
		// 	ShowMessage(bAnimate, bPlaySound);
		// }
	}

	MessageText = InText;
}

FText UUIFormInputWidget::GetText() const {
	return InputWidget->GetText();
}

void UUIFormInputWidget::SetText(const FText& InText) const {
	InputWidget->SetText(InText);
}

void UUIFormInputWidget::ClearText() {
	InputWidget->ClearText();
}

void UUIFormInputWidget::SetIsReadOnly(const bool bReadonly) {
	if (InputWidget) {
		InputWidget->SetIsReadOnly(bReadonly);
	}
}

bool UUIFormInputWidget::Validate_Implementation() {
	const FRegexPattern Pattern = FRegexPattern(ValidationRegexPattern);
	FRegexMatcher Matcher = FRegexMatcher(Pattern, GetText().ToString());
	if (Matcher.FindNext()) {
		SetMessageText(FText());
		return true;
	}
	SetMessageText(ValidationErrorMessage);
	return false;
}

void UUIFormInputWidget::OnTextCommittedCallback(const FText& InText, const ETextCommit::Type InCommitMethod) {
	if (InText.IsEmpty() && InCommitMethod != ETextCommit::OnEnter) {
		OnTextCommitted.Broadcast(InText, InCommitMethod);
		return;
	}

	if (Validate()) {
		OnTextCommitted.Broadcast(InText, InCommitMethod);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UUIFormInputWidget::OnTextChangedCallback(const FText& InText) {
	OnTextChanged.Broadcast(InText);
}
