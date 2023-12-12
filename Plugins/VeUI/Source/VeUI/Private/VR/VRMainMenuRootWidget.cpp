// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/VRMainMenuRootWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "Subsystems/UIDialogSubsystem.h"
#include "VR/UIMainMenuWidget.h"
#include "VR/UISpaceBrowserWidget.h"
// #include "Subsystems/UIMenuSubsystem.h"
#include "Subsystems/UICharacterCustomizationSubsystem.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"


class UUIMenuSubsystem;

void UVRMainMenuRootWidget::NativeConstruct() {
	bSpaceBrowserWidgetVisible = true;
	bSettingsWidgetVisible = true;
	bServerBrowserWidgetVisible = true;
	bModBrowserWidgetVisible = true;
	bUserBrowserWidgetVisible = true;
	bAboutWidgetVisible = true;
	//bObjectBrowserWidgetVisible = true;
	bMainMenuWidgetVisible = true;
	bMainWidgetVisible = true;
	bMainMenuArtheonWidgetVisible = true;
	bAvatarBrowserWidgetVisible = true;

	NavigateToMainScreen(false, false);

	Super::NativeConstruct();
}

void UVRMainMenuRootWidget::RegisterCallbacks() {
	if (IsValid(MainMenuWidget)) {
		if (!OnMainMenuModsButtonClickedDelegateHandle.IsValid()) {
			OnMainMenuModsButtonClickedDelegateHandle = MainMenuWidget->OnModsButtonClicked.AddWeakLambda(this, [this]() {
				if (ModBrowserWidget) {
					ModBrowserWidget->OnNavigate();
				}
				NavigateToModBrowserScreen();
			});
		}
	}

	if (IsValid(MainMenuWidget)) {
		if (!OnMainMenuSpacesButtonClickedDelegateHandle.IsValid()) {
			OnMainMenuSpacesButtonClickedDelegateHandle = MainMenuWidget->OnSpacesButtonClicked.AddWeakLambda(this, [this]() {
				NavigateToSpaceBrowserScreen();
			});
		}
	}

	if (IsValid(MainMenuWidget)) {
		if (!OnMainMenuUsersButtonClickedDelegateHandle.IsValid()) {
			OnMainMenuUsersButtonClickedDelegateHandle = MainMenuWidget->OnUsersButtonClicked.AddWeakLambda(this, [this]() {
				if (UserBrowserWidget) {
					UserBrowserWidget->OnNavigate();
				}
				NavigateToUserBrowserScreen();
			});
		}
	}

	if (IsValid(MainMenuWidget)) {
		if (!OnMainMenuSettingsButtonClickedDelegateHandle.IsValid()) {
			OnMainMenuSettingsButtonClickedDelegateHandle = MainMenuWidget->OnSettingsButtonClicked.AddWeakLambda(this, [this]() {
				NavigateToSettingsScreen();
			});
		}
	}

	if (IsValid(MainMenuWidget)) {
		if (!OnMainMenuServersButtonClickedDelegateHandle.IsValid()) {
			OnMainMenuServersButtonClickedDelegateHandle = MainMenuWidget->OnServersButtonClicked.AddWeakLambda(this, [this]() {
				NavigateToServerBrowserScreen();
			});
		}
	}

	if (IsValid(MainMenuWidget)) {
		if (!OnMainMenuAboutButtonClickedDelegateHandle.IsValid()) {
			OnMainMenuAboutButtonClickedDelegateHandle = MainMenuWidget->OnAboutButtonClicked.AddWeakLambda(this, [this]() {
				NavigateToAboutScreen();
			});
		}
	}

	if (IsValid(AboutWidget)) {
		if (!OnMainMenuAboutButtonCloseDelegateHandle.IsValid()) {
			OnMainMenuAboutButtonCloseDelegateHandle = AboutWidget->OnClosed.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
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
			OnMainMenuObjectsButtonClickedDelegateHandle = MainMenuWidget->OnArteonDLCButtonClicked.AddWeakLambda(this, [this]() {
				//NavigateToObjectBrowserScreen();
				NavigateToMainMenuArtheonWidget();
			});
		}
	}

	if (IsValid(MainMenuWidget)) {
		if (!OnAvatarBrowserButtonClickedDelegateHandle.IsValid()) {
			OnAvatarBrowserButtonClickedDelegateHandle = MainMenuWidget->OnAvatarButtonClicked.AddWeakLambda(this, [this]() {
				if (AvatarBrowserWidget) {
					AvatarBrowserWidget->Refresh();
				}
				NavigateToAvatarBrowserWidget();
			});
		}
	}

	if (IsValid(MainMenuArtheonWidget)) {
		if (!OnBackMainMenuButtonClickedDelegateHandle.IsValid()) {
			OnBackMainMenuButtonClickedDelegateHandle = MainMenuArtheonWidget->MainMenuWidget->OnBackToLe7elButtonClicked.AddWeakLambda(this, [this]() {
				//NavigateToObjectBrowserScreen();
				NavigateToMainScreen();
			});
		}
	}

	// if (IsValid(ObjectBrowserWidget)) {
	// 	if (!OnObjectBrowserClosedDelegateHandle.IsValid()) {
	// 		OnObjectBrowserClosedDelegateHandle = ObjectBrowserWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
	// 			NavigateToMainScreen();
	// 		});
	// 	}
	// }

	if (IsValid(SpaceBrowserWidget)) {
		if (!OnSpaceBrowserClosedDelegateHandle.IsValid()) {
			OnSpaceBrowserClosedDelegateHandle = SpaceBrowserWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}

	if (IsValid(ModBrowserWidget)) {
		if (!OnModBrowserClosedDelegateHandle.IsValid()) {
			OnModBrowserClosedDelegateHandle = ModBrowserWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}

	if (IsValid(UserBrowserWidget)) {
		if (!OnUserBrowserClosedDelegateHandle.IsValid()) {
			OnUserBrowserClosedDelegateHandle = UserBrowserWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}

	if (IsValid(ServerBrowserWidget)) {
		if (!OnServerBrowserClosedDelegateHandle.IsValid()) {
			OnServerBrowserClosedDelegateHandle = ServerBrowserWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}

	if (IsValid(AvatarBrowserWidget)) {
		if (!OnAvatarBrowserClosedDelegateHandle.IsValid()) {
			OnAvatarBrowserClosedDelegateHandle = AvatarBrowserWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}

	if (IsValid(SettingsWidget)) {
		if (!OnSettingsClosedDelegateHandle.IsValid()) {
			OnSettingsClosedDelegateHandle = SettingsWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}

	if (IsValid(SettingsWidget)) {
		if (!OnSettingsSubmitDelegateHandle.IsValid()) {
			OnSettingsSubmitDelegateHandle = SettingsWidget->OnSubmitDelegate.AddWeakLambda(this, [this]() {
				NavigateToMainScreen();
			});
		}
	}
}

void UVRMainMenuRootWidget::UnregisterCallbacks() {
	if (OnMainMenuModsButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(MainMenuWidget)) {
			MainMenuWidget->OnModsButtonClicked.Remove(OnMainMenuModsButtonClickedDelegateHandle);
			OnMainMenuModsButtonClickedDelegateHandle.Reset();
		}
	}

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

	if (OnMainMenuSettingsButtonClickedDelegateHandle.IsValid()) {
		if (MainMenuWidget) {
			MainMenuWidget->OnSettingsButtonClicked.Remove(OnMainMenuSettingsButtonClickedDelegateHandle);
			OnMainMenuSettingsButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnMainMenuServersButtonClickedDelegateHandle.IsValid()) {
		if (MainMenuWidget) {
			MainMenuWidget->OnServersButtonClicked.Remove(OnMainMenuServersButtonClickedDelegateHandle);
			OnMainMenuServersButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnMainMenuAboutButtonClickedDelegateHandle.IsValid()) {
		if (MainMenuWidget) {
			MainMenuWidget->OnAboutButtonClicked.Remove(OnMainMenuAboutButtonClickedDelegateHandle);
			OnMainMenuAboutButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnMainMenuAboutButtonCloseDelegateHandle.IsValid()) {
		if (AboutWidget) {
			AboutWidget->OnClosed.Remove(OnMainMenuAboutButtonCloseDelegateHandle);
			OnMainMenuAboutButtonCloseDelegateHandle.Reset();
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
			MainMenuWidget->OnArteonDLCButtonClicked.Remove(OnMainMenuObjectsButtonClickedDelegateHandle);
			OnMainMenuObjectsButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnBackMainMenuButtonClickedDelegateHandle.IsValid()) {
		if (MainMenuArtheonWidget && MainMenuWidget) {
			MainMenuArtheonWidget->MainMenuWidget->OnBackToLe7elButtonClicked.Remove(OnBackMainMenuButtonClickedDelegateHandle);
			OnBackMainMenuButtonClickedDelegateHandle.Reset();
		}
	}

	// if (OnObjectBrowserClosedDelegateHandle.IsValid()) {
	// 	if (ObjectBrowserWidget) {
	// 		ObjectBrowserWidget->OnClosedDelegate.Remove(OnObjectBrowserClosedDelegateHandle);
	// 		OnObjectBrowserClosedDelegateHandle.Reset();
	// 	}
	// }

	if (OnSpaceBrowserClosedDelegateHandle.IsValid()) {
		if (SpaceBrowserWidget) {
			SpaceBrowserWidget->OnClosedDelegate.Remove(OnSpaceBrowserClosedDelegateHandle);
			OnSpaceBrowserClosedDelegateHandle.Reset();
		}
	}

	if (OnModBrowserClosedDelegateHandle.IsValid()) {
		if (ModBrowserWidget) {
			ModBrowserWidget->OnClosedDelegate.Remove(OnModBrowserClosedDelegateHandle);
			OnModBrowserClosedDelegateHandle.Reset();
		}
	}

	if (OnUserBrowserClosedDelegateHandle.IsValid()) {
		if (UserBrowserWidget) {
			UserBrowserWidget->OnClosedDelegate.Remove(OnUserBrowserClosedDelegateHandle);
			OnUserBrowserClosedDelegateHandle.Reset();
		}
	}

	if (OnServerBrowserClosedDelegateHandle.IsValid()) {
		if (ServerBrowserWidget) {
			ServerBrowserWidget->OnClosedDelegate.Remove(OnServerBrowserClosedDelegateHandle);
			OnServerBrowserClosedDelegateHandle.Reset();
		}
	}

	if (OnAvatarBrowserClosedDelegateHandle.IsValid()) {
		if (AvatarBrowserWidget) {
			AvatarBrowserWidget->OnClosedDelegate.Remove(OnAvatarBrowserClosedDelegateHandle);
			OnAvatarBrowserClosedDelegateHandle.Reset();
		}
	}

	if (OnSettingsClosedDelegateHandle.IsValid()) {
		if (SettingsWidget) {
			SettingsWidget->OnClosedDelegate.Remove(OnSettingsClosedDelegateHandle);
			OnSettingsClosedDelegateHandle.Reset();
		}
	}

	if (OnSettingsSubmitDelegateHandle.IsValid()) {
		if (SettingsWidget) {
			SettingsWidget->OnSubmitDelegate.Remove(OnSettingsSubmitDelegateHandle);
			OnSettingsSubmitDelegateHandle.Reset();
		}
	}
}

void UVRMainMenuRootWidget::NativeOnInitialized() {
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


void UVRMainMenuRootWidget::ShowSpaceBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bSpaceBrowserWidgetVisible) {
		bSpaceBrowserWidgetVisible = true;
		SpaceBrowserWidget->ResetState();
		SpaceBrowserWidget->ResetAnimationState();
		SpaceBrowserWidget->Show(bAnimate, bPlaySound);
	}
}

void UVRMainMenuRootWidget::ShowUserBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bUserBrowserWidgetVisible) {
		bUserBrowserWidgetVisible = true;
		UserBrowserWidget->ResetState();
		UserBrowserWidget->ResetAnimationState();
		UserBrowserWidget->Show(bAnimate, bPlaySound);
	}
}

void UVRMainMenuRootWidget::ShowModBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bModBrowserWidgetVisible) {
		bModBrowserWidgetVisible = true;
		ModBrowserWidget->ResetState();
		ModBrowserWidget->ResetAnimationState();
		ModBrowserWidget->Show(bAnimate, bPlaySound);
	}
}

void UVRMainMenuRootWidget::ShowSettingsWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bSettingsWidgetVisible) {
		bSettingsWidgetVisible = true;
		SettingsWidget->ResetState();
		SettingsWidget->ResetAnimationState();
		SettingsWidget->Show(bAnimate, bPlaySound);
	}
}

// void UVRMainMenuRootWidget::ShowServersWidget(const bool bAnimate, const bool bPlaySound) {
// 	if (!bServerBrowserWidgetVisible) {
// 		bServerBrowserWidgetVisible = true;
// 		ServerBrowserWidget->ResetState();
// 		ServerBrowserWidget->ResetAnimationState();
// 		ServerBrowserWidget->Show(bAnimate, bPlaySound);
// 	}
// }

void UVRMainMenuRootWidget::ShowAboutWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bAboutWidgetVisible) {
		bAboutWidgetVisible = true;
		AboutWidget->ResetState();
		AboutWidget->ResetAnimationState();
		AboutWidget->Show(bAnimate, bPlaySound);
	}
}

