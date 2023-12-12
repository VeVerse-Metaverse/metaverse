// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIPageManagerWidget.h"

#include "VeUI.h"
#include "VeShared.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "UIAnimationWidget.h"
#include "UIPageContentWidget.h"
#include "Subsystems/UIPageSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"


UUIPageManagerWidget::UUIPageManagerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	SetClipping(EWidgetClipping::ClipToBoundsAlways);
}

void UUIPageManagerWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	GET_MODULE_SUBSYSTEM(PageSubsystem, UI, Page);
	if (PageSubsystem) {
		PageSubsystem->SetPageManagerUI(this);
	}

	PlayAnimationPage = false;

	if (bAutoOpenHomePage) {
		OpenHomePage();
	}
}

void UUIPageManagerWidget::NativeConstruct() {
	Super::NativeConstruct();
}

UUIPageContentWidget* UUIPageManagerWidget::OpenPage(const TSubclassOf<UUIPageContentWidget> Class, const FString& Options) {
	if (!IsValid(Class)) {
		return nullptr;
	}

	if (TObjectPtr<UUIPageContentWidget> Page = CreateWidget<UUIPageContentWidget>(Class)) {
		Page->SetOptions(Options);
		ShowPage(Page);
		return Page;
	}

	LogWarningF("failed to create a widget");
	return nullptr;
}

UUIPageContentWidget* UUIPageManagerWidget::OpenHomePage(const FString& Options) {
	return OpenPage(HomePageClass, Options);
}

bool UUIPageManagerWidget::CanShowFirstPage() const {
	return HistoryAnimations.Num() && HistoryCurrentIndex != 0;
}

UUIPageContentWidget* UUIPageManagerWidget::ShowFirstPage(bool Refresh) {
	// Temporary fix fast navigation error
	if (CurrentAnimationWidget_OnShowFinishedHandle.IsValid()) {
		return nullptr;
	}

	if (!CanShowFirstPage()) {
		return nullptr;
	}

	HistoryCurrentIndex = 0;

	if (HistoryAnimations.IsValidIndex(HistoryCurrentIndex)) {
		auto* AnimationWidget = HistoryAnimations[HistoryCurrentIndex];
		auto* Page = AnimationWidget->GetContentWidget<UUIPageContentWidget>();

		if (CurrentAnimationWidget.IsValid()) {
			if (CurrentAnimationWidget_OnShowFinishedHandle.IsValid()) {
				QueuePageForShow = Page;
				return Page;
			}
			HidePage();
		}

		CurrentAnimationWidget = AnimationWidget;
		NativeOnHistoryChanged();
		ShowAnimationWidget(AnimationWidget);

		if (Refresh) {
			if (IsValid(Page)) {
				Page->Refresh();
			} else {
				VeLogWarningFunc("Page is nullptr");
			}
		}

		return Page;
	}

	return nullptr;
}

bool UUIPageManagerWidget::CanShowNextPage() const {
	return HistoryAnimations.Num() && HistoryCurrentIndex != HistoryAnimations.Num() - 1;
}

UUIPageContentWidget* UUIPageManagerWidget::ShowNextPage(const bool Refresh) {
	// Temporary fix fast navigation error
	if (CurrentAnimationWidget_OnShowFinishedHandle.IsValid()) {
		return nullptr;
	}

	if (!CanShowNextPage()) {
		return nullptr;
	}

	++HistoryCurrentIndex;

	if (HistoryAnimations.IsValidIndex(HistoryCurrentIndex)) {
		auto* AnimationWidget = HistoryAnimations[HistoryCurrentIndex];
		auto* Page = AnimationWidget->GetContentWidget<UUIPageContentWidget>();

		if (CurrentAnimationWidget.IsValid()) {
			if (CurrentAnimationWidget_OnShowFinishedHandle.IsValid()) {
				QueuePageForShow = Page;
				return Page;
			}
			HidePage();
		}

		CurrentAnimationWidget = AnimationWidget;
		NativeOnHistoryChanged();
		ShowAnimationWidget(AnimationWidget);

		if (Refresh) {
			if (IsValid(Page)) {
				Page->Refresh();
			} else {
				VeLogWarningFunc("Page is nullptr");
			}
		}

		return Page;
	}

	return nullptr;
}

