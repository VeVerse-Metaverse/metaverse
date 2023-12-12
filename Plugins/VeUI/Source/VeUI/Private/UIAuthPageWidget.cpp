// Author: Khusan Yadgarov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIAuthPageWidget.h"

#include "UIPageManagerWidget.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIFormInputWidget.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "OAuthOAuthSubsystem.h"
#include "VeOAuth.h"
#include "VeShared.h"
#include "Components/UIButtonWidget.h"
#include "Animation/UMGSequencePlayer.h"
#include "VeUI.h"
#include "Subsystems/UIDialogSubsystem.h"
#include "UIAuthOAuthLoginPageWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"

constexpr static auto ProviderGoogle = "google";
constexpr static auto ProviderLE7EL = "le7el";
constexpr static auto ProviderDiscord = "discord";
constexpr static auto ProviderEpic = "eos";

void UUIAuthPageWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	ReadOAuthProvidersConfig();
	ShowOAuthButtonConfig();
	
	if (GoogleLoginButtonWidget) {
		GoogleLoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			auto* Page = GetPageManager()->OpenPage<UUIAuthOAuthLoginPageWidget>(OAuthPageClass);
			Page->SetHeaderText(FText::FromString(TEXT("Google")));
			Page->SetProvider(ProviderGoogle);
		});
	}

	if (LevelLoginButtonWidget) {
		LevelLoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			auto* Page = GetPageManager()->OpenPage<UUIAuthOAuthLoginPageWidget>(OAuthPageClass);
			Page->SetHeaderText(FText::FromString(TEXT("LE7EL")));
			Page->SetProvider(ProviderLE7EL);
		});
	}

	if (DiscordLoginButtonWidget) {
		DiscordLoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			auto* Page = GetPageManager()->OpenPage<UUIAuthOAuthLoginPageWidget>(OAuthPageClass);
			Page->SetHeaderText(FText::FromString(TEXT("Discord")));
			Page->SetProvider(ProviderDiscord);
		});
	}

	if (EpicLoginButtonWidget) {
		EpicLoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			auto* Page = GetPageManager()->OpenPage<UUIAuthOAuthLoginPageWidget>(OAuthPageClass);
			Page->SetHeaderText(FText::FromString(TEXT("Epic Games")));
			Page->SetProvider(ProviderEpic);
		});
	}

	if (EmailLoginButtonWidget) {
		EmailLoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GetPageManager()->OpenPage(AuthEmailPageClass);
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

void UUIAuthPageWidget::ReadOAuthProvidersConfig() {
	GConfig->GetBool(TEXT("OAuthProviders"), TEXT("Google"), bIsGoogleSupported, FPaths::ProjectConfigDir().Append("DefaultApp.ini"));
	GConfig->GetBool(TEXT("OAuthProviders"), TEXT("Discord"), bIsDiscordSupported, FPaths::ProjectConfigDir().Append("DefaultApp.ini"));
	GConfig->GetBool(TEXT("OAuthProviders"), TEXT("Le7el"), bIsLe7elSupported, FPaths::ProjectConfigDir().Append("DefaultApp.ini"));
	GConfig->GetBool(TEXT("OAuthProviders"), TEXT("Eos"), bIsEosSupported, FPaths::ProjectConfigDir().Append("DefaultApp.ini"));
}

void UUIAuthPageWidget::ShowOAuthButtonConfig() {
	if (GoogleLoginButtonWidget) {
		GoogleLoginButtonWidget->SetVisibility(bIsGoogleSupported ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (LevelLoginButtonWidget) {
		LevelLoginButtonWidget->SetVisibility(bIsLe7elSupported ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (DiscordLoginButtonWidget) {
		DiscordLoginButtonWidget->SetVisibility(bIsDiscordSupported ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (EpicLoginButtonWidget) {
		EpicLoginButtonWidget->SetVisibility(bIsEosSupported ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UUIAuthPageWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {

			if (AuthorizationState == EAuthorizationState::LoginFail) {
				return;
			}

			if (AuthorizationState == EAuthorizationState::LoginSuccess) {
				if (LoginSuccessSound) {
					PlaySound(LoginSuccessSound);
				}
			}

		};

		AuthSubsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
	}
}

void UUIAuthPageWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnChangeAuthorizationState().RemoveAll(this);
	}
}


#undef LOCTEXT_NAMESPACE