// void UUIMainMenuRootWidget::ShowObjectBrowserWidget(const bool bAnimate, const bool bPlaySound) {
// 	if (!bObjectBrowserWidgetVisible) {
// 		bObjectBrowserWidgetVisible = true;
// 		ObjectBrowserWidget->ResetState();
// 		ObjectBrowserWidget->ResetAnimationState();
// 		ObjectBrowserWidget->Show(bAnimate, bPlaySound);
// 	}
// }

void UVRMainMenuRootWidget::ShowMainMenuArtheonWidget(bool bAnimate, bool bPlaySound) {
	if (!bMainMenuArtheonWidgetVisible) {
		bMainMenuArtheonWidgetVisible = true;
		MainMenuArtheonWidget->ResetState();
		MainMenuArtheonWidget->ResetAnimationState();
		MainMenuArtheonWidget->Show(bAnimate, bPlaySound);
	}
}

void UVRMainMenuRootWidget::ShowMainMenuWidget(bool bAnimate, bool bPlaySound) {
	if (!bMainMenuWidgetVisible) {
		bMainMenuWidgetVisible = true;
		MainMenuWidget->ResetState();
		MainMenuWidget->ResetAnimationState();
		MainMenuWidget->Show(bAnimate, bPlaySound);
	}
}


void UVRMainMenuRootWidget::ShowAvatarBrowserWidget(bool bAnimate, bool bPlaySound) {
	if (!bAvatarBrowserWidgetVisible) {
		bAvatarBrowserWidgetVisible = true;
		AvatarBrowserWidget->ResetState();
		AvatarBrowserWidget->ResetAnimationState();
		AvatarBrowserWidget->Show(bAnimate, bPlaySound);
	}
	
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
			CharacterCustomizationSubsystem->OpenCustomizerPreview();
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }

	// if (const UUIMenuSubsystem* MenuSubsystem = GetWorld()->GetSubsystem<UUIMenuSubsystem>()) {
	// 	MenuSubsystem->EnableCustomizer();
	// } else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Menu)); }
}

