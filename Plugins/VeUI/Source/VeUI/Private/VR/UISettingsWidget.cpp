// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UISettingsWidget.h"

#include "Scalability.h"

void UUISettingsWidget::RegisterCallbacks() {
	if (IsValid(CloseButtonWidget)) {
		if (!OnCloseButtonClickedDelegateHandle.IsValid()) {
			OnCloseButtonClickedDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
			});
		}
	}

	if (IsValid(CancelButtonWidget)) {
		if (!OnCancelButtonClickedDelegateHandle.IsValid()) {
			OnCancelButtonClickedDelegateHandle = CancelButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
			});
		}
	}

	if (IsValid(SubmitButtonWidget)) {
		if (!OnSubmitButtonClickedDelegateHandle.IsValid()) {
			OnSubmitButtonClickedDelegateHandle = SubmitButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnSubmitDelegate.IsBound()) {
					OnSubmitDelegate.Broadcast();
				}
			});
		}
	}
}

void UUISettingsWidget::UnregisterCallbacks() {
	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnCancelButtonClickedDelegateHandle.IsValid()) {
		if (CancelButtonWidget) {
			CancelButtonWidget->GetOnButtonClicked().Remove(OnCancelButtonClickedDelegateHandle);
			OnCancelButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnSubmitButtonClickedDelegateHandle.IsValid()) {
		if (SubmitButtonWidget) {
			SubmitButtonWidget->GetOnButtonClicked().Remove(OnSubmitButtonClickedDelegateHandle);
			OnSubmitButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUISettingsWidget::NativeConstruct() {
	
	Super::NativeConstruct();
}

void UUISettingsWidget::ChooseQualitySettings() {
	
}
