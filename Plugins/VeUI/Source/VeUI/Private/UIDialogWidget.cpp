// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIDialogWidget.h"

#include "Components/UIButtonWidget.h"
#include "Components/TextBlock.h"

const static FText HeaderDefaultText = NSLOCTEXT("VeUI", "DefaultDialogHeader", "Confirmation");
const static FText MessageDefaultText = NSLOCTEXT("VeUI", "DefaultDialogMessage", "Please confirm the action");
const static FText ButtonOkDefaultText = NSLOCTEXT("VeUI", "DefaultDialogButtonOK", "OK");
const static FText ButtonYesDefaultText = NSLOCTEXT("VeUI", "DefaultDialogButtonYes", "Yes");
const static FText ButtonNoDefaultText = NSLOCTEXT("VeUI", "DefaultDialogButtonNo", "No");
const static FText ButtonCancelDefaultText = NSLOCTEXT("VeUI", "DefaultDialogButtonCancel", "Cancel");

void UUIDialogWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();
	
	if (FirstButtonWidget) {
		if (!FirstButtonWidget->GetOnButtonClicked().IsBound()) {
			FirstButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				OnButtonClicked.ExecuteIfBound(this, 0);
				if (DialogData.bCloseOnButtonClick) {
					HideAndDestroy();
				}
			});
		}
	}

	if (SecondButtonWidget) {
		if (!SecondButtonWidget->GetOnButtonClicked().IsBound()) {
			SecondButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				OnButtonClicked.ExecuteIfBound(this, 1);
				if (DialogData.bCloseOnButtonClick) {
					HideAndDestroy();
				}
			});
		}
	}

	if (ThirdButtonWidget) {
		if (!ThirdButtonWidget->GetOnButtonClicked().IsBound()) {
			ThirdButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				OnButtonClicked.ExecuteIfBound(this, 2);
				if (DialogData.bCloseOnButtonClick) {
					HideAndDestroy();
				}
			});
		}
	}
}

void UUIDialogWidget::HideAndDestroy() {
	Hide();
	RemoveFromParent();
}

void UUIDialogWidget::Open(const FUIDialogData& InDialogData, const FUIDialogButtonClicked& InOnButtonClicked) {
	DialogData = InDialogData;

	OnButtonClicked = InOnButtonClicked;

	UpdateButtons();

	if (HeaderWidget) {
		if (DialogData.HeaderText.IsEmpty()) {
			HeaderWidget->SetText(HeaderDefaultText);
		} else {
			HeaderWidget->SetText(DialogData.HeaderText);
		}
	}

	if (MessageWidget) {
		if (DialogData.HeaderText.IsEmpty()) {
			MessageWidget->SetText(MessageDefaultText);
		} else {
			MessageWidget->SetText(DialogData.MessageText);
		}
	}

	switch (DialogData.Type) {
	case EUIDialogType::Ok:
		if (FirstButtonWidget) {
			FirstButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
		}

		if (FirstButtonWidget) {
			if (DialogData.FirstButtonText.IsEmpty()) {
				FirstButtonWidget->SetButtonText(ButtonOkDefaultText);
			} else {
				FirstButtonWidget->SetButtonText(DialogData.FirstButtonText);
			}
		}
		break;
	case EUIDialogType::OkCancel:
		if (FirstButtonWidget) {
			FirstButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
		}

		if (DialogData.FirstButtonText.IsEmpty()) {
			if (FirstButtonWidget) {
				FirstButtonWidget->SetButtonText(ButtonOkDefaultText);
			}

			if (SecondButtonWidget) {
				SecondButtonWidget->SetButtonText(ButtonCancelDefaultText);
			}
		} else {
			if (FirstButtonWidget) {
				FirstButtonWidget->SetButtonText(DialogData.FirstButtonText);
			}

			if (SecondButtonWidget) {
				SecondButtonWidget->SetButtonText(DialogData.SecondButtonText);
			}
		}
		break;
	case EUIDialogType::YesNoCancel:
		if (DialogData.FirstButtonText.IsEmpty()) {
			if (FirstButtonWidget) {
				FirstButtonWidget->SetButtonText(ButtonYesDefaultText);
			}

			if (SecondButtonWidget) {
				SecondButtonWidget->SetButtonText(ButtonNoDefaultText);
			}

			if (ThirdButtonWidget) {
				ThirdButtonWidget->SetButtonText(ButtonCancelDefaultText);
			}
		} else {
			if (FirstButtonWidget) {
				FirstButtonWidget->SetButtonText(DialogData.FirstButtonText);
			}

			if (SecondButtonWidget) {
				SecondButtonWidget->SetButtonText(DialogData.SecondButtonText);
			}

			if (ThirdButtonWidget) {
				ThirdButtonWidget->SetButtonText(DialogData.ThirdButtonText);
			}
		}
		break;
	}
}

void UUIDialogWidget::Close() {
	HideAndDestroy();
}

void UUIDialogWidget::UpdateButtons() const {
	switch (DialogData.Type) {
	case EUIDialogType::Ok:
		HideButtons();

		if (FirstButtonWidget) {
			FirstButtonWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	case EUIDialogType::OkCancel:
		HideButtons();

		if (FirstButtonWidget) {
			FirstButtonWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		if (SecondButtonWidget) {
			SecondButtonWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	case EUIDialogType::YesNoCancel:
		if (FirstButtonWidget) {
			FirstButtonWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		if (SecondButtonWidget) {
			SecondButtonWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		if (ThirdButtonWidget) {
			ThirdButtonWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	}
}

void UUIDialogWidget::HideButtons() const {
	if (FirstButtonWidget) {
		FirstButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (SecondButtonWidget) {
		SecondButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ThirdButtonWidget) {
		ThirdButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
