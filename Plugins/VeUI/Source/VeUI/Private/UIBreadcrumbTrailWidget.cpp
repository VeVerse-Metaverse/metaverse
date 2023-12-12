// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIBreadcrumbTrailWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"

void UUIBreadcrumbTrailWidget::RegisterCallbacks() {
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				OnBreadcrumbPushedDelegateHandle = BreadcrumbSubsystem->OnBreadcrumbPushed.AddUObject(this, &UUIBreadcrumbTrailWidget::OnBreadcrumbPushed);
				OnBreadcrumbPoppedDelegateHandle = BreadcrumbSubsystem->OnBreadcrumbPopped.AddUObject(this, &UUIBreadcrumbTrailWidget::OnBreadcrumbPopped);
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

	Super::RegisterCallbacks();
}

void UUIBreadcrumbTrailWidget::UnregisterCallbacks() {
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				if (OnBreadcrumbPushedDelegateHandle.IsValid()) {
					BreadcrumbSubsystem->OnBreadcrumbPushed.Remove(OnBreadcrumbPushedDelegateHandle);
					OnBreadcrumbPushedDelegateHandle.Reset();
				}
				if (OnBreadcrumbPoppedDelegateHandle.IsValid()) {
					BreadcrumbSubsystem->OnBreadcrumbPopped.Remove(OnBreadcrumbPoppedDelegateHandle);
					OnBreadcrumbPoppedDelegateHandle.Reset();
				}
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

	Super::UnregisterCallbacks();
}

void UUIBreadcrumbTrailWidget::OnBreadcrumbPushed(const FText& InBreadcrumbText) {
	PushCrumb(InBreadcrumbText);
}

void UUIBreadcrumbTrailWidget::OnBreadcrumbPopped(const FText& /*InBreadcrumbText*/) {
	PopCrumb();
}

void UUIBreadcrumbTrailWidget::OnBreadcrumbReset() {
	Reset();
}

void UUIBreadcrumbTrailWidget::Reset() {
	while (HasCrumbs()) {
		const FBreadcrumbInfo Crumb = Crumbs.Pop();

		if (Crumb.DelimiterWidget) {
			Crumb.DelimiterWidget->RemoveFromParent();
		}

		if (Crumb.TextWidget) {
			Crumb.TextWidget->RemoveFromParent();
		}
	}
}

void UUIBreadcrumbTrailWidget::PushCrumb(const FText& InText) {
	FBreadcrumbInfo Crumb;

	if (!WidgetTree) {
		LogErrorF("failed to get a widget tree");
		return;
	}

	if (!IsValid(BreadcrumbDelimiterWidgetClass)) {
		LogErrorF("breadcrumb delimiter widget class is invalid");
		return;
	}

	if (!IsValid(BreadcrumbItemWidgetClass)) {
		LogErrorF("breadcrumb item widget class is invalid");
		return;
	}

	// If there are crumbs already, add the delimiter.
	if (HasCrumbs()) {
		Crumb.DelimiterWidget = WidgetTree->ConstructWidget<UUIBreadcrumbDelimiterWidget>(BreadcrumbDelimiterWidgetClass);
		if (!Crumb.DelimiterWidget) {
			LogErrorF("failed to create a delimiter widget");
			return;
		}
		// Crumb.DelimiterWidget->Brush;
		CrumbContainer->AddChild(Crumb.DelimiterWidget);
	}

	// Add the text block.
	Crumb.TextWidget = WidgetTree->ConstructWidget<UUIBreadcrumbItemWidget>(BreadcrumbItemWidgetClass);
	if (!Crumb.TextWidget) {
		LogErrorF("failed to create a text widget");
		return;
	}
	Crumb.TextWidget->SetText(InText);
	CrumbContainer->AddChild(Crumb.TextWidget);

	// Add the crumb to the array.
	Crumbs.Add(Crumb);
}

void UUIBreadcrumbTrailWidget::PopCrumb() {
	if (Crumbs.Num() <= 0) {
		LogF("attempted to pop empty crumbs");
		return;
	}

	const FBreadcrumbInfo Crumb = Crumbs.Pop();

	if (Crumb.DelimiterWidget) {
		Crumb.DelimiterWidget->RemoveFromParent();
	}

	if (Crumb.TextWidget) {
		Crumb.TextWidget->RemoveFromParent();
	}
}

bool UUIBreadcrumbTrailWidget::HasCrumbs() const {
	return Crumbs.Num() > 0;
}
