// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIPageContentWidget.h"

#include "UILog.h"
#include "UIPageManagerWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


UUIPageContentWidget::UUIPageContentWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	Title = NSLOCTEXT("VeUI", "PageNameUnknown", "Unnamed");
}

FPageInfo UUIPageContentWidget::GetInfo() const {
	FPageInfo Info;
	Info.Class = GetClass();
	Info.ParentClass = ParentPageClass.Get();
	Info.ParentPage = ParentPage;
	Info.Title = Title;
	Info.Options = PageOptions;

	return Info;
}

void UUIPageContentWidget::SetPageManager(UUIPageManagerWidget* InPageManager) {
	PageManager = InPageManager;
}

void UUIPageContentWidget::Refresh() {
	NativeOnRefresh();
}

void UUIPageContentWidget::NativeConstruct() {
	// UE_LOG(VeUI, Log, TEXT(__FUNCTION__ " %p (%s)"), this, *Title.ToString());
	Super::NativeConstruct();
}

void UUIPageContentWidget::NativeDestruct() {
	// UE_LOG(VeUI, Log, TEXT(__FUNCTION__ " %p (%s)"), this, *Title.ToString());
	Super::NativeDestruct();
	if (bNoHistory) {
		GetPageManager()->RemovePage(this);
	}
}

void UUIPageContentWidget::SetOptions(const FString& InOptions) {
	PageOptions = InOptions;
	NativeOnRefresh();
	NativeOnInfoChanged();
}

void UUIPageContentWidget::SetTitle(const FText& InTitle) {
	Title = InTitle;
	NativeOnInfoChanged();
}

void UUIPageContentWidget::SetParentPage(UUIPageContentWidget* InParentPage) {
	ParentPage = InParentPage;
	NativeOnInfoChanged();
}

void UUIPageContentWidget::NativeOnRefresh() {
	if (OnRefresh.IsBound()) {
		OnRefresh.Broadcast(this);
	}
}

void UUIPageContentWidget::NativeOnShow() {
	// UE_LOG(VeUI, Log, TEXT(__FUNCTION__ " %p (%s)"), this, *Title.ToString());
	if (OnShow.IsBound()) {
		OnShow.Broadcast(this);
	}
}

void UUIPageContentWidget::NativeOnHide() {
	// UE_LOG(VeUI, Log, TEXT(__FUNCTION__ " %p (%s)"), this, *Title.ToString());
	if (OnHide.IsBound()) {
		OnHide.Broadcast(this);
	}
}

void UUIPageContentWidget::NativeOnInfoChanged() {
	if (OnInfoChanged.IsBound()) {
		OnInfoChanged.Broadcast(this);
	}
}


#undef LOCTEXT_NAMESPACE
