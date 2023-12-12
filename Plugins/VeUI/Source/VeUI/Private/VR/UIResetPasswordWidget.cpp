// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIResetPasswordWidget.h"

#include "VeHUD.h"
#include "VeShared.h"
#include "VeApi.h"
#include "VeUI.h"
#include "Subsystems/UIDialogSubsystem.h"


void UUIResetPasswordWidget::NativeOnInitializedShared() {
	if (IsValid(CloseButtonWidget)) {
		if (!CloseButtonWidget->GetOnButtonClicked().IsBound()) {
			CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (auto* HUD = GetHUD()) {
					HUD->RequestExitConfirmation();
				}
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

	if (IsValid(RegisterButtonWidget)) {
		if (!RegisterButtonWidget->GetOnButtonClicked().IsBound()) {
			RegisterButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				OnRegisterButtonClicked.Broadcast();
			});
		}
	}

	if (ResetPasswordButtonWidget) {
		if (!ResetPasswordButtonWidget->GetOnButtonClicked().IsBound()) {
			ResetPasswordButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				ResetPasswordRequest();
			});
		}
	}

	Super::NativeOnInitializedShared();
}

void UUIResetPasswordWidget::ResetPasswordRequest() {
	if (!EmailInputWidget) {
		return;
	}
	
	GET_MODULE_SUBSYSTEM(UserSubsystem, Api, User);
	if (UserSubsystem) {
		const FString Email = EmailInputWidget->GetText().ToString();

		auto CallbackPtr = MakeShared<FOnOkRequestCompleted>();

		CallbackPtr->BindWeakLambda(this, [this](const bool bInOk, const bool bInSuccessful, const FString& Error) {
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *Error);
			}
			ResetPasswordResponse(bInOk, bInSuccessful, Error);
		});
		
		if (!UserSubsystem->PasswordReset(Email, CallbackPtr)) {
			LogErrorF("failed to request");
		}
	}
}

void UUIResetPasswordWidget::ResetPasswordResponse(const bool bInOk, const bool bInSuccessful, const FString& Error) {

	GET_MODULE_SUBSYSTEM(DialogSubsystem, UI, Dialog);
	if (DialogSubsystem) {
		FUIDialogData DialogData;
		DialogData.Type = EUIDialogType::OkCancel;

		if (bInSuccessful && bInOk) {
			DialogData.HeaderText = NSLOCTEXT("VeUI", "ConfirmCloseDialogHeader", "Reset password");
			DialogData.MessageText = NSLOCTEXT("VeUI", "ConfirmCloseDialogMessage", "If this email is used we will send you the password reset link");
		} else {
			DialogData.HeaderText = NSLOCTEXT("VeUI", "ConfirmCloseDialogHeader", "Reset password");
			DialogData.MessageText = NSLOCTEXT("VeUI", "ConfirmCloseDialogMessage", "An error occurred, please try again later.");
		}

		DialogData.FirstButtonText = NSLOCTEXT("VeUI", "ConfirmCloseDialogButtonOk", "OK");
		DialogData.bCloseOnButtonClick = true;

		FUIDialogButtonClicked OnDialogButtonClicked;
		OnDialogButtonClicked.BindWeakLambda(this, [this](UUIDialogWidget*, const uint8& InButtonIndex) {
			OnRestorePasswordButtonClicked.Broadcast();
		});
		
		DialogSubsystem->ShowDialog(DialogData, OnDialogButtonClicked);
	}
}
