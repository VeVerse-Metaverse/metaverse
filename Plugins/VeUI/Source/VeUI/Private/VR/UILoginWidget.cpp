// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UILoginWidget.h"

#include "VeHUD.h"
#include "Animation/UMGSequencePlayer.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"


void UUILoginWidget::Login() const {
	if (OnLoginButtonClicked.IsBound()) {
		OnLoginButtonClicked.Broadcast();
	}

	FApiUserLoginMetadata Metadata;
	Metadata.Email = EmailInputWidget->GetText().ToString();
	Metadata.Password = PasswordInputWidget->GetText().ToString();

	// GET_MODULE_SUBSYSTEM(AuthSubsystem, Api, Auth);
	// if (AuthSubsystem) {
	// 	AuthSubsystem->Login(Metadata);
	// }

	GET_MODULE_SUBSYSTEM(Auth2Subsystem, Api2, Auth);
	if (Auth2Subsystem) {
		Auth2Subsystem->Login(Metadata);
	}
}

void UUILoginWidget::NativeConstruct() {
	Super::NativeConstruct();

	// todo: callback interface, unbind delegates
	// Deprecated
#if 0 
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		if (!AuthSubsystem->GetOnUserLogin().IsBoundToObject(this)) {
			AuthSubsystem->GetOnUserLogin().AddWeakLambda(this, [this](const FApiUserMetadata& User, const bool bSuccessful, const FString& Error) {
				if (bSuccessful) {
					if (LoginSuccessSound) {
						PlaySound(LoginSuccessSound);
					}
					ResetState();
				} else {
					SetMessageText(FText::FromString(Error));
				}
			});
		}
		if (!AuthSubsystem->GetOnUserLogout().IsBoundToObject(this)) {
			AuthSubsystem->GetOnUserLogout().AddWeakLambda(this, [this]() {
				ResetState();
			});
		}
		
	}
#endif
}

void UUILoginWidget::NativeOnInitializedShared() {
	if (CloseButtonWidget) {
		if (!CloseButtonWidget->GetOnButtonClicked().IsBound()) {
			CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (auto* HUD = GetHUD()) {
					HUD->RequestExitConfirmation();
				}
			});
		}
	}

	if (LoginButtonWidget) {
		if (!LoginButtonWidget->GetOnButtonClicked().IsBound()) {
			LoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				Login();
			});
		}
	}

	if (EmailInputWidget) {
		if (!EmailInputWidget->OnTextCommitted.IsBoundToObject(this)) {
			EmailInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& /*InText*/, const ETextCommit::Type InCommitMethod) {
				if (InCommitMethod == ETextCommit::OnEnter) {
					Login();
				}
			});
		}
	}

	if (PasswordInputWidget) {
		if (!PasswordInputWidget->OnTextCommitted.IsBoundToObject(this)) {
			PasswordInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& /*InText*/, const ETextCommit::Type InCommitMethod) {
				if (InCommitMethod == ETextCommit::OnEnter) {
					Login();
				}
			});
		}
	}

	if (RegisterButtonWidget) {
		if (!RegisterButtonWidget->GetOnButtonClicked().IsBound()) {
			RegisterButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnRegisterButtonClicked.IsBound()) {
					OnRegisterButtonClicked.Broadcast();
				}
			});
		}
	}

	if (ResetPasswordButtonWidget) {
		if (!ResetPasswordButtonWidget->GetOnButtonClicked().IsBound()) {
			ResetPasswordButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnRestorePasswordButtonClicked.IsBound()) {
					OnRestorePasswordButtonClicked.Broadcast();
				}
			});
		}
	}

	Super::NativeOnInitializedShared();
}

void UUILoginWidget::ResetState() {
	EmailInputWidget->SetText(FText());
	PasswordInputWidget->SetText(FText());
	SetMessageText(FText());
}

void UUILoginWidget::ShowMessage() {
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

void UUILoginWidget::HideMessage() {
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

void UUILoginWidget::SetMessageText(const FText& InText) {
	MessageText = InText;

	if (MessageWidget) {
		if (MessageText.IsEmpty()) {
			HideMessage();
		} else {
			ShowMessage();
		}
	}
}

void UUILoginWidget::Hide() {
	SetMessageText(FText());
	// return Super::Hide(bAnimate, bPlaySound);
}
