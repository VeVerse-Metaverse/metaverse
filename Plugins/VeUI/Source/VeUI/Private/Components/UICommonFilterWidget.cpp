// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Components/UICommonFilterWidget.h"


void UUICommonFilterWidget::RegisterCallbacks() {
	if (ModNameFilterWidget) {
		if (!OnSearchTextChangedDelegateHandle.IsValid()) {
			OnSearchTextChangedDelegateHandle = ModNameFilterWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InTextCommit) {
				if (InTextCommit == ETextCommit::OnEnter) {
					if (OnFilterChanged.IsBound()) {
						OnFilterChanged.Broadcast(InText.ToString());
					}
				}
			});
		}
	}

	if (SearchButtonWidget) {
		if (!OnButtonClickedDelegateHandle.IsValid()) {
			OnButtonClickedDelegateHandle = SearchButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnFilterChanged.IsBound() && IsValid(ModNameFilterWidget)) {
					OnFilterChanged.Broadcast(ModNameFilterWidget->GetText().ToString());
				}
			});
		}
	}
}

void UUICommonFilterWidget::UnregisterCallbacks() {
	if (OnSearchTextChangedDelegateHandle.IsValid()) {
		if (ModNameFilterWidget) {
			ModNameFilterWidget->OnTextCommitted.Remove(OnSearchTextChangedDelegateHandle);
			OnSearchTextChangedDelegateHandle.Reset();
		}
	}

	if (OnButtonClickedDelegateHandle.IsValid()) {
		if (SearchButtonWidget) {
			SearchButtonWidget->GetOnButtonClicked().Remove(OnButtonClickedDelegateHandle);
			OnButtonClickedDelegateHandle.Reset();
		}
	}
}