bool UUIPageManagerWidget::CanShowPrevPage() const {
	return HistoryAnimations.Num() && HistoryCurrentIndex != 0;
}

UUIPageContentWidget* UUIPageManagerWidget::ShowPrevPage(const bool Refresh) {
	// Temporary fix fast navigation error
	if (CurrentAnimationWidget_OnShowFinishedHandle.IsValid()) {
		return nullptr;
	}

	if (!CanShowPrevPage()) {
		return nullptr;
	}

	--HistoryCurrentIndex;

	if (HistoryAnimations.IsValidIndex(HistoryCurrentIndex)) {
		auto* AnimationWidget = HistoryAnimations[HistoryCurrentIndex];
		auto* Page = AnimationWidget->GetContentWidget<UUIPageContentWidget>();

		if (CurrentAnimationWidget.IsValid()) {
			if (CurrentAnimationWidget_OnShowFinishedHandle.IsValid()) {
				QueuePageForShow = Page;
				return Page;
			}
			HidePage();
		}

		CurrentAnimationWidget = AnimationWidget;
		NativeOnHistoryChanged();
		ShowAnimationWidget(AnimationWidget);

		if (Refresh) {
			if (IsValid(Page)) {
				Page->Refresh();
			} else {
				VeLogWarningFunc("Page is nullptr");
			}
		}

		return Page;
	}

	return nullptr;
}

bool UUIPageManagerWidget::RemovePage(UUIPageContentWidget* Page) {
	TArray<UUIAnimationWidget*> DeleteList;

	// Find all animations contain the page
	int32 HistoryNum = HistoryAnimations.Num();
	int32 HistoryOffset = 0;
	for (int Index = 0; Index < HistoryNum; ++Index) {
		auto* AnimationWidget = HistoryAnimations[Index];
		if (AnimationWidget->GetContentWidget<UUIPageContentWidget>() == Page) {
			// if (AnimationWidget == CurrentAnimationWidget) {
			// 	return false;
			// }
			if (Index < HistoryCurrentIndex) {
				++HistoryOffset;
			}
			DeleteList.Add(AnimationWidget);
		}
	}

	// Delete all animations contain the page
	if (DeleteList.Num()) {
		for (auto* AnimationWidget : DeleteList) {
			HistoryAnimations.Remove(AnimationWidget);
		}
		HistoryCurrentIndex -= HistoryOffset;
		NativeOnHistoryChanged();
		return true;
	}

	return false;
}

void UUIPageManagerWidget::CloseAllPages() {
	if (QueuePageForShow.IsValid()) {
		QueuePageForShow = nullptr;
	}
	HidePage();

	HistoryAnimations.Empty();
	NativeOnHistoryChanged();
}

// Create animation widget, add it to history and show animation widget 
void UUIPageManagerWidget::ShowPage(UUIPageContentWidget* NewPage, const bool bAddToHistory) {
	if (NewPage == QueuePageForShow || NewPage == GetCurrentPage()) {
		return;
	}

	if (CurrentAnimationWidget.IsValid()) {
		if (CurrentAnimationWidget_OnShowFinishedHandle.IsValid()) {
			QueuePageForShow = NewPage;
			return;
		}
		HidePage();
	}

	if (!IsValid(NewPage)) {
		return;
	}

	// Create animation widget
	UUIAnimationWidget* AnimationWidget = nullptr;
	if (IsValid(NewPage->AnimationClass)) {
		AnimationWidget = CreateWidget<UUIAnimationWidget>(NewPage->AnimationClass);
	} else if (IsValid(DefaultAnimationClass)) {
		AnimationWidget = CreateWidget<UUIAnimationWidget>(DefaultAnimationClass);
	}

	// todo: fallback case without animation
	if (!AnimationWidget) {
		VeLogErrorFunc("failed to create a widget animation");
		return;
	}

	CurrentAnimationWidget = AnimationWidget;

	CurrentAnimationWidget->SetContentWidget(NewPage);
	NewPage->SetPageManager(this);

	// Add page to history
	if (bAddToHistory) {
		// if (!PageHistory.Contains(NewPage)) {
		const int32 RemoveIndex = HistoryCurrentIndex + 1;
		if (HistoryAnimations.IsValidIndex(RemoveIndex)) {
			HistoryAnimations.RemoveAt(RemoveIndex, HistoryAnimations.Num() - RemoveIndex);
		}
		HistoryAnimations.Add(AnimationWidget);
		HistoryCurrentIndex = HistoryAnimations.Num() - 1;
		// }
		NativeOnHistoryChanged();
	}

	// Show animation widget
	ShowAnimationWidget(AnimationWidget);
}

