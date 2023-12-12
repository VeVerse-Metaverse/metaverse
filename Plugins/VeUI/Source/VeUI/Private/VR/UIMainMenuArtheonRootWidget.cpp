// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIMainMenuArtheonRootWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "Subsystems/UIDialogSubsystem.h"
#include "VR/UISpaceBrowserWidget.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIMainMenuArtheonRootWidget::NativeConstruct() {

	bSpaceBrowserWidgetVisible = true;
	bObjectBrowserWidgetVisible = true;
	bMainMenuWidgetVisible = true;
	bMainWidgetVisible = true;
	bOpenSeaNFTsBrowserWidgetVisible = true;

	NavigateToMainScreen(false, false);

	Super::NativeConstruct();
}

void UUIMainMenuArtheonRootWidget::RegisterCallbacks() {

	if (IsValid(MainMenuWidget)) {
		if (!OnMainMenuSpacesButtonClickedDelegateHandle.IsValid()) {
			OnMainMenuSpacesButtonClickedDelegateHandle = MainMenuWidget->OnSpacesButtonClicked.AddWeakLambda(this, [this]() {
				if (SpaceBrowserWidget) {
					SpaceBrowserWidget->OnNavigate();
				}
				NavigateToSpaceBrowserScreen();
			});
		}
	}


	if (IsValid(MainMenuWidget)) {
		if (!OnMainMenuButtonCloseDelegateHandle.IsValid()) {
			OnMainMenuButtonCloseDelegateHandle = MainMenuWidget->OnReturnMainMenu.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}

	if (IsValid(MainMenuWidget)) {
		if (!OnMainMenuObjectsButtonClickedDelegateHandle.IsValid()) {
			OnMainMenuObjectsButtonClickedDelegateHandle = MainMenuWidget->OnObjectsButtonClicked.AddWeakLambda(this, [this]() {
				if (ObjectBrowserWidget) {
					ObjectBrowserWidget->OnNavigate();
				}
				NavigateToObjectBrowserScreen();
			});
		}
	}

	if (IsValid(MainMenuWidget)) {
		if (!OnOpenSeaNFTsButtonClickedDelegateHandle.IsValid()) {
			OnOpenSeaNFTsButtonClickedDelegateHandle = MainMenuWidget->OnOpenSeaNFTsButtonClicked.AddWeakLambda(this, [this]() {
				OpenSeaAssetBrowserWidget->OnNavigate();
				NavigateToOpenSeaNFTsBrowserScreen();
			});
		}
	}

	if (IsValid(ObjectBrowserWidget)) {
		if (!OnObjectBrowserClosedDelegateHandle.IsValid()) {
			OnObjectBrowserClosedDelegateHandle = ObjectBrowserWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}
	
	if (IsValid(OpenSeaAssetBrowserWidget)) {
		if (!OnOpenSeaNFTsCloseButtonClickedDelegateHandle.IsValid()) {
			OnOpenSeaNFTsCloseButtonClickedDelegateHandle = OpenSeaAssetBrowserWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}

	if (IsValid(SpaceBrowserWidget)) {
		if (!OnSpaceBrowserClosedDelegateHandle.IsValid()) {
			OnSpaceBrowserClosedDelegateHandle = SpaceBrowserWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}

}

void UUIMainMenuArtheonRootWidget::UnregisterCallbacks() {

	if (OnMainMenuSpacesButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(MainMenuWidget)) {
			MainMenuWidget->OnSpacesButtonClicked.Remove(OnMainMenuSpacesButtonClickedDelegateHandle);
			OnMainMenuSpacesButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnMainMenuUsersButtonClickedDelegateHandle.IsValid()) {
		if (MainMenuWidget) {
			MainMenuWidget->OnUsersButtonClicked.Remove(OnMainMenuUsersButtonClickedDelegateHandle);
			OnMainMenuUsersButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnMainMenuButtonCloseDelegateHandle.IsValid()) {
		if (MainMenuWidget) {
			MainMenuWidget->OnReturnMainMenu.Remove(OnMainMenuButtonCloseDelegateHandle);
			OnMainMenuButtonCloseDelegateHandle.Reset();
		}
	}

	if (OnMainMenuObjectsButtonClickedDelegateHandle.IsValid()) {
		if (MainMenuWidget) {
			MainMenuWidget->OnObjectsButtonClicked.Remove(OnMainMenuObjectsButtonClickedDelegateHandle);
			OnMainMenuObjectsButtonClickedDelegateHandle.Reset();
		}
	}
	
	if (OnOpenSeaNFTsButtonClickedDelegateHandle.IsValid()) {
		if (MainMenuWidget) {
			MainMenuWidget->OnOpenSeaNFTsButtonClicked.Remove(OnOpenSeaNFTsButtonClickedDelegateHandle);
			OnOpenSeaNFTsButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnObjectBrowserClosedDelegateHandle.IsValid()) {
		if (ObjectBrowserWidget) {
			ObjectBrowserWidget->OnClosedDelegate.Remove(OnObjectBrowserClosedDelegateHandle);
			OnObjectBrowserClosedDelegateHandle.Reset();
		}
	}

	if (OnOpenSeaNFTsCloseButtonClickedDelegateHandle.IsValid()) {
		if (OpenSeaAssetBrowserWidget) {
			OpenSeaAssetBrowserWidget->OnClosedDelegate.Remove(OnOpenSeaNFTsCloseButtonClickedDelegateHandle);
			OnOpenSeaNFTsCloseButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnSpaceBrowserClosedDelegateHandle.IsValid()) {
		if (SpaceBrowserWidget) {
			SpaceBrowserWidget->OnClosedDelegate.Remove(OnSpaceBrowserClosedDelegateHandle);
			OnSpaceBrowserClosedDelegateHandle.Reset();
		}
	}

}

void UUIMainMenuArtheonRootWidget::NativeOnInitialized() {
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIDialogSubsystem> DialogSubsystem = UIModule->GetDialogSubsystem()) {
			DialogSubsystem->GetOnExitConfirmed().AddWeakLambda(this, [this]() {
				// Check if is hidden.
				Hide();
			});
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Dialog)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

	Super::NativeOnInitialized();
}


void UUIMainMenuArtheonRootWidget::ShowSpaceBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bSpaceBrowserWidgetVisible) {
		bSpaceBrowserWidgetVisible = true;
		SpaceBrowserWidget->ResetState();
		SpaceBrowserWidget->ResetAnimationState();
		SpaceBrowserWidget->Show(bAnimate, bPlaySound);
	}
}

void UUIMainMenuArtheonRootWidget::ShowObjectBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bObjectBrowserWidgetVisible) {
		bObjectBrowserWidgetVisible = true;
		// ObjectBrowserWidget->ResetState();
		// ObjectBrowserWidget->ResetAnimationState();
		ObjectBrowserWidget->Show();
	}
}

void UUIMainMenuArtheonRootWidget::ShowMainMenuWidget(bool bAnimate, bool bPlaySound) {
	if (!bMainMenuWidgetVisible) {
		bMainMenuWidgetVisible = true;
		MainMenuWidget->ResetState();
		MainMenuWidget->ResetAnimationState();
		MainMenuWidget->Show();
	}
}

void UUIMainMenuArtheonRootWidget::ShowOpenSeaNFTsBrowserWidget(bool bAnimate, bool bPlaySound) {
	if (!bOpenSeaNFTsBrowserWidgetVisible) {
		bOpenSeaNFTsBrowserWidgetVisible = true;
		// OpenSeaAssetBrowserWidget->ResetState();
		// OpenSeaAssetBrowserWidget->ResetAnimationState();
		OpenSeaAssetBrowserWidget->Show();
	}
}

void UUIMainMenuArtheonRootWidget::HideSpaceBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (bSpaceBrowserWidgetVisible) {
		bSpaceBrowserWidgetVisible = false;
		SpaceBrowserWidget->Hide();
	}
}

void UUIMainMenuArtheonRootWidget::HideObjectBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (bObjectBrowserWidgetVisible) {
		bObjectBrowserWidgetVisible = false;
		ObjectBrowserWidget->Hide();
	}
}

