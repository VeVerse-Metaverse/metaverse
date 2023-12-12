// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UIOpenSeaNftFilterWidget.h"
#include "VeShared.h"

void UUIOpenSeaNftFilterWidget::RegisterCallbacks() {
	if (IsValid(QueryInputWidget)) {
		if (!OnQueryTextCommittedDelegateHandle.IsValid()) {
			OnQueryTextCommittedDelegateHandle = QueryInputWidget->OnTextCommitted.AddUObject(this, &UUIOpenSeaNftFilterWidget::OnQueryTextCommittedCallback);
		}
	}

	if (IsValid(SearchButtonWidget)) {
		if (!OnSearchButtonClickedDelegateHandle.IsValid()) {
			OnSearchButtonClickedDelegateHandle = SearchButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIOpenSeaNftFilterWidget::OnSearchButtonClickedCallback);
		}
	}
}

void UUIOpenSeaNftFilterWidget::UnregisterCallbacks() {
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

void UUIOpenSeaNftFilterWidget::OnQueryTextCommittedCallback(const FText& InText, const ETextCommit::Type InTextCommit) {
	if (InTextCommit == ETextCommit::OnEnter) {
		// LogScreenLogF("Query Text Commit method: %d", InTextCommit);
		if (OnFilterChanged.IsBound()) {
			OnFilterChanged.Broadcast(InText.ToString());
		}
	}
}

void UUIOpenSeaNftFilterWidget::OnSearchButtonClickedCallback() {
	// LogScreenLogF("Search Button clicked");
	if (OnFilterChanged.IsBound()) {
		OnFilterChanged.Broadcast(QueryInputWidget->GetText().ToString());
	}
}