void UVRMainMenuRootWidget::HideSpaceBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (bSpaceBrowserWidgetVisible) {
		bSpaceBrowserWidgetVisible = false;
		SpaceBrowserWidget->Hide(bAnimate, bPlaySound);
	}
}

void UVRMainMenuRootWidget::HideUserBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (bUserBrowserWidgetVisible) {
		bUserBrowserWidgetVisible = false;
		UserBrowserWidget->Hide(bAnimate, bPlaySound);
	}
}

void UVRMainMenuRootWidget::HideModBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (bModBrowserWidgetVisible) {
		bModBrowserWidgetVisible = false;
		ModBrowserWidget->Hide(bAnimate, bPlaySound);
	}
}

void UVRMainMenuRootWidget::HideSettingsWidget(const bool bAnimate, const bool bPlaySound) {
	if (bSettingsWidgetVisible) {
		bSettingsWidgetVisible = false;
		SettingsWidget->Hide(bAnimate, bPlaySound);
	}
}

void UVRMainMenuRootWidget::HideServersWidget(const bool bAnimate, const bool bPlaySound) {
	if (bServerBrowserWidgetVisible) {
		bServerBrowserWidgetVisible = false;
		ServerBrowserWidget->Hide();
	}
}

void UVRMainMenuRootWidget::HideAboutWidget(const bool bAnimate, const bool bPlaySound) {
	if (bAboutWidgetVisible) {
		bAboutWidgetVisible = false;
		AboutWidget->Hide(bAnimate, bPlaySound);
	}
}

