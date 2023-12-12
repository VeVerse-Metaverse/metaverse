// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIAuthRegisterPageWidget.h"

#include "UIPageManagerWidget.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIFormInputWidget.h"
#include "Components/UIInviteCodeInput.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Animation/UMGSequencePlayer.h"


#define LOCTEXT_NAMESPACE "VeUI"

void UUIAuthRegisterPageWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	if (RegisterButtonWidget) {
		RegisterButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIAuthRegisterPageWidget::Register);
	}

	if (LoginButtonWidget) {
		LoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GetPageManager()->ShowFirstPage();
		});
	}

	if (PasswordInputWidget) {
		PasswordInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
			PasswordConfirmationInputWidget->ValidationRegexPattern = "^" + InText.ToString() + "$";
			PasswordConfirmationInputWidget->Validate();
		});
	}
}

void UUIAuthRegisterPageWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnUserRegister().AddWeakLambda(this, [this](const FApiUserMetadata&, const bool bSuccessful, const FString& Error) {
			if (bSuccessful) {
				GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
				if (NotificationSubsystem) {
					FUINotificationData NotificationData;
					NotificationData.Lifetime = 60.f;
					NotificationData.Type = EUINotificationType::Info;
					NotificationData.Header = LOCTEXT("RegisterSuccessNotificationHeader", "Email Verification");
					NotificationData.Message = LOCTEXT("RegisterSuccessNotificationMessage", "Follow the steps in the email to activate your account. Without activation your experience could be limited.");
					NotificationSubsystem->AddNotification(NotificationData);
				}

				if (RegisterSuccessSound) { PlaySound(RegisterSuccessSound); }
				ResetState();
				GetPageManager()->ShowFirstPage();
			} else {
				SetMessageText(FText::FromString(Error));
			}
		});
	}

	GET_MODULE_SUBSYSTEM(AuthSubsystem2, Api2, Auth);
	if (AuthSubsystem2) {
		auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
			if (AuthorizationState == EAuthorizationState::LogoutSuccess) {
				ResetState();
			}
		};

		AuthSubsystem2->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
	}
}

void UUIAuthRegisterPageWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnUserRegister().RemoveAll(this);
	}

	GET_MODULE_SUBSYSTEM(AuthSubsystem2, Api2, Auth);
	if (AuthSubsystem2) {
		AuthSubsystem2->GetOnChangeAuthorizationState().RemoveAll(this);
	}
}

void UUIAuthRegisterPageWidget::Register() const {
	// if (OnRegisterButtonClicked.IsBound()) {
	// 	OnRegisterButtonClicked.Broadcast();
	// }

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api, Auth);
	if (AuthSubsystem) {
		FApiRegisterUserMetadata Metadata;
		Metadata.Email = EmailInputWidget->GetText().ToString();
		Metadata.Name = NameInputWidget->GetText().ToString();
		Metadata.Password = PasswordInputWidget->GetText().ToString();
		Metadata.PasswordConfirmation = PasswordConfirmationInputWidget->GetText().ToString();
		Metadata.InviteCode = InviteCodeInputWidget->GetText().ToString();
		AuthSubsystem->Register(Metadata);
	}
}

void UUIAuthRegisterPageWidget::ResetState() {
	EmailInputWidget->SetText(FText());
	NameInputWidget->SetText(FText());
	PasswordInputWidget->SetText(FText());
	PasswordConfirmationInputWidget->SetText(FText());
	InviteCodeInputWidget->ClearText();
	SetMessageText(FText());
}

//--------------------------------------------------------------------------
// Message

void UUIAuthRegisterPageWidget::SetMessageText(const FText& InText) {
	MessageText = InText;

	if (IsValid(MessageWidget)) {
		if (MessageText.IsEmpty()) {
			HideMessage();
		} else {
			ShowMessage();
		}
	}
}

void UUIAuthRegisterPageWidget::ShowMessage() {
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

void UUIAuthRegisterPageWidget::HideMessage() {
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

#undef LOCTEXT_NAMESPACE
