// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIObjectFilterWidget.h"
#include "VeShared.h"

void UUIObjectFilterWidget::RegisterCallbacks() {
	if (IsValid(QueryInputWidget)) {
		if (!OnQueryTextCommittedDelegateHandle.IsValid()) {
			OnQueryTextCommittedDelegateHandle = QueryInputWidget->OnTextCommitted.AddUObject(this, &UUIObjectFilterWidget::OnQueryTextCommittedCallback);
		}
	}

	if (IsValid(SearchButtonWidget)) {
		if (!OnSearchButtonClickedDelegateHandle.IsValid()) {
			OnSearchButtonClickedDelegateHandle = SearchButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectFilterWidget::OnSearchButtonClickedCallback);
		}
	}
}

void UUIObjectFilterWidget::UnregisterCallbacks() {
	if (OnQueryTextCommittedDelegateHandle.IsValid()) {
		if (IsValid(QueryInputWidget)) {
			QueryInputWidget->OnTextCommitted.Remove(OnQueryTextCommittedDelegateHandle);
			OnQueryTextCommittedDelegateHandle.Reset();
		}
	}

	if (OnSearchButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(SearchButtonWidget)) {
			SearchButtonWidget->GetOnButtonClicked().Remove(OnSearchButtonClickedDelegateHandle);
			OnSearchButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUIObjectFilterWidget::OnQueryTextCommittedCallback(const FText& InText, const ETextCommit::Type InTextCommit) {
	if (InTextCommit == ETextCommit::OnEnter) {
		// LogScreenLogF("Query Text Commit method: %d", InTextCommit);
		if (OnFilterChanged.IsBound()) {
			OnFilterChanged.Broadcast(InText.ToString());
		}
	}
}

void UUIObjectFilterWidget::OnSearchButtonClickedCallback() {
	// LogScreenLogF("Search Button clicked");
	if (OnFilterChanged.IsBound()) {
		OnFilterChanged.Broadcast(QueryInputWidget->GetText().ToString());
	}
}