// void UUIMainMenuRootWidget::HideObjectBrowserWidget(const bool bAnimate, const bool bPlaySound) {
// 	if (bObjectBrowserWidgetVisible) {
// 		bObjectBrowserWidgetVisible = false;
// 		ObjectBrowserWidget->Hide(bAnimate, bPlaySound);
// 	}
// }

void UVRMainMenuRootWidget::HideMainMenuArtheonWidget(bool bAnimate, bool bPlaySound) {
	if (bMainMenuArtheonWidgetVisible) {
		bMainMenuArtheonWidgetVisible = false;
		MainMenuArtheonWidget->Hide(bAnimate, bPlaySound);
	}
}

void UVRMainMenuRootWidget::HideMainMenuWidget(bool bAnimate, bool bPlaySound) {
	if (bMainMenuWidgetVisible) {
		bMainMenuWidgetVisible = false;
		MainMenuWidget->Hide(bAnimate, bPlaySound);
	}
}

void UVRMainMenuRootWidget::HideAvatarBrowserWidget(bool bAnimate, bool bPlaySound) {
	if (bAvatarBrowserWidgetVisible) {
		bAvatarBrowserWidgetVisible = false;
		AvatarBrowserWidget->Hide(bAnimate, bPlaySound);
	}
	
	// if (const UUIMenuSubsystem* MenuSubsystem = GetWorld()->GetSubsystem<UUIMenuSubsystem>()) {
	// 	MenuSubsystem->DisableCustomizer();
	// } else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Menu)); }
}

