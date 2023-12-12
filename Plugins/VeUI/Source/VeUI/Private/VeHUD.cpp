// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeHUD.h"

#include "Subsystems/UIDialogSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"
#include "UIHUDWidget.h"
#include "UIRootWidget.h"
#include "Editor/UIEditorRootWidget.h"
#include "Inspector/UIInspectorRootWidget.h"
#include "UObject/ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "VeUI"

AVeHUD::AVeHUD()
	: Super() {

#if ! UE_SERVER
	{
		static ConstructorHelpers::FClassFinder<UUIRootWidget> Widget(TEXT("/VeUI/UI/WBP_UIRootWidget"));
		RootWidgetClass = Widget.Class;
	}

	{
		static ConstructorHelpers::FClassFinder<UUIRootWidget> Widget(TEXT("/VeUI/Mobile/WBP_MobileUIRootWidget"));
		MobileRootWidgetClass = Widget.Class;
	}

	{
		static ConstructorHelpers::FClassFinder<UUserWidget> Widget(TEXT("/VeUI/UI/WBP_UIHUDWidget"));
		DefaultHUDWidgetClass = Widget.Class;
	}
#endif
}

void AVeHUD::PostInitializeComponents() {
	Super::PostInitializeComponents();

	const auto* World = GetWorld();
	if (World && World->IsGameWorld()) {
		if (APlayerController* PlayerController = GetOwningPlayerController()) {
			// RootWidget
			{
				TSubclassOf<UUIRootWidget> RootWidgetClassToUse = RootWidgetClass;
#if WITH_EDITOR
				if (World && World->IsPlayInMobilePreview()) {
					RootWidgetClassToUse = MobileRootWidgetClass;
				}
#else
				if (FVeEnvironmentSubsystem::IsMobilePlatform()) {
					RootWidgetClassToUse = MobileRootWidgetClass;
				}
#endif

				if (IsValid(RootWidgetClassToUse)) {
					RootWidget = CreateWidget<UUIRootWidget>(GetOwningPlayerController(), RootWidgetClassToUse, TEXT("UIRoot"));
					if (RootWidget) {
						RootWidget->OnHideMenu.BindUObject(this, &AVeHUD::RootWidget_OnHideMenu);
						RootWidget->AddToViewport();
					} else {
						LogErrorF("failed to create a root widget for HUD");
					}
				}
			}

			//
			// ObjectPreviewWidget = CreateWidget<UUIObjectDetailWidget>(PlayerController, RootWidgetClass, TEXT("UIObjecy"));
			// if (ObjectPreviewWidget) {
			// 	ObjectPreviewWidget->AddToViewport();
			// } else {
			// 	LogErrorF("failed to create a object widget for HUD");
			// }
		}
	}
}

void AVeHUD::BeginPlay() {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
			FUINotificationData NotificationData;
			NotificationData.Lifetime = DefaultNotificationLifetime;

			if (AuthorizationState == EAuthorizationState::LoginSuccess) {
				NotificationData.Header = LOCTEXT("LoginSuccessfulNotificationHeader", "Login successful");
				NotificationData.Message = FText::Format(LOCTEXT("LoginSuccessfulNotificationMessage", "Welcome,\n{0}"), FText::FromString(AuthSubsystem->GetUserMetadata().Name));
				NotificationData.Type = EUINotificationType::Success;
				AddNotification(NotificationData);
				return;
			}

			if (AuthorizationState == EAuthorizationState::LoginFail) {
				NotificationData.Header = LOCTEXT("LoginFailedNotificationHeader", "Login failed");
				NotificationData.Message = FText::Format(LOCTEXT("LoginFailedNotificationMessage", "{0}"), FText::FromString(AuthSubsystem->GetAuthorizationMessage()));
				NotificationData.Type = EUINotificationType::Failure;
				AddNotification(NotificationData);
				return;
			}
		};

		AuthSubsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
		OnChangeAuthorizationStateCallback(AuthSubsystem->GetAuthorizationState());
	}
}

