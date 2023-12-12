// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISocialLoginPageWidget.h"

#include "ApiUserMetadata.h"
#include "OAuthOAuthSubsystem.h"
#include "VeOAuth.h"
#include "UIPageManagerWidget.h"
#include "VeShared.h"
#include "Components/UIButtonWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"

constexpr static auto ProviderGoogle1 = "google";
constexpr static auto ProviderLE7EL1 = "le7el";
constexpr static auto ProviderDiscord1 = "discord";
constexpr static auto ProviderEpic1 = "eos";

void UUISocialLoginPageWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	if (GoogleLoginButtonWidget) {
		GoogleLoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			const auto ExternalProcessingCallback = MakeShared<FOnOAuthCompleted>();
			ExternalProcessingCallback->BindLambda([=](const FString& Token, const bool bInSuccessful, const FString& InError) {
				if (!bInSuccessful) {
					LogErrorF("failed to login with Google");
				}
			});

			GET_MODULE_SUBSYSTEM(OAuthSubsystem, OAuth, OAuth);
			if (OAuthSubsystem) {
				OAuthSubsystem->Login(ProviderGoogle1, ExternalProcessingCallback);
			}
		});
	}

	if (LevelLoginButtonWidget) {
		LevelLoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			const auto ExternalProcessingCallback = MakeShared<FOnOAuthCompleted>();
			ExternalProcessingCallback->BindLambda([=](const FString& Token, const bool bInSuccessful, const FString& InError) {
				if (!bInSuccessful) {
					LogErrorF("failed to login with LE7EL");
				}
			});

			GET_MODULE_SUBSYSTEM(OAuthSubsystem, OAuth, OAuth);
			if (OAuthSubsystem) {
				OAuthSubsystem->Login(ProviderLE7EL1, ExternalProcessingCallback);
			}
		});
	}

	if (DiscordLoginButtonWidget) {
		DiscordLoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			const auto ExternalProcessingCallback = MakeShared<FOnOAuthCompleted>();
			ExternalProcessingCallback->BindLambda([=](const FString& Token, const bool bInSuccessful, const FString& InError) {
				if (!bInSuccessful) {
					LogErrorF("failed to login with Discord");
				}
			});

			GET_MODULE_SUBSYSTEM(OAuthSubsystem, OAuth, OAuth);
			if (OAuthSubsystem) {
				OAuthSubsystem->Login(ProviderDiscord1, ExternalProcessingCallback);
			}
		});
	}

	if (EpicLoginButtonWidget) {
		EpicLoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			const auto ExternalProcessingCallback = MakeShared<FOnOAuthCompleted>();
			ExternalProcessingCallback->BindLambda([=](const FString& Token, const bool bInSuccessful, const FString& InError) {
				if (!bInSuccessful) {
					LogErrorF("failed to login with Epic");
				}
			});

			GET_MODULE_SUBSYSTEM(OAuthSubsystem, OAuth, OAuth);
			if (OAuthSubsystem) {
				OAuthSubsystem->Login(ProviderEpic1, ExternalProcessingCallback);
			}
		});
	}

	if (EmailLoginButtonWidget) {
		EmailLoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GetPageManager()->OpenPage(AuthEmailPageClass);
		});
	}

}

#undef LOCTEXT_NAMESPACE
