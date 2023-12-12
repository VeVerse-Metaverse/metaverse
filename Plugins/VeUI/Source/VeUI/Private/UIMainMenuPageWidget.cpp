// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIMainMenuPageWidget.h"

#include "UILog.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "UIPlayerController.h"
#include "Components/UIButtonWidget.h"
#include "UIPageManagerWidget.h"
#include "VeHUD.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIMainMenuPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (HomeButtonWidget) {
		HomeButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
			if (const auto PlayerController = GetOwningPlayer<AUIPlayerController>()) {
				PlayerController->OpenLevel(*FVeConfigLibrary::GetStartupMap(), true);
			}
		});
	}

	if (LogoutButtonWidget) {
		LogoutButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
			if (AuthSubsystem) {
				AuthSubsystem->Logout();
			}
		});
	}

	if (UsersButtonWidget) {
		UsersButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(UsersPageClass) && IsValid(GetPageManager())) {
				GetPageManager()->OpenPage(UsersPageClass);
			}
		});
	}

	if (ServersButtonWidget) {
		ServersButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(ServersPageClass) && IsValid(GetPageManager())) {
				GetPageManager()->OpenPage(ServersPageClass);
			}
		});
	}

	if (PackagesButtonWidget) {
		PackagesButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(PackagesPageClass) && IsValid(GetPageManager())) {
				GetPageManager()->OpenPage(PackagesPageClass);
			}
		});
	}

	if (WorldsButtonWidget) {
		WorldsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(WorldsPageClass) && IsValid(GetPageManager())) {
				GetPageManager()->OpenPage(WorldsPageClass);
			}
		});
	}

	if (EventsButtonWidget) {
		EventsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(EventsPageClass) && IsValid(GetPageManager())) {
				GetPageManager()->OpenPage(EventsPageClass);
			}
		});
	}

	if (ArtheonMenuButtonWidget) {
		ArtheonMenuButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(ArtheonMenuPageClass) && IsValid(GetPageManager())) {
				GetPageManager()->OpenPage(ArtheonMenuPageClass);
			}
		});
	}

	if (ObjectsButtonWidget) {
		ObjectsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(ObjectsPageClass) && GetPageManager()) {
				GetPageManager()->OpenPage(ObjectsPageClass);
			}
		});
	}

	if (OpenSeaNftButtonWidget) {
		OpenSeaNftButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(OpenSeaNftPageClass) && GetPageManager()) {
				GetPageManager()->OpenPage(OpenSeaNftPageClass);
			}
		});
	}

	if (AvatarButtonWidget) {
		AvatarButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(AvatarPageClass) && IsValid(GetPageManager())) {
				GetPageManager()->OpenPage(AvatarPageClass);
			}
		});
	}

	if (SettingsButtonWidget) {
		SettingsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(SettingsPageClass) && IsValid(GetPageManager())) {
				GetPageManager()->OpenPage(SettingsPageClass);
			}
		});
	}

	if (AboutButtonWidget) {
		AboutButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(AboutPageClass) && IsValid(GetPageManager())) {
				GetPageManager()->OpenPage(AboutPageClass);
			}
		});
	}

	if (ExitButtonWidget) {
		ExitButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (auto* HUD = GetHUD()) {
				HUD->RequestExitConfirmation();
			}
		});
	}
}

#undef LOCTEXT_NAMESPACE
