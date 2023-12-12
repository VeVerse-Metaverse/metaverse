// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIAuthMenuRootWidget.h"

#include "Subsystems/UIDialogSubsystem.h"
#include "VeUI.h"

void UUIAuthMenuRootWidget::NativeOnInitializedShared() {

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIDialogSubsystem> DialogSubsystem = UIModule->GetDialogSubsystem()) {
			DialogSubsystem->GetOnExitConfirmed().AddWeakLambda(this, [this]() {
			// Check if is hidden.
			Hide();
		});
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Dialog)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
	
	// if (AVeHUD* HUD = GetHUD()) {
	// 	HUD->GetOnExitConfirmed().AddWeakLambda(this, [this]() {
	// 		// Check if is hidden.
	// 		Hide();
	// 	});
	// }

	if (!LoginWidget->OnRegisterButtonClicked.IsBoundToObject(this)) {
		LoginWidget->OnRegisterButtonClicked.AddWeakLambda(this, [this]() {
			NavigateToRegisterScreen();
		});
	}

	if (!LoginWidget->OnRestorePasswordButtonClicked.IsBoundToObject(this)) {
		LoginWidget->OnRestorePasswordButtonClicked.AddWeakLambda(this, [this]() {
			NavigateToResetPasswordScreen();
		});
	}

	if (!RegisterWidget->OnLoginButtonClicked.IsBoundToObject(this)) {
		RegisterWidget->OnLoginButtonClicked.AddWeakLambda(this, [this]() {
			NavigateToLoginScreen();
		});
	}

	if (!ResetPasswordWidget->OnRegisterButtonClicked.IsBoundToObject(this)) {
		ResetPasswordWidget->OnRegisterButtonClicked.AddWeakLambda(this, [this]() {
			NavigateToRegisterScreen();
		});
	}

	if (!ResetPasswordWidget->OnLoginButtonClicked.IsBoundToObject(this)) {
		ResetPasswordWidget->OnLoginButtonClicked.AddWeakLambda(this, [this]() {
			NavigateToLoginScreen();
		});
	}
	if (!ResetPasswordWidget->OnRestorePasswordButtonClicked.IsBoundToObject(this)) {
		ResetPasswordWidget->OnRestorePasswordButtonClicked.AddWeakLambda(this, [this]() {
			NavigateToLoginScreen();
		});
	}

	Super::NativeOnInitializedShared();
}

void UUIAuthMenuRootWidget::NativeConstruct() {
	bLoginWidgetVisible = true;
	bRegisterWidgetVisible = true;
	bResetPasswordWidgetVisible = true;

	ShowLoginWidget(false, false);
	HideRegisterWidget(false, false);
	HideResetPasswordWidget(false, false);

	Super::NativeConstruct();
}

void UUIAuthMenuRootWidget::NavigateToRegisterScreen(const bool bAnimate, const bool bPlaySound) {
	ShowRegisterWidget(bAnimate, bPlaySound);
	HideLoginWidget(bAnimate, bPlaySound);
	HideResetPasswordWidget(bAnimate, bPlaySound);
}

void UUIAuthMenuRootWidget::NavigateToLoginScreen(const bool bAnimate, const bool bPlaySound) {
	ShowLoginWidget(bAnimate, bPlaySound);
	HideRegisterWidget(bAnimate, bPlaySound);
	HideResetPasswordWidget(bAnimate, bPlaySound);
}

void UUIAuthMenuRootWidget::NavigateToResetPasswordScreen(const bool bAnimate, const bool bPlaySound) {
	ShowResetPasswordWidget(bAnimate, bPlaySound);
	HideLoginWidget(bAnimate, bPlaySound);
	HideRegisterWidget(bAnimate, bPlaySound);
}

void UUIAuthMenuRootWidget::ShowLoginWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bLoginWidgetVisible) {
		bLoginWidgetVisible = true;
		// LoginWidget->ResetState();
		// LoginWidget->ResetAnimationState();
		LoginWidget->Show();
	}
}

void UUIAuthMenuRootWidget::ShowRegisterWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bRegisterWidgetVisible) {
		bRegisterWidgetVisible = true;
		// RegisterWidget->ResetState();
		// RegisterWidget->ResetAnimationState();
		RegisterWidget->Show();
	}
}

void UUIAuthMenuRootWidget::ShowResetPasswordWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bResetPasswordWidgetVisible) {
		bResetPasswordWidgetVisible = true;
		// ResetPasswordWidget->ResetState();
		// ResetPasswordWidget->ResetAnimationState();
		ResetPasswordWidget->Show();
	}
}

void UUIAuthMenuRootWidget::HideLoginWidget(const bool bAnimate, const bool bPlaySound) {
	if (bLoginWidgetVisible) {
		bLoginWidgetVisible = false;
		LoginWidget->Hide();
	}
}

void UUIAuthMenuRootWidget::HideRegisterWidget(const bool bAnimate, const bool bPlaySound) {
	if (bRegisterWidgetVisible) {
		bRegisterWidgetVisible = false;
		RegisterWidget->Hide();
	}
}

void UUIAuthMenuRootWidget::HideResetPasswordWidget(const bool bAnimate, const bool bPlaySound) {
	if (bResetPasswordWidgetVisible) {
		bResetPasswordWidgetVisible = false;
		ResetPasswordWidget->Hide();
	}
}
