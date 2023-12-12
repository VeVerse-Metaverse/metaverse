// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIUserFilterWidget.h"


void UUIUserFilterWidget::RegisterCallbacks() {
	if (UserNameFilterWidget) {
		if (!OnSearchTextChangedDelegateHandle.IsValid()) {
			OnSearchTextChangedDelegateHandle = UserNameFilterWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InTextCommit) {
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
				if (OnFilterChanged.IsBound()) {
					OnFilterChanged.Broadcast(UserNameFilterWidget->GetText().ToString());
				}
			});
		}
	}
}

void UUIUserFilterWidget::UnregisterCallbacks() {
	if (OnSearchTextChangedDelegateHandle.IsValid()) {
		if (UserNameFilterWidget) {
			UserNameFilterWidget->OnTextCommitted.Remove(OnSearchTextChangedDelegateHandle);
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