void UUIPageManagerWidget::ShowAnimationWidget(UUIAnimationWidget* AnimationWidget) {
	if (IsValid(ContainerWidget)) {
		UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(ContainerWidget->AddChild(AnimationWidget));
		if (OverlaySlot) {
			OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
			OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		}
	}

	if (!CurrentAnimationWidget_OnShowFinishedHandle.IsValid()) {
		CurrentAnimationWidget_OnShowFinishedHandle = AnimationWidget->GetOnShowFinished().AddUObject(this, &UUIPageManagerWidget::CurrentAnimationWidget_OnShowFinishedCallback);

		if (PlayAnimationPage) {
			AnimationWidget->Show(true);
		} else {
			AnimationWidget->Show(false);
			PlayAnimationPage = true;
		}

		NativeOnPageShow(AnimationWidget->GetContentWidget<UUIPageContentWidget>());
	}
}

void UUIPageManagerWidget::HidePage() {
	if (!CurrentAnimationWidget.IsValid()) {
		return;
	}

	CurrentAnimationWidget->GetOnHideFinished().AddWeakLambda(this, [this](UUIAnimationWidget* InAnimationWidget) {
		if (IsValid(ContainerWidget)) {
			ContainerWidget->RemoveChild(InAnimationWidget);
		}
		InAnimationWidget->GetOnHideFinished().RemoveAll(this);
	});

	NativeOnPageHide(CurrentAnimationWidget->GetContentWidget<UUIPageContentWidget>());
	CurrentAnimationWidget->Hide();
	CurrentAnimationWidget = nullptr;
}

UUIPageContentWidget* UUIPageManagerWidget::GetCurrentPage() const {
	if (CurrentAnimationWidget.IsValid()) {
		return CurrentAnimationWidget->GetContentWidget<UUIPageContentWidget>();
	}
	return nullptr;
}

void UUIPageManagerWidget::CurrentAnimationWidget_OnShowFinishedCallback(UUIAnimationWidget* InPageAnimationWidget) {
	if (CurrentAnimationWidget_OnShowFinishedHandle.IsValid()) {
		CurrentAnimationWidget->GetOnShowFinished().Remove(CurrentAnimationWidget_OnShowFinishedHandle);
		CurrentAnimationWidget_OnShowFinishedHandle.Reset();
	}

	if (QueuePageForShow.IsValid()) {
		// Hide current page
		HidePage();

		// Show page from queue
		UUIPageContentWidget* NewPage = QueuePageForShow.Get();
		QueuePageForShow = nullptr;
		ShowPage(NewPage);
	}
}

void UUIPageManagerWidget::NativeOnPageShow(UUIPageContentWidget* Page) {
	if (IsValid(Page)) {
		Page->NativeOnShow();
		OnPageShow.Broadcast(Page);
	} else {
		VeLogWarningFunc("Page is nullptr");
	}
}

void UUIPageManagerWidget::NativeOnPageHide(UUIPageContentWidget* Page) {
	if (IsValid(Page)) {
		Page->NativeOnHide();
		OnPageHide.Broadcast(Page);
	} else {
		VeLogWarningFunc("Page is nullptr");
	}
}

void UUIPageManagerWidget::NativeOnHistoryChanged() {
	OnHistoryChanged.Broadcast(this);
}


#undef LOCTEXT_NAMESPACE
