// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIPagePanelWidget.h"

#include "UIPageContentWidget.h"
#include "Components/HorizontalBox.h"
#include "UIPageManagerWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/UIButtonWidget.h"
#include "UIBreadcrumbDelimiterWidget.h"
#include "UIBreadcrumbItemWidget.h"
#include "UIBreadcrumbButtonWidget.h"
#include "VeShared.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIPagePanelWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	if (NextButtonWidget) {
		NextButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
			if (PageManager.IsValid()) {
				PageManager->ShowNextPage();
			}
		});
	}
	
	if (PrevButtonWidget) {
		PrevButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
			if (PageManager.IsValid()) {
				PageManager->ShowPrevPage();
			}
		});
	}
	
	RefreshButtons();
}

void UUIPagePanelWidget::SetPageManager(UUIPageManagerWidget* InRootWidget) {
	if (OnHistoryChangedDelegateHandle.IsValid()) {
		if (PageManager.IsValid()) {
			PageManager->GetOnHistoryChanged().Remove(OnHistoryChangedDelegateHandle);
		}
		OnHistoryChangedDelegateHandle.Reset();
	}

	PageManager = InRootWidget;

	if (PageManager.IsValid()) {
		if (!OnHistoryChangedDelegateHandle.IsValid()) {
			OnHistoryChangedDelegateHandle = PageManager->GetOnHistoryChanged().AddUObject(this, &UUIPagePanelWidget::OnHistoryChangedCallback);
		}
	}

	RefreshButtons();
}

void UUIPagePanelWidget::OnHistoryChangedCallback(UUIPageManagerWidget* InPageManager) {
	auto* Page = InPageManager->GetCurrentPage();
	if (IsValid(Page)) {
		if (!Page->GetOnInfoChanged().IsBoundToObject(this)) {
			Page->GetOnInfoChanged().AddUObject(this, &UUIPagePanelWidget::OnPageInfoChangedCallback);
		}
	}

	RefreshButtons();
}

void UUIPagePanelWidget::OnPageInfoChangedCallback(UUIPageContentWidget* Page) {
	if (PageManager.IsValid()) {
		if (Page == PageManager->GetCurrentPage()) {
			RefreshButtons();
		}
	}
}

void UUIPagePanelWidget::RefreshButtons() {

	// Clear

	if (IsValid(PrevButtonWidget)) {
		PrevButtonWidget->SetIsEnabled(false);
	}

	if (IsValid(PrevButtonWidget)) {
		NextButtonWidget->SetIsEnabled(false);
	}

	if (IsValid(PathBoxWidget)) {
		PathBoxWidget->ClearChildren();
	}

	if (!PageManager.IsValid()) {
		return;
	}

	// History buttons

	if (IsValid(PrevButtonWidget)) {
		PrevButtonWidget->SetIsEnabled(PageManager->CanShowPrevPage());
	}

	if (IsValid(NextButtonWidget)) {
		NextButtonWidget->SetIsEnabled(PageManager->CanShowNextPage());
	}

	// Breadcrumbs

	if (IsValid(PathBoxWidget) && IsValid(BreadcrumbButtonClass) && IsValid(BreadcrumbDelimiterClass)) {
		const auto Page = PageManager->GetCurrentPage();
		if (!IsValid(Page)) {
			return;
		}

		TArray<FPageInfo> PageList;
		{
			FPageInfo PageInfo = Page->GetInfo();
			PageList.Add(PageInfo);

			UUIPageContentWidget* ParentPage = nullptr;

			do {
				if (PageInfo.ParentPage.IsValid()) {
					ParentPage = PageInfo.ParentPage.Get();
				} else if (IsValid(PageInfo.ParentClass)) {
					ParentPage = PageInfo.ParentClass->GetDefaultObject<UUIPageContentWidget>();
				} else {
					ParentPage = nullptr;
				}

				if (ParentPage) {
					PageInfo = ParentPage->GetInfo();
					PageList.Add(PageInfo);
				}
			} while (ParentPage);
		}

		int32 LastIndex = PageList.Num() - 1;
		for (int32 index = LastIndex; index >= 0; --index) {
			const FPageInfo& PageInfo = PageList[index];

			// todo: make property
			const int32 FontSize = 14;

			// Delimiter

			if (index != LastIndex) {
				if (IsValid(BreadcrumbDelimiterClass)) {
					auto DelimiterWidget = CreateWidget<UUIBreadcrumbDelimiterWidget>(BreadcrumbDelimiterClass);
					if (DelimiterWidget) {
						PathBoxWidget->AddChild(DelimiterWidget);
					}
				}
			}

			// Button parent page

			if (index) {
				if (IsValid(BreadcrumbButtonClass)) {
					auto ButtonWidget = CreateWidget<UUIBreadcrumbButtonWidget>(BreadcrumbButtonClass);
					if (IsValid(ButtonWidget)) {
						ButtonWidget->SetText(PageInfo.Title);

						ButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [&, PageInfo]() {
							PageManager.Get()->OpenPage(PageInfo.Class, PageInfo.Options);
						});

						PathBoxWidget->AddChild(ButtonWidget);
					}
				}
			}

			// Title current page

			else {
				if (IsValid(BreadcrumbItemClass)) {
					auto TitleWidget = CreateWidget<UUIBreadcrumbItemWidget>(BreadcrumbItemClass);
					if (TitleWidget) {
						TitleWidget->SetText(PageInfo.Title);
						PathBoxWidget->AddChild(TitleWidget);
					}
				}
			}

		}
	}
}


#undef LOCTEXT_NAMESPACE
