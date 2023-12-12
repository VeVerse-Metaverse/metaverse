// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Subsystems/UIBreadcrumbSubsystem.h"

const FName FUIBreadcrumbSubsystem::Name = FName("UIBreadcrumbSubsystem");

void FUIBreadcrumbSubsystem::Initialize() {
}

void FUIBreadcrumbSubsystem::Shutdown() {
}

TArray<FText> FUIBreadcrumbSubsystem::GetBreadcrumbs() const {
	return Breadcrumbs;
}

void FUIBreadcrumbSubsystem::Push(const FText& InBreadcrumb) {
	Breadcrumbs.Push(InBreadcrumb);
	if (OnBreadcrumbPushed.IsBound()) {
		OnBreadcrumbPushed.Broadcast(InBreadcrumb);
	}
}

void FUIBreadcrumbSubsystem::Pop() {
	if (Breadcrumbs.Num() <= 0) {
		LogF("attempted to pop empty breadcrumbs");
		return;
	}

	const FText Breadcrumb = Breadcrumbs.Pop();
	if (OnBreadcrumbPopped.IsBound()) {
		OnBreadcrumbPopped.Broadcast(Breadcrumb);
	}
}

void FUIBreadcrumbSubsystem::Reset() {
	Breadcrumbs.Reset();
}
