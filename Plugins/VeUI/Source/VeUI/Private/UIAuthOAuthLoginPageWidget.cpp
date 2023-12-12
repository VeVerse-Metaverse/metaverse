// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIAuthOAuthLoginPageWidget.h"

#include "UIPageManagerWidget.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIFormInputWidget.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Subsystems/UINotificationSubsystem.h"
#if !WITH_EDITORONLY_DATA
#include "Animation/UMGSequencePlayer.h"
#include "VeUI.h"
#include "Subsystems/UIDialogSubsystem.h"
#endif
#include "VeOAuth.h"
#include "OAuthOAuthSubsystem.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIAuthOAuthLoginPageWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	if (OpenButtonWidget) {
		OpenButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			const auto ExternalProcessingCallback = MakeShared<FOnOAuthCompleted>();
			ExternalProcessingCallback->BindLambda([=](const FString& Token, const bool bInSuccessful, const FString& InError) {
				if (InError.Contains("failed to connect")) {
					GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
					if (NotificationSubsystem) {
						FUINotificationData NotificationData;
						NotificationData.Type = EUINotificationType::Failure;
						NotificationData.Header = LOCTEXT("OAuthErrorHeader", "OAuth Error");
						NotificationData.Message = FText::Format(LOCTEXT("OAuthErrorMessage", "Failed to login with {0}. Please check your firewall settings (oauth-helper.exe) or internet connection."), FText::FromString(Provider));
						NotificationData.Lifetime = 15.0f;
						NotificationSubsystem->AddNotification(NotificationData);
					}
				}

				if (!bInSuccessful) {
					LogErrorF("failed to login with %s", *Provider);
				}
			});

			GET_MODULE_SUBSYSTEM(OAuthSubsystem, OAuth, OAuth);
			if (OAuthSubsystem) {
				OAuthSubsystem->Login(Provider, ExternalProcessingCallback);
			}
		});
	}

	if (LoginOptionsButtonWidget) {
		LoginOptionsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GetPageManager()->ShowFirstPage();
		});
	}

#if WITH_EDITORONLY_DATA
	QuitButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
#else
	if (QuitButtonWidget) {
		QuitButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GET_MODULE_SUBSYSTEM(DialogSubsystem, UI, Dialog);
			if (DialogSubsystem) {
				DialogSubsystem->RequestExitConfirmation(ExitDelay);
			}
		});
	}
#endif

}

void UUIAuthOAuthLoginPageWidget::NativeConstruct() {
	Super::NativeConstruct();

	// GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	// if (AuthSubsystem) {
	// 	auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
	// 		NativeOnLoginEnd();
	//
	// 		if (AuthorizationState == EAuthorizationState::LoginFail) {
	// 			SetMessageText(FText::FromString(AuthSubsystem->GetAuthorizationMessage()));
	// 			return;
	// 		}
	//
	// 		if (AuthorizationState == EAuthorizationState::LoginSuccess) {
	// 			if (LoginSuccessSound) {
	// 				PlaySound(LoginSuccessSound);
	// 			}
	// 		}
	//
	// 		ResetState();
	// 	};
	//
	// 	AuthSubsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
	// }
}

void UUIAuthOAuthLoginPageWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnChangeAuthorizationState().RemoveAll(this);
	}
}

void UUIAuthOAuthLoginPageWidget::SetHeaderText(const FText& InText) {
	HeaderText->SetText(InText);
}

void UUIAuthOAuthLoginPageWidget::SetProvider(const FString& InProvider) {
	Provider = InProvider;
}

#undef LOCTEXT_NAMESPACE
