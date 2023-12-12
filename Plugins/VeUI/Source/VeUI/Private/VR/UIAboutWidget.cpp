// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIAboutWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"

void UUIAboutWidget::NativeConstruct() {
	if (CloseButtonWidget) {
		if (!CloseButtonWidget->GetOnButtonClicked().IsBoundToObject(this)) {
			CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						BreadcrumbSubsystem->Pop();
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
				if (OnClosed.IsBound()) {
					OnClosed.Broadcast();
				}
			});
		}
	}

	Super::NativeConstruct();
}
