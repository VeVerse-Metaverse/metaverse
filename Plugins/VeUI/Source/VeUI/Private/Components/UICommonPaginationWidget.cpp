// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Components/UICommonPaginationWidget.h"

#include "VeShared.h"


void UUICommonPaginationWidget::RegisterCallbacks() {
	if (FirstPageButtonWidget) {
		if (!OnFirstPageButtonClickedDelegateHandle.IsValid()) {
			OnFirstPageButtonClickedDelegateHandle = FirstPageButtonWidget->GetOnButtonClicked().AddWeakLambda(
				this, [this]() {
					CurrentPage = 0;
					if (OnPageChanged.IsBound()) {
						OnPageChanged.Broadcast(CurrentPage, ItemsPerPage);
					}
					UpdateCurrentPageText();
				});
		}
	}

	if (LastPageButtonWidget) {
		if (!OnLastPageButtonClickedDelegateHandle.IsValid()) {
			OnLastPageButtonClickedDelegateHandle = LastPageButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				CurrentPage = TotalPages;
				if (OnPageChanged.IsBound()) {
					OnPageChanged.Broadcast(CurrentPage, ItemsPerPage);
				}
				UpdateCurrentPageText();
			});
		}
	}

	if (PreviousPageButtonWidget) {
		if (!OnPreviousPageButtonClickedDelegateHandle.IsValid()) {
			OnPreviousPageButtonClickedDelegateHandle = PreviousPageButtonWidget->GetOnButtonClicked().AddWeakLambda(
				this, [this]() {
					if (CurrentPage > 0) {
						--CurrentPage;
						if (OnPageChanged.IsBound()) {
							OnPageChanged.Broadcast(CurrentPage, ItemsPerPage);
						}
						UpdateCurrentPageText();
					}
				});
		}
	}

	if (NextPageButtonWidget) {
		if (!OnNextPageButtonClickedDelegateHandle.IsValid()) {
			OnNextPageButtonClickedDelegateHandle = NextPageButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (CurrentPage < TotalPages || bUnlimited) {
					++CurrentPage;
					if (OnPageChanged.IsBound()) {
						OnPageChanged.Broadcast(CurrentPage, ItemsPerPage);
					}
					UpdateCurrentPageText();
				}
			});
		}
	}
}

void UUICommonPaginationWidget::UnregisterCallbacks() {

	if (OnFirstPageButtonClickedDelegateHandle.IsValid()) {
		if (FirstPageButtonWidget) {
			FirstPageButtonWidget->GetOnButtonClicked().Remove(OnFirstPageButtonClickedDelegateHandle);
			OnFirstPageButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnLastPageButtonClickedDelegateHandle.IsValid()) {
		if (LastPageButtonWidget) {
			LastPageButtonWidget->GetOnButtonClicked().Remove(OnLastPageButtonClickedDelegateHandle);
			OnLastPageButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnPreviousPageButtonClickedDelegateHandle.IsValid()) {
		if (PreviousPageButtonWidget) {
			PreviousPageButtonWidget->GetOnButtonClicked().Remove(OnPreviousPageButtonClickedDelegateHandle);
			OnPreviousPageButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnNextPageButtonClickedDelegateHandle.IsValid()) {
		if (NextPageButtonWidget) {
			NextPageButtonWidget->GetOnButtonClicked().Remove(OnNextPageButtonClickedDelegateHandle);
			OnNextPageButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUICommonPaginationWidget::SetUnlimited(bool bIsUnlimited) {
	bUnlimited = bIsUnlimited;
	if (bUnlimited) {
		LastPageButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	} else {
		LastPageButtonWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UUICommonPaginationWidget::ResetPagination() {
	SetPaginationOptions(0, 0);
	CurrentPage = 0;
}

void UUICommonPaginationWidget::UpdateCurrentPageText() const {
	if (!ShowInfo) {
		return;
	}
	
	if (CurrentPageTextWidget) {
		if (bUnlimited) {
			const int32 First = CurrentPage * ItemsPerPage + 1;
			int32 Last = CurrentPage * ItemsPerPage + ItemsPerPage;
			CurrentPageTextWidget->SetText(
				FText::Format(NSLOCTEXT("VeUI", "CurrentPageStatus", "Displaying results {0} - {1}"), FText::AsNumber(First), FText::AsNumber(Last)));
		} else {
			const int32 First = TotalElements > 0 ? CurrentPage * ItemsPerPage + 1 : 0;
			int32 Last = CurrentPage * ItemsPerPage + ItemsPerPage;
			if (Last >= TotalElements) {
				Last = TotalElements;
			}
			CurrentPageTextWidget->SetText(
				FText::Format(NSLOCTEXT("VeUI", "CurrentPageStatus", "Displaying results {0} - {1} of {2} "), FText::AsNumber(First), FText::AsNumber(Last),
							  FText::AsNumber(TotalElements)));;
		}
	}
}

void UUICommonPaginationWidget::SetPaginationOptions(const int32 InTotalElements, const int32 InItemsPerPage) {
	if (InItemsPerPage != 0) {
		TotalPages = InTotalElements / InItemsPerPage + ((InTotalElements % InItemsPerPage) ? 1 : 0) - 1;
	} else {
		TotalPages = 0;
	}
	TotalElements = InTotalElements;
	ItemsPerPage = InItemsPerPage;
	UpdateCurrentPageText();
}
