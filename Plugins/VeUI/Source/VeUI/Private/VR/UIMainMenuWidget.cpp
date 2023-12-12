// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIMainMenuWidget.h"

#include "VeHUD.h"
#include "UIPlayerController.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UUIMainMenuWidget::RegisterCallbacks() {
	if (const APlayerController* PC = GetOwningPlayer()) {
		if (const UWorld* World = PC->GetWorld()) {
			if (World->GetNetMode() == NM_Standalone) {
				AvatarButtonWidget->SetVisibility(ESlateVisibility::Visible);
			} else {
				AvatarButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

	if (!OnHomeButtonClickedDelegateHandle.IsValid()) {
		OnHomeButtonClickedDelegateHandle = HomeButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
			const auto PlayerController = GetOwningPlayer<AUIPlayerController>();
			if (PlayerController) {
				PlayerController->OpenLevel(*FVeConfigLibrary::GetStartupMap(), true);
			}
		});
	}

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
	if (!OnModsButtonClickedDelegateHandle.IsValid()) {
		OnModsButtonClickedDelegateHandle = ModsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnModsButtonClicked.IsBound()) {
				OnModsButtonClicked.Broadcast();
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

	// Re-translate the event.
	if (!OnUsersButtonClickedDelegateHandle.IsValid()) {
		OnUsersButtonClickedDelegateHandle = UsersButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnUsersButtonClicked.IsBound()) {
				OnUsersButtonClicked.Broadcast();
			}
		});
	}

	// Re-translate the event.
	if (!OnSettingsButtonClickedDelegateHandle.IsValid()) {
		OnSettingsButtonClickedDelegateHandle = SettingsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnSettingsButtonClicked.IsBound()) {
				OnSettingsButtonClicked.Broadcast();
			}
		});
	}

	// Re-translate the event.
	if (!OnServersButtonClickedDelegateHandle.IsValid()) {
		OnServersButtonClickedDelegateHandle = ServersButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnServersButtonClicked.IsBound()) {
				OnServersButtonClicked.Broadcast();
			}
		});
	}

	// Re-translate the event.
	if (!OnAboutButtonClickedDelegateHandle.IsValid()) {
		OnAboutButtonClickedDelegateHandle = AboutButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnAboutButtonClicked.IsBound()) {
				OnAboutButtonClicked.Broadcast();
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
			if (OnArteonDLCButtonClicked.IsBound()) {
				OnArteonDLCButtonClicked.Broadcast();
			}
		});
	}

	if (!OnAvatarButtonClickedDelegateHandle.IsValid()) {
		OnAvatarButtonClickedDelegateHandle = AvatarButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (OnAvatarButtonClicked.IsBound()) {
				OnAvatarButtonClicked.Broadcast();
			}

			if (AvatarButtonClicked.IsBound()) {
				AvatarButtonClicked.Broadcast();
			}
		});
	}
}

void UUIMainMenuWidget::UnregisterCallbacks() {
	if (OnHomeButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(HomeButtonWidget)) {
			HomeButtonWidget->GetOnButtonClicked().Remove(OnHomeButtonClickedDelegateHandle);
			OnHomeButtonClickedDelegateHandle.Reset();
		}
	}

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

	if (OnModsButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(ModsButtonWidget)) {
			ModsButtonWidget->GetOnButtonClicked().Remove(OnModsButtonClickedDelegateHandle);
			OnModsButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnSpacesButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(SpacesButtonWidget)) {
			SpacesButtonWidget->GetOnButtonClicked().Remove(OnSpacesButtonClickedDelegateHandle);
			OnSpacesButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnUsersButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(UsersButtonWidget)) {
			UsersButtonWidget->GetOnButtonClicked().Remove(OnUsersButtonClickedDelegateHandle);
			OnUsersButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnSettingsButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(SettingsButtonWidget)) {
			SettingsButtonWidget->GetOnButtonClicked().Remove(OnSettingsButtonClickedDelegateHandle);
			OnSettingsButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnServersButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(ServersButtonWidget)) {
			ServersButtonWidget->GetOnButtonClicked().Remove(OnServersButtonClickedDelegateHandle);
			OnServersButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnAboutButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(AboutButtonWidget)) {
			AboutButtonWidget->GetOnButtonClicked().Remove(OnAboutButtonClickedDelegateHandle);
			OnAboutButtonClickedDelegateHandle.Reset();
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

	if (OnAvatarButtonClickedDelegateHandle.IsValid()) {
		if (AvatarButtonWidget) {
			AvatarButtonWidget->GetOnButtonClicked().Remove(OnAvatarButtonClickedDelegateHandle);
			OnAvatarButtonClickedDelegateHandle.Reset();
		}
	}
}
