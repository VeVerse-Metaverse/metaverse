// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIMainMenuArtheonWidget.h"

#include "VeHUD.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"


void UUIMainMenuArtheonWidget::RegisterCallbacks() {
	if (!OnLogoutButtonClickedDelegateHandle.IsValid()) {
		OnLogoutButtonClickedDelegateHandle = LogoutButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (FVeApi2Module* ApiModule = FVeApi2Module::Get()) {
				if (const TSharedPtr<FApi2AuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
					{
						AuthSubsystem->Logout();
					}
				} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
			} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
		});
	}

	if (!OnExitButtonClickedDelegateHandle.IsValid()) {
		OnExitButtonClickedDelegateHandle = ExitButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (AVeHUD* HUD = GetHUD()) {
				HUD->RequestExitConfirmation();
			}
		});
	}


	// Re-translate the event.
	if (!OnSpacesButtonClickedDelegateHandle.IsValid()) {
		OnSpacesButtonClickedDelegateHandle = SpacesButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnSpacesButtonClicked.IsBound()) {
				OnSpacesButtonClicked.Broadcast();
			}
		});
	}


	if (!OnReturnMainMenuClickedDelegateHandle.IsValid()) {
		OnReturnMainMenuClickedDelegateHandle = HomeButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnReturnMainMenu.IsBound()) {
				OnReturnMainMenu.Broadcast();
			}
		});
	}

	if (!OnObjectsButtonClickedDelegateHandle.IsValid()) {
		OnObjectsButtonClickedDelegateHandle = ObjectsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnObjectsButtonClicked.IsBound()) {
				OnObjectsButtonClicked.Broadcast();
			}
		});
	}
	
	if (!OnOpenSeaNFTsButtonClickedDelegateHandle.IsValid()) {
		OnOpenSeaNFTsButtonClickedDelegateHandle = OpenSeaNFTsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnOpenSeaNFTsButtonClicked.IsBound()) {
				OnOpenSeaNFTsButtonClicked.Broadcast();
			}
		});
	}

	if (!OnBackToLe7elButtonClickedDelegateHandle.IsValid()) {
		OnBackToLe7elButtonClickedDelegateHandle = BackToLe7elButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnBackToLe7elButtonClicked.IsBound()) {
				OnBackToLe7elButtonClicked.Broadcast();
			}
		});
	}
}

void UUIMainMenuArtheonWidget::UnregisterCallbacks() {
	if (OnLogoutButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(LogoutButtonWidget)) {
			LogoutButtonWidget->GetOnButtonClicked().Remove(OnLogoutButtonClickedDelegateHandle);
			OnLogoutButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnExitButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(ExitButtonWidget)) {
			ExitButtonWidget->GetOnButtonClicked().Remove(OnExitButtonClickedDelegateHandle);
			OnExitButtonClickedDelegateHandle.Reset();
		}
	}


	if (OnSpacesButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(SpacesButtonWidget)) {
			SpacesButtonWidget->GetOnButtonClicked().Remove(OnSpacesButtonClickedDelegateHandle);
			OnSpacesButtonClickedDelegateHandle.Reset();
		}
	}
	
	if (OnReturnMainMenuClickedDelegateHandle.IsValid()) {
		if (HomeButtonWidget) {
			HomeButtonWidget->GetOnButtonClicked().Remove(OnReturnMainMenuClickedDelegateHandle);
			OnReturnMainMenuClickedDelegateHandle.Reset();
		}
	}

	if (OnObjectsButtonClickedDelegateHandle.IsValid()) {
		if (ObjectsButtonWidget) {
			ObjectsButtonWidget->GetOnButtonClicked().Remove(OnObjectsButtonClickedDelegateHandle);
			OnObjectsButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnOpenSeaNFTsButtonClickedDelegateHandle.IsValid()) {
		if (OpenSeaNFTsButtonWidget) {
			OpenSeaNFTsButtonWidget->GetOnButtonClicked().Remove(OnOpenSeaNFTsButtonClickedDelegateHandle);
			OnOpenSeaNFTsButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnBackToLe7elButtonClickedDelegateHandle.IsValid()) {
		if (BackToLe7elButtonWidget) {
			BackToLe7elButtonWidget->GetOnButtonClicked().Remove(OnBackToLe7elButtonClickedDelegateHandle);
			OnBackToLe7elButtonClickedDelegateHandle.Reset();
		}
	}
}
