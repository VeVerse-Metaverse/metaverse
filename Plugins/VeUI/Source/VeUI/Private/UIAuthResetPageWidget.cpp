// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIAuthResetPageWidget.h"

#include "Api2UserSubsystem.h"
#include "UIPageManagerWidget.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIFormInputWidget.h"
#include "VeUI.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Subsystems/UIDialogSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIAuthResetPageWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	if (ResetPasswordButtonWidget) {
		ResetPasswordButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIAuthResetPageWidget::Reset);
	}

	if (LoginButtonWidget) {
		LoginButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GetPageManager()->ShowFirstPage();
		});
	}

	if (RegisterButtonWidget) {
		RegisterButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GetPageManager()->OpenPage(RegisterPageClass);
		});
	}
}

void UUIAuthResetPageWidget::Reset() {}

void UUIAuthResetPageWidget::ResetPasswordRequest() {
	if (!EmailInputWidget) {
		return;
	}

	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (UserSubsystem) {
		const FString Email = EmailInputWidget->GetText().ToString();

		auto CallbackPtr = MakeShared<FOnOkRequestCompleted2>();

		CallbackPtr->BindWeakLambda(this, [this](const bool bInOk, const EApi2ResponseCode InResponseCode, const FString& Error) {
			const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;

			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *Error);
			}
			ResetPasswordResponse(bInOk, bInSuccessful, Error);
		});

		UserSubsystem->PasswordReset(Email, CallbackPtr);
	}
}

void UUIAuthResetPageWidget::ResetPasswordResponse(const bool bInOk, const bool bInSuccessful, const FString& Error) {
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
			GetPageManager()->ShowFirstPage();
		});

		DialogSubsystem->ShowDialog(DialogData, OnDialogButtonClicked);
	}
}

#undef LOCTEXT_NAMESPACE
