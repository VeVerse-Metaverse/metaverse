// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIRegisterWidget.h"

#include "ApiAuthSubsystem.h"
#include "Animation/UMGSequencePlayer.h"
#include "VeHUD.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIRegisterWidget::Register() const {
	if (OnRegisterButtonClicked.IsBound()) {
		OnRegisterButtonClicked.Broadcast();
	}

	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
			{
				FApiRegisterUserMetadata Metadata;
				Metadata.Email = EmailInputWidget->GetText().ToString();
				Metadata.Name = NameInputWidget->GetText().ToString();
				Metadata.Password = PasswordInputWidget->GetText().ToString();
				Metadata.PasswordConfirmation = PasswordConfirmationInputWidget->GetText().ToString();
				Metadata.InviteCode = InviteCodeInputWidget->GetText().ToString();
				AuthSubsystem->Register(Metadata);
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUIRegisterWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api, Auth);
	if (AuthSubsystem) {
		if (!AuthSubsystem->GetOnUserRegister().IsBoundToObject(this)) {
			AuthSubsystem->GetOnUserRegister().AddWeakLambda(this, [this](const FApiUserMetadata&, const bool bSuccessful, const FString& Error) {
				if (bSuccessful) {
					GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
					if (NotificationSubsystem) {
						FUINotificationData NotificationData;
						NotificationData.Lifetime = 1200.f;
						NotificationData.Type = EUINotificationType::Info;
						NotificationData.Header = LOCTEXT("RegisterSuccessNotificationHeader", "Metaverse Email Verification");
						NotificationData.Message = LOCTEXT("RegisterSuccessNotificationMessage", "Follow the steps in the email to activate your account. Without activation your experience could be limited.");
						NotificationSubsystem->AddNotification(NotificationData);
					}

					if (RegisterSuccessSound) { PlaySound(RegisterSuccessSound); }
					ResetState();
				} else {
					SetMessageText(FText::FromString(Error));
				}
			});
		}
	}

	// Deprecated
#if 0
	GET_MODULE_SUBSYSTEM(AuthSubsystem2, Api2, Auth);
	if (AuthSubsystem2) {
		if (!AuthSubsystem2->GetOnUserLogout().IsBoundToObject(this)) {
			AuthSubsystem2->GetOnUserLogout().AddWeakLambda(this, [this]() {
				ResetState();
			});
		}
	}
#endif
}

void UUIRegisterWidget::NativeOnInitializedShared() {
	if (IsValid(CloseButtonWidget)) {
		if (!CloseButtonWidget->GetOnButtonClicked().IsBound()) {
			CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (auto* HUD = GetHUD()) {
					HUD->RequestExitConfirmation();
				}
			});
		}
	}

	if (IsValid(RegisterButtonWidget)) {
		if (!RegisterButtonWidget->GetOnButtonClicked().IsBound()) {
			RegisterButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				Register();
			});
		}
	}

	if (IsValid(LoginButtonWidget)) {
		if (!LoginButtonWidget->GetOnButtonClicked().IsBound()) {
			LoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				OnLoginButtonClicked.Broadcast();
			});
		}
	}

	if (IsValid(PasswordInputWidget)) {
		if (!PasswordInputWidget->OnTextCommitted.IsBound()) {
			PasswordInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				OnPasswordTextCommittedCallback(InText, InCommitMethod);
			});
		}
	}

	Super::NativeOnInitializedShared();
}

void UUIRegisterWidget::ResetState() {
	EmailInputWidget->SetText(FText());
	NameInputWidget->SetText(FText());
	PasswordInputWidget->SetText(FText());
	PasswordConfirmationInputWidget->SetText(FText());
	InviteCodeInputWidget->ClearText();
	SetMessageText(FText());
}

void UUIRegisterWidget::ShowMessage() {
	if (MessageFadeInSound) {
		PlaySound(MessageFadeInSound);
	}

	if (MessageFadeInAnimation) {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(MessageFadeInAnimation);
		if (SequencePlayer) {
			SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
				MessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				MessageWidget->SetText(MessageText);
			});
			return;
		}
	}

	MessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	MessageWidget->SetText(MessageText);
}

void UUIRegisterWidget::HideMessage() {
	if (MessageFadeOutSound) {
		PlaySound(MessageFadeOutSound);
	}

	if (MessageFadeOutAnimation) {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(MessageFadeOutAnimation);
		if (SequencePlayer) {
			SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
				MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
				MessageWidget->SetText(MessageText);
			});
			return;
		}
	} else {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationReverse(MessageFadeInAnimation);
		if (SequencePlayer) {
			SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
				MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
				MessageWidget->SetText(MessageText);
			});
			return;
		}
	}

	MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
	MessageWidget->SetText(MessageText);
}

void UUIRegisterWidget::SetMessageText(const FText& InText) {
	MessageText = InText;

	if (IsValid(MessageWidget)) {
		if (MessageText.IsEmpty()) {
			HideMessage();
		} else {
			ShowMessage();
		}
	}
}

void UUIRegisterWidget::OnPasswordTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod) {
	PasswordConfirmationInputWidget->ValidationRegexPattern = "^" + InText.ToString() + "$";
	PasswordConfirmationInputWidget->Validate();
}

#undef LOCTEXT_NAMESPACE
