// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIAuthEmailPageWidget.h"

#include "UIPageManagerWidget.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIFormInputWidget.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Animation/UMGSequencePlayer.h"
#include "VeUI.h"
#include "Subsystems/UIDialogSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIAuthEmailPageWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	HideMessageWidget();

	if (LoginButtonWidget) {
		LoginButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIAuthEmailPageWidget::LoginButton_OnClicked);
	}

	if (PasswordInputWidget) {
		PasswordInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText&, ETextCommit::Type InCommitMethod) {
			if (InCommitMethod == ETextCommit::OnEnter) {
				LoginButton_OnClicked();
			}
		});
	}

	if (LoginOptionsButtonWidget) {
		LoginOptionsButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GetPageManager()->ShowFirstPage();
		});
	}

	if (EmailInputWidget) {
		EmailInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText&, ETextCommit::Type InCommitMethod) {
			if (InCommitMethod == ETextCommit::OnEnter) {
				LoginButton_OnClicked();
			}
		});
	}

	if (RegisterButtonWidget) {
		RegisterButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GetPageManager()->OpenPage(RegisterPageClass);
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

void UUIAuthEmailPageWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
			NativeOnLoginEnd();

			if (AuthorizationState == EAuthorizationState::LoginFail) {
				SetMessageText(FText::FromString(AuthSubsystem->GetAuthorizationMessage()));
				return;
			}

			if (AuthorizationState == EAuthorizationState::LoginSuccess) {
				if (LoginSuccessSound) {
					PlaySound(LoginSuccessSound);
				}
			}

			ResetState();
		};

		AuthSubsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
	}
}

void UUIAuthEmailPageWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnChangeAuthorizationState().RemoveAll(this);
	}
}

void UUIAuthEmailPageWidget::NativeOnLoginBegin() const {
	if (LoginButtonWidget) {
		LoginButtonWidget->SetIsEnabled(false);
	}
	HideMessageWidget();
}

void UUIAuthEmailPageWidget::NativeOnLoginEnd() const {
	if (LoginButtonWidget) {
		LoginButtonWidget->SetIsEnabled(true);
	}
	ShowMessageWidget();
}

void UUIAuthEmailPageWidget::ShowMessageWidget() const {
	if (MessageWidget) {
		MessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UUIAuthEmailPageWidget::HideMessageWidget() const {
	if (MessageWidget) {
		MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUIAuthEmailPageWidget::ShowMessage() {
	if (MessageFadeInSound) {
		PlaySound(MessageFadeInSound);
	}

	if (MessageFadeInAnimation) {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(MessageFadeInAnimation);
		if (SequencePlayer) {
			SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
				ShowMessageWidget();
				MessageWidget->SetText(MessageText);
			});
			return;
		}
	}

	ShowMessageWidget();
	MessageWidget->SetText(MessageText);
}

void UUIAuthEmailPageWidget::HideMessage() {
	if (MessageFadeOutSound) {
		PlaySound(MessageFadeOutSound);
	}

	if (MessageFadeOutAnimation) {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(MessageFadeOutAnimation);
		if (SequencePlayer) {
			SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
				HideMessageWidget();
				MessageWidget->SetText(MessageText);
			});
			return;
		}
	} else {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationReverse(MessageFadeInAnimation);
		if (SequencePlayer) {
			SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
				HideMessageWidget();
				MessageWidget->SetText(MessageText);
			});
			return;
		}
	}

	HideMessageWidget();
	MessageWidget->SetText(MessageText);
}

void UUIAuthEmailPageWidget::SetMessageText(const FText& InText) {
	MessageText = InText;

	if (MessageWidget) {
		if (MessageText.IsEmpty()) {
			HideMessage();
		} else {
			ShowMessage();
		}
	}
}

void UUIAuthEmailPageWidget::LoginButton_OnClicked() const {
	// if (OnLoginButtonClicked.IsBound()) {
	// 	OnLoginButtonClicked.Broadcast();
	// }
	FApiUserLoginMetadata Metadata;
	Metadata.Email = EmailInputWidget->GetText().ToString();
	Metadata.Password = PasswordInputWidget->GetText().ToString();

	PasswordInputWidget->ClearText();

	// GET_MODULE_SUBSYSTEM(AuthSubsystem, Api, Auth);
	// if (AuthSubsystem) {
	// 	AuthSubsystem->Login(Metadata);
	// }

	GET_MODULE_SUBSYSTEM(Auth2Subsystem, Api2, Auth);
	if (Auth2Subsystem) {
		NativeOnLoginBegin();
		Auth2Subsystem->Login(Metadata);
	}
}

void UUIAuthEmailPageWidget::ResetState() {
	EmailInputWidget->SetText(FText());
	PasswordInputWidget->SetText(FText());
	SetMessageText(FText());
}

#undef LOCTEXT_NAMESPACE
