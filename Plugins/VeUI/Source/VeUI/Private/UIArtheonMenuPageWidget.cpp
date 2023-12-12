// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIArtheonMenuPageWidget.h"

#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Components/UIButtonWidget.h"
#include "UIPageManagerWidget.h"
#include "VeHUD.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIArtheonMenuPageWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	// if (!OnHomeButtonClickedDelegateHandle.IsValid()) {
	// 	OnHomeButtonClickedDelegateHandle = HomeButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
	// 		if (const auto PlayerController = GetOwningPlayer<AUIPlayerController>()) {
	// 			PlayerController->OpenLevel(TEXT("Home"), true);
	// 		}
	// 	});
	// }

	if (!OnLogoutButtonClickedDelegateHandle.IsValid()) {
		OnLogoutButtonClickedDelegateHandle = LogoutButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
			if (AuthSubsystem) {
				AuthSubsystem->Logout();
			}
		});
	}

	if (!OnObjectsButtonClickedDelegateHandle.IsValid()) {
		OnObjectsButtonClickedDelegateHandle = ObjectsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(ObjectsPageClass) && GetPageManager()) {
				GetPageManager()->OpenPage(ObjectsPageClass);
			}
		});
	}

	if (!OnOpenSeaNftButtonClickedDelegateHandle.IsValid()) {
		OnOpenSeaNftButtonClickedDelegateHandle = OpenSeaNftButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(OpenSeaNftPageClass) && GetPageManager()) {
				GetPageManager()->OpenPage(OpenSeaNftPageClass);
			}
		});
	}

	if (!OnMainMenuButtonClickedDelegateHandle.IsValid()) {
		OnMainMenuButtonClickedDelegateHandle = MainMenuButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(MainMenuPageClass) && GetPageManager()) {
				GetPageManager()->OpenPage(MainMenuPageClass);
			}
		});
	}

	if (!OnExitButtonClickedDelegateHandle.IsValid()) {
		OnExitButtonClickedDelegateHandle = ExitButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (auto* HUD = GetHUD()) {
				HUD->RequestExitConfirmation();
			}
		});
	}
}

void UUIArtheonMenuPageWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	// if (OnHomeButtonClickedDelegateHandle.IsValid()) {
	// 	if (IsValid(HomeButtonWidget)) {
	// 		HomeButtonWidget->GetOnButtonClicked().Remove(OnHomeButtonClickedDelegateHandle);
	// 	}
	// 	OnHomeButtonClickedDelegateHandle.Reset();
	// }

	if (OnLogoutButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(LogoutButtonWidget)) {
			LogoutButtonWidget->GetOnButtonClicked().Remove(OnLogoutButtonClickedDelegateHandle);
		}
		OnLogoutButtonClickedDelegateHandle.Reset();
	}

	if (OnObjectsButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(ObjectsButtonWidget)) {
			ObjectsButtonWidget->GetOnButtonClicked().Remove(OnObjectsButtonClickedDelegateHandle);
		}
		OnObjectsButtonClickedDelegateHandle.Reset();
	}

	if (OnOpenSeaNftButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(OpenSeaNftButtonWidget)) {
			OpenSeaNftButtonWidget->GetOnButtonClicked().Remove(OnOpenSeaNftButtonClickedDelegateHandle);
		}
		OnOpenSeaNftButtonClickedDelegateHandle.Reset();
	}

	if (OnMainMenuButtonClickedDelegateHandle.IsValid()) {
		if (MainMenuButtonWidget) {
			MainMenuButtonWidget->GetOnButtonClicked().Remove(OnMainMenuButtonClickedDelegateHandle);
		}
		OnMainMenuButtonClickedDelegateHandle.Reset();
	}

	if (OnExitButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(ExitButtonWidget)) {
			ExitButtonWidget->GetOnButtonClicked().Remove(OnExitButtonClickedDelegateHandle);
		}
		OnExitButtonClickedDelegateHandle.Reset();
	}
}


#undef LOCTEXT_NAMESPACE
