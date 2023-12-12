// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Components/UICommonOptionSwitcherWidget.h"

#include "VeShared.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUICommonOptionSwitcherWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	
	if (IsValid(NameTextWidget)) {
		NameTextWidget->SetText(OptionName);
	}
}

void UUICommonOptionSwitcherWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
	
	if (OptionList.Num()) {
		CurrentIndex = 0;
	}
	UpdateInfo();
}

void UUICommonOptionSwitcherWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (!OnPrevPageButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(PrevButtonWidget)) {
			OnPrevPageButtonClickedDelegateHandle = PrevButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (CurrentIndex > 0) {
					--CurrentIndex;
					UpdateInfo();
				}
			});
		}
	}

	if (!OnNextPageButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(NextButtonWidget)) {
			OnNextPageButtonClickedDelegateHandle = NextButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (CurrentIndex < OptionList.Num() - 1) {
					++CurrentIndex;
					UpdateInfo();
				}
			});
		}
	}
}

void UUICommonOptionSwitcherWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	if (OnPrevPageButtonClickedDelegateHandle.IsValid()) {
		if (PrevButtonWidget) {
			PrevButtonWidget->GetOnButtonClicked().Remove(OnPrevPageButtonClickedDelegateHandle);
		}
		OnPrevPageButtonClickedDelegateHandle.Reset();
	}

	if (OnNextPageButtonClickedDelegateHandle.IsValid()) {
		if (NextButtonWidget) {
			NextButtonWidget->GetOnButtonClicked().Remove(OnNextPageButtonClickedDelegateHandle);
		}
		OnNextPageButtonClickedDelegateHandle.Reset();
	}
}

bool UUICommonOptionSwitcherWidget::SetValue(int32 Value) {
	const int32 Num = OptionList.Num();
	if (!Num) {
		return false;
	}
	
	Value = FMath::Clamp(Value, 0, Num - 1);
	if (CurrentIndex != Value) {
		CurrentIndex = Value;
		UpdateInfo();
	}
	
	return true;
}

void UUICommonOptionSwitcherWidget::UpdateInfo() {
	if (OptionList.IsValidIndex(CurrentIndex)) {
		if (IsValid(OptionTextWidget)) {
			OptionTextWidget->SetText(OptionList[CurrentIndex]);
		}
	
		if (IsValid(PrevButtonWidget)) {
			PrevButtonWidget->SetIsEnabled(CurrentIndex > 0);
		}

		if (IsValid(PrevButtonWidget)) {
			NextButtonWidget->SetIsEnabled(CurrentIndex < OptionList.Num() - 1);
		}
	} else {
		if (IsValid(OptionTextWidget)) {
			OptionTextWidget->SetText(LOCTEXT("OptionError", "Error"));
		}
	
		if (IsValid(PrevButtonWidget)) {
			PrevButtonWidget->SetIsEnabled(false);
		}

		if (IsValid(PrevButtonWidget)) {
			NextButtonWidget->SetIsEnabled(false);
		}
	}
}

#undef LOCTEXT_NAMESPACE
