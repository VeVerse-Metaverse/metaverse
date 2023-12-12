// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISocialFilterWidget.h"
#include "VeShared.h"

void UUISocialFilterWidget::RegisterCallbacks() {
	if (IsValid(QueryInputWidget)) {
		if (!OnQueryTextCommittedDelegateHandle.IsValid()) {
			OnQueryTextCommittedDelegateHandle = QueryInputWidget->OnTextCommitted.AddUObject(this, &UUISocialFilterWidget::OnQueryTextCommittedCallback);
		}
	}

	if (IsValid(SearchButtonWidget)) {
		if (!OnSearchButtonClickedDelegateHandle.IsValid()) {
			OnSearchButtonClickedDelegateHandle = SearchButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISocialFilterWidget::OnSearchButtonClickedCallback);
		}
	}
}

void UUISocialFilterWidget::UnregisterCallbacks() {
	if (OnQueryTextCommittedDelegateHandle.IsValid()) {
		if (QueryInputWidget) {
			QueryInputWidget->OnTextCommitted.Remove(OnQueryTextCommittedDelegateHandle);
			OnQueryTextCommittedDelegateHandle.Reset();
		}
	}

	if (OnSearchButtonClickedDelegateHandle.IsValid()) {
		if (SearchButtonWidget) {
			SearchButtonWidget->GetOnButtonClicked().Remove(OnSearchButtonClickedDelegateHandle);
			OnSearchButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUISocialFilterWidget::OnQueryTextCommittedCallback(const FText& InText, const ETextCommit::Type InTextCommit) {
	if (InTextCommit == ETextCommit::OnEnter) {
		// LogScreenLogF("Query Text Commit method: %d", InTextCommit);
		if (OnFilterChanged.IsBound()) {
			OnFilterChanged.Broadcast(InText.ToString());
		}
	}
}

void UUISocialFilterWidget::OnSearchButtonClickedCallback() {
	// LogScreenLogF("Search Button clicked");
	if (OnFilterChanged.IsBound()) {
		OnFilterChanged.Broadcast(QueryInputWidget->GetText().ToString());
	}
}