void AVeHUD::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnChangeAuthorizationState().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

UUIDialogWidget* AVeHUD::ShowDialog(const FUIDialogData& DialogData, const FUIDialogButtonClicked& InOnButtonClicked) const {
	GET_MODULE_SUBSYSTEM(DialogSubsystem, UI, Dialog);
	if (DialogSubsystem) {
		return DialogSubsystem->ShowDialog(DialogData, InOnButtonClicked);
	}
	return nullptr;
}

void AVeHUD::AddNotification(const FUINotificationData& NotificationData) const {
	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		NotificationSubsystem->AddNotification(NotificationData);

		if (auto* HUDWidget = Cast<UUIHUDWidget>(CurrentHUDWidget)) {
			HUDWidget->ShowNotification();
		}
	}
}

void AVeHUD::SetVoiceState(const bool bInVoiceActive) const {
	if (auto* HUDWidget = Cast<UUIHUDWidget>(CurrentHUDWidget)) {
		if (bInVoiceActive) {
			HUDWidget->ShowVoiceIndicator();
		} else {
			HUDWidget->HideVoiceIndicator();
		}
	}
}

void AVeHUD::RequestExitConfirmation() {
	GET_MODULE_SUBSYSTEM(DialogSubsystem, UI, Dialog);
	if (DialogSubsystem) {
		DialogSubsystem->RequestExitConfirmation(ExitDelay);
	}
}

UUIEditorRootWidget* AVeHUD::GetEditorRootWidget() const {
	if (IsValid(RootWidget.Get())) {
		return RootWidget->GetEditorRootWidget();
	}
	return nullptr;
}

void AVeHUD::ShowEditor() {
	if (IsValid(RootWidget.Get())) {
		RootWidget->ShowEditor();
	}
}

void AVeHUD::HideEditor() {
	if (IsValid(RootWidget.Get())) {
		RootWidget->HideEditor();
	}
}

UUIInspectorRootWidget* AVeHUD::GetInspectorRootWidget() const {
	if (IsValid(RootWidget.Get())) {
		return RootWidget->GetInspectorRootWidget();
	}
	return nullptr;
}

void AVeHUD::ShowInspector() {
	if (IsValid(RootWidget.Get())) {
		RootWidget->ShowInspector();
	}
}

void AVeHUD::HideInspector() {
	if (IsValid(RootWidget.Get())) {
		RootWidget->HideInspector();
	}
}

bool AVeHUD::ShowMenu() {
	if (IsValid(RootWidget.Get())) {
		if (CurrentHUDWidget.IsValid()) {
			CurrentHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		RootWidget->ShowMenu();
		return true;
	}
	return false;
}

bool AVeHUD::HideMenu() {
	if (IsValid(RootWidget.Get())) {
		RootWidget->HideMenu();
		if (CurrentHUDWidget.IsValid()) {
			CurrentHUDWidget->SetVisibility(ESlateVisibility::Visible);
		}
		return true;
	}
	return false;
}

void AVeHUD::RootWidget_OnHideMenu() {
	HideMenu();
}

UUserWidget* AVeHUD::ShowHUDWidget(TSubclassOf<UUserWidget> WidgetClass) {
	if (CurrentHUDWidget.IsValid()) {
		CurrentHUDWidget->RemoveFromParent();
		CurrentHUDWidget = nullptr;
	}

	if (GetWorld() && GetWorld()->bIsTearingDown) {
		return nullptr;
	}

	if (!WidgetClass) {
		WidgetClass = DefaultHUDWidgetClass;
		if (!WidgetClass) {
			return nullptr;
		}
	}

	if (auto* PlayerController = GetOwningPlayerController()) {
		CurrentHUDWidget = CreateWidget<UUserWidget>(PlayerController, WidgetClass);
		if (CurrentHUDWidget.IsValid()) {
			CurrentHUDWidget->AddToViewport();
			return CurrentHUDWidget.Get();
		} else {
			LogErrorF("failed to create a hud widget");
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
