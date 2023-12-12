// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UIKeyWidget.h"

void UUIKeyWidget::NativeOnInitialized() {
	bIsFocusable = false;

	if (ButtonWidget) {
		ButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnClicked.IsBound()) {
				if (bIsUppercase) {
					if (UpperCaseKey.IsValid()) {
						OnClicked.Execute(UpperCaseKey);
					} else {
						OnClicked.Execute(Key);
					}
				} else {
					OnClicked.Execute(Key);
				}
			}

			if (ButtonHighlightAnimation) {
				PlayAnimationForward(ButtonHighlightAnimation);
			}
		});
	}

	Super::NativeOnInitialized();
}

void UUIKeyWidget::NativePreConstruct() {
	UpdateLabel();

	Super::NativePreConstruct();
}

void UUIKeyWidget::UpdateLabel() const {
	if (bIsUppercase) {
		if (ButtonWidget) {
			if (UpperCaseLabel.IsEmpty()) {
				ButtonWidget->SetButtonText(Label);
			} else {
				ButtonWidget->SetButtonText(UpperCaseLabel);
			}
		}
	} else {
		if (ButtonWidget) {
			ButtonWidget->SetButtonText(Label);
		}
	}
}

void UUIKeyWidget::SetUppercase(const bool bInIsUppercase) {
	if (bInIsUppercase == bIsUppercase) {
		return;
	}

	bIsUppercase = bInIsUppercase;

	UpdateLabel();
}

void UUIHighlightableKeyWidget::SetUppercase(const bool bInIsUppercase) {
	if (ButtonWidget) {
		if (bInIsUppercase) {
			ButtonWidget->SetButtonPreset(EUIButtonPreset::KeyboardHighlightVR);
		} else {
			ButtonWidget->SetButtonPreset(EUIButtonPreset::KeyboardVR);
		}
	}
	Super::SetUppercase(bInIsUppercase);
}
