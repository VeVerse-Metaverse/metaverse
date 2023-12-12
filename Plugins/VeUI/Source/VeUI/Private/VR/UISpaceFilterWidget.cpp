// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UISpaceFilterWidget.h"
#include "VeShared.h"

void UUISpaceFilterWidget::RegisterCallbacks() {
	if (QueryInputWidget) {
		if (!OnQueryTextCommittedDelegateHandle.IsValid()) {
			OnQueryTextCommittedDelegateHandle = QueryInputWidget->OnTextCommitted.AddUObject(this, &UUISpaceFilterWidget::OnQueryTextCommittedCallback);
		}
	}

	if (SearchButtonWidget) {
		if (!OnSearchButtonClickedDelegateHandle.IsValid()) {
			OnSearchButtonClickedDelegateHandle = SearchButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceFilterWidget::OnSearchButtonClickedCallback);
		}
	}
}


void UUISpaceFilterWidget::UnregisterCallbacks() {
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

void UUISpaceFilterWidget::OnQueryTextCommittedCallback(const FText& InText, const ETextCommit::Type InTextCommit) {
	if (InTextCommit == ETextCommit::OnEnter) {
		// LogScreenLogF("Query Text Commit method: %d", InTextCommit);
		if (OnFilterChanged.IsBound()) {
			OnFilterChanged.Broadcast(InText.ToString());
		}
	}
}

void UUISpaceFilterWidget::OnSearchButtonClickedCallback() {
	// LogScreenLogF("Search Button clicked");
	if (OnFilterChanged.IsBound()) {
		OnFilterChanged.Broadcast(QueryInputWidget->GetText().ToString());
	}
}