void UUIMainMenuArtheonRootWidget::HideMainMenuWidget(bool bAnimate, bool bPlaySound) {
	if (bMainMenuWidgetVisible) {
		bMainMenuWidgetVisible = false;
		MainMenuWidget->Hide();
	}
}

void UUIMainMenuArtheonRootWidget::HideOpenSeaNFTsBrowserWidget(bool bAnimate, bool bPlaySound) {
	if (bOpenSeaNFTsBrowserWidgetVisible) {
		bOpenSeaNFTsBrowserWidgetVisible = false;
		if (IsValid(OpenSeaAssetBrowserWidget)) {
			OpenSeaAssetBrowserWidget->Hide();
		}
	}
}

void UUIMainMenuArtheonRootWidget::NavigateToSpaceBrowserScreen(const bool bAnimate, const bool bPlaySound) {
	ShowSpaceBrowserWidget(bAnimate, bPlaySound);
	HideObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuWidget(bAnimate, bPlaySound);
	HideOpenSeaNFTsBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
				BreadcrumbSubsystem->Push(LOCTEXT("Spaces", "Spaces"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}


void UUIMainMenuArtheonRootWidget::NavigateToMainScreen(const bool bAnimate, const bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	HideObjectBrowserWidget(bAnimate, bPlaySound);
	ShowMainMenuWidget(bAnimate, bPlaySound);
	HideOpenSeaNFTsBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

void UUIMainMenuArtheonRootWidget::NavigateToObjectBrowserScreen(const bool bAnimate, const bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	ShowObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuWidget(bAnimate, bPlaySound);
	HideOpenSeaNFTsBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
				BreadcrumbSubsystem->Push(LOCTEXT("Objects", "Objects"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

void UUIMainMenuArtheonRootWidget::NavigateToOpenSeaNFTsBrowserScreen(bool bAnimate, bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	HideObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuWidget(bAnimate, bPlaySound);
	ShowOpenSeaNFTsBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
				BreadcrumbSubsystem->Push(LOCTEXT("OpenSeaNFTs", "OpenSeaNFTs"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

#undef LOCTEXT_NAMESPACE
