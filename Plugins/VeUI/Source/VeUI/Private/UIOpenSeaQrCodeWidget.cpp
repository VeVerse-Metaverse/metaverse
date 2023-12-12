// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UIOpenSeaQrCodeWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VeApi.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIOpenSeaQrCodeWidget::NativeConstruct() {
	
	Super::NativeConstruct();
}

void UUIOpenSeaQrCodeWidget::RegisterCallbacks() {

	if (!OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			OnCloseButtonClickedDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
			});
		}
	}
	
	if (!OnAuthenticateMetamaskButtonClickedDelegateHandle.IsValid()) {
		if (AuthenticationMetamaskButtonWidget) {
			OnAuthenticateMetamaskButtonClickedDelegateHandle = AuthenticationMetamaskButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			
			});
		}
	}
	

}

void UUIOpenSeaQrCodeWidget::UnregisterCallbacks() {
	

	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnAuthenticateMetamaskButtonClickedDelegateHandle.IsValid()) {
		if (AuthenticationMetamaskButtonWidget) {
			AuthenticationMetamaskButtonWidget->GetOnButtonClicked().Remove(OnAuthenticateMetamaskButtonClickedDelegateHandle);
			OnAuthenticateMetamaskButtonClickedDelegateHandle.Reset();
		}
	}

}


#undef LOCTEXT_NAMESPACE