void UVRMainMenuRootWidget::NavigateToSpaceBrowserScreen(const bool bAnimate, const bool bPlaySound) {
	ShowSpaceBrowserWidget(bAnimate, bPlaySound);
	HideUserBrowserWidget(bAnimate, bPlaySound);
	HideModBrowserWidget(bAnimate, bPlaySound);
	HideSettingsWidget(bAnimate, bPlaySound);
	HideServersWidget(bAnimate, bPlaySound);
	HideAboutWidget(bAnimate, bPlaySound);
	//HideObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuWidget(bAnimate, bPlaySound);
	HideMainMenuArtheonWidget(bAnimate, bPlaySound);
	HideAvatarBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
				BreadcrumbSubsystem->Push(LOCTEXT("Galleries", "Galleries"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

void UVRMainMenuRootWidget::NavigateToUserBrowserScreen(const bool bAnimate, const bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	ShowUserBrowserWidget(bAnimate, bPlaySound);
	HideModBrowserWidget(bAnimate, bPlaySound);
	HideSettingsWidget(bAnimate, bPlaySound);
	HideServersWidget(bAnimate, bPlaySound);
	HideAboutWidget(bAnimate, bPlaySound);
	//HideObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuWidget(bAnimate, bPlaySound);
	HideMainMenuArtheonWidget(bAnimate, bPlaySound);
	HideAvatarBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
				BreadcrumbSubsystem->Push(LOCTEXT("VeUI", "People"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

void UVRMainMenuRootWidget::NavigateToModBrowserScreen(const bool bAnimate, const bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	HideUserBrowserWidget(bAnimate, bPlaySound);
	ShowModBrowserWidget(bAnimate, bPlaySound);
	HideSettingsWidget(bAnimate, bPlaySound);
	HideServersWidget(bAnimate, bPlaySound);
	HideAboutWidget(bAnimate, bPlaySound);
	//HideObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuWidget(bAnimate, bPlaySound);
	HideMainMenuArtheonWidget(bAnimate, bPlaySound);
	HideAvatarBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
				BreadcrumbSubsystem->Push(LOCTEXT("VirtualWorlds", "Virtual Worlds"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

void UVRMainMenuRootWidget::NavigateToSettingsScreen(const bool bAnimate, const bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	HideUserBrowserWidget(bAnimate, bPlaySound);
	HideModBrowserWidget(bAnimate, bPlaySound);
	ShowSettingsWidget(bAnimate, bPlaySound);
	HideServersWidget(bAnimate, bPlaySound);
	HideAboutWidget(bAnimate, bPlaySound);
	//HideObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuArtheonWidget(bAnimate, bPlaySound);
	ShowMainMenuWidget(bAnimate, bPlaySound);
	HideAvatarBrowserWidget(bAnimate, bPlaySound);

	// if (FVeUIModule* UIModule = FVeUIModule::Get()) {
	// 	if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
	// 		{
	// 			BreadcrumbSubsystem->Reset();
	// 			BreadcrumbSubsystem->Push(LOCTEXT("Settings", "Settings"));
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

void UVRMainMenuRootWidget::NavigateToServerBrowserScreen(const bool bAnimate, const bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	HideUserBrowserWidget(bAnimate, bPlaySound);
	HideModBrowserWidget(bAnimate, bPlaySound);
	HideSettingsWidget(bAnimate, bPlaySound);
	// ShowServersWidget(bAnimate, bPlaySound);
	HideAboutWidget(bAnimate, bPlaySound);
	//HideObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuWidget(bAnimate, bPlaySound);
	HideMainMenuArtheonWidget(bAnimate, bPlaySound);
	HideAvatarBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
				BreadcrumbSubsystem->Push(LOCTEXT("Servers", "Servers"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

void UVRMainMenuRootWidget::NavigateToAboutScreen(const bool bAnimate, const bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	HideUserBrowserWidget(bAnimate, bPlaySound);
	HideModBrowserWidget(bAnimate, bPlaySound);
	HideSettingsWidget(bAnimate, bPlaySound);
	HideServersWidget(bAnimate, bPlaySound);
	ShowAboutWidget(bAnimate, bPlaySound);
	//HideObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuWidget(bAnimate, bPlaySound);
	HideMainMenuArtheonWidget(bAnimate, bPlaySound);
	HideAvatarBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
				BreadcrumbSubsystem->Push(LOCTEXT("About", "About"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

void UVRMainMenuRootWidget::NavigateToMainScreen(const bool bAnimate, const bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	HideUserBrowserWidget(bAnimate, bPlaySound);
	HideModBrowserWidget(bAnimate, bPlaySound);
	HideSettingsWidget(bAnimate, bPlaySound);
	HideServersWidget(bAnimate, bPlaySound);
	HideAboutWidget(bAnimate, bPlaySound);
	//HideObjectBrowserWidget(bAnimate, bPlaySound);
	ShowMainMenuWidget(bAnimate, bPlaySound);
	HideMainMenuArtheonWidget(bAnimate, bPlaySound);
	HideAvatarBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

// void UUIMainMenuRootWidget::NavigateToObjectBrowserScreen(const bool bAnimate, const bool bPlaySound) {
// 	HideSpaceBrowserWidget(bAnimate, bPlaySound);
// 	HideUserBrowserWidget(bAnimate, bPlaySound);
// 	HideModBrowserWidget(bAnimate, bPlaySound);
// 	HideSettingsWidget(bAnimate, bPlaySound);
// 	HideServersWidget(bAnimate, bPlaySound);
// 	HideAboutWidget(bAnimate, bPlaySound);
// 	ShowObjectBrowserWidget(bAnimate, bPlaySound);
// 	HideMainMenuWidget(bAnimate, bPlaySound);
// 	HideMainMenuArtheonWidget(bAnimate, bPlaySound);
//
// 	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
// 		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
// 			{
// 				BreadcrumbSubsystem->Reset();
// 				BreadcrumbSubsystem->Push(LOCTEXT("Objects", "Objects"));
// 			}
// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
// }

void UVRMainMenuRootWidget::NavigateToMainMenuArtheonWidget(const bool bAnimate, const bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	HideUserBrowserWidget(bAnimate, bPlaySound);
	HideModBrowserWidget(bAnimate, bPlaySound);
	HideSettingsWidget(bAnimate, bPlaySound);
	HideServersWidget(bAnimate, bPlaySound);
	HideAboutWidget(bAnimate, bPlaySound);
	//HideObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuWidget(bAnimate, bPlaySound);
	ShowMainMenuArtheonWidget(bAnimate, bPlaySound);
	HideAvatarBrowserWidget(bAnimate, bPlaySound);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
				BreadcrumbSubsystem->Push(LOCTEXT("Artheon", "Artheon"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

void UVRMainMenuRootWidget::NavigateToAvatarBrowserWidget(bool bAnimate, bool bPlaySound) {
	HideSpaceBrowserWidget(bAnimate, bPlaySound);
	HideUserBrowserWidget(bAnimate, bPlaySound);
	HideModBrowserWidget(bAnimate, bPlaySound);
	HideSettingsWidget(bAnimate, bPlaySound);
	HideServersWidget(bAnimate, bPlaySound);
	HideAboutWidget(bAnimate, bPlaySound);
	//HideObjectBrowserWidget(bAnimate, bPlaySound);
	HideMainMenuWidget(bAnimate, bPlaySound);
	HideMainMenuArtheonWidget(bAnimate, bPlaySound);
	ShowAvatarBrowserWidget(bAnimate, bPlaySound);


	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Reset();
				BreadcrumbSubsystem->Push(LOCTEXT("Persona", "Persona"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
}

#undef LOCTEXT_NAMESPACE
