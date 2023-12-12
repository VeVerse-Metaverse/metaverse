// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/VRRootWidget.h"

#include "ApiAuthSubsystem.h"
#include "VR/UIObjectDetailWidget.h"
#include "Subsystems/UIDialogSubsystem.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "VR/UIAuthMenuRootWidget.h"
#include "VR/UINotificationRootWidget.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "VeUI.h"
#include "VR/VRMainMenuRootWidget.h"
#include "GameFramework/InputSettings.h"


UVRRootWidget::UVRRootWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer){
	bIsFocusable = true;

	bAuthRootWidgetVisible = false;
	bMainMenuRootWidgetVisible = false;
}

void UVRRootWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
	
	// Set EscapeActionList
	{
		const UInputSettings* InputSettings = UInputSettings::StaticClass()->GetDefaultObject<UInputSettings>();
		for (const FInputActionKeyMapping& ActionMapping : InputSettings->GetActionMappings()) {
			if (ActionMapping.ActionName == EscapeInputAction) {
				FInputActionKeyMapping Item(ActionMapping);
				Item.ActionName = NAME_None;
				EscapeActionMappings.Add(Item);
			}
		}
	}
	
	if (MainMenuRootWidget) {
		MainMenuRootWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	ShowAuthorizationDialog();

	// Deprecated
#if 0
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		if (!AuthSubsystem->GetOnUserLogin().IsBoundToObject(this)) {
			AuthSubsystem->GetOnUserLogin().AddWeakLambda(this, [this](const FApiUserMetadata&, const bool bSuccessful, const FString&) {
				if (bSuccessful) {
					// PUBLIC_NavigateToMainMenuScreen();
					HideAuthorizationDialog();
				}
			});
		}
		if (!AuthSubsystem->GetOnUserLogout().IsBoundToObject(this)) {
			AuthSubsystem->GetOnUserLogout().AddWeakLambda(this, [this]() {
				// PUBLIC_NavigateToAuthMenuScreen();
				ShowAuthorizationDialog();
			});
		}

		if (AuthSubsystem->IsLoggedIn()) {
			HideAuthorizationDialog();
		// } else {
		// 	NEW_ShowAuthorizationDialog();
		}
	}
#endif
	
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIDialogSubsystem> DialogSubsystem = UIModule->GetDialogSubsystem()) {
			DialogSubsystem->SetRootWidget(this);
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Dialog)); }

		if (const TSharedPtr<FUINotificationSubsystem> NotificationSubsystem = UIModule->GetNotificationSubsystem()) {
			NotificationSubsystem->SetRootWidget(this);
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Notification)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
}

UUIDialogWidget* UVRRootWidget::ShowDialog(const FUIDialogData& InDialogData, const FUIDialogButtonClicked& InOnButtonClicked) const {
	if (DialogRootWidget) {
		return DialogRootWidget->ShowDialog(InDialogData, InOnButtonClicked);
	}

	return nullptr;
}

void UVRRootWidget::AddNotification(const FUINotificationData& InNotificationData) {
	if (NotificationRootWidget) {
		NotificationRootWidget->AddNotification(InNotificationData);
	}
}

void UVRRootWidget::ShowAuthorizationDialog() {
	if (!bAuthRootWidgetVisible) {
		HideMenu();
		if (AuthRootWidget) {
			AuthRootWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		if (BackgroundWidget) {
			BackgroundWidget->SetVisibility(ESlateVisibility::Visible);
		}
		SetInputModeUIOnly();
	}

	bAuthRootWidgetVisible = true;
}

void UVRRootWidget::HideAuthorizationDialog() {
	if (bAuthRootWidgetVisible) {
		if (AuthRootWidget) {
			AuthRootWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		if (BackgroundWidget) {
			BackgroundWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		SetInputModeGame();
	}

	bAuthRootWidgetVisible = false;
}

void UVRRootWidget::ToggleMenu() {
	if (bMainMenuRootWidgetVisible) {
		HideMenu();
	} else {
		ShowMenu();
	}
}

void UVRRootWidget::ShowMenu() {
	if (bAuthRootWidgetVisible) {
		return;
	}

	if (!bMainMenuRootWidgetVisible) {
		if (MainMenuRootWidget) {
			MainMenuRootWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		if (BackgroundWidget) {
			BackgroundWidget->SetVisibility(ESlateVisibility::Visible);
		}
		SetInputModeUIOnly();
	}

	bMainMenuRootWidgetVisible = true;
}

void UVRRootWidget::HideMenu() {
	if (bAuthRootWidgetVisible) {
		return;
	}
	
	if (bMainMenuRootWidgetVisible) {
		if (MainMenuRootWidget) {
			MainMenuRootWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		if (BackgroundWidget) {
			BackgroundWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		SetInputModeGame();
	}

	bMainMenuRootWidgetVisible = false;
}

void UVRRootWidget::SetInputModeUIOnly() {
	auto* PlayerController = GetOwningPlayer();
	if (PlayerController != nullptr) {
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);

		// if (InWidgetToFocus != nullptr) {
		// 	InputMode.SetWidgetToFocus(InWidgetToFocus->TakeWidget());
		// }
		InputMode.SetWidgetToFocus(TakeWidget());
		
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
}

void UVRRootWidget::SetInputModeGame() {
	auto* PlayerController = GetOwningPlayer();
	if (PlayerController != nullptr) {
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
	}
}

FReply UVRRootWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) {
	FInputActionKeyMapping InActionMapping(
		NAME_None,
		InKeyEvent.GetKey(),
		InKeyEvent.IsShiftDown(),
		InKeyEvent.IsControlDown(),
		InKeyEvent.IsAltDown(),
		InKeyEvent.IsCommandDown()
		);

	for (const FInputActionKeyMapping& ActionMapping : EscapeActionMappings) {
		if (InActionMapping == ActionMapping) {
			Escape();
			return FReply::Handled();
		}
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UVRRootWidget::Escape() {
	HideMenu();
}
