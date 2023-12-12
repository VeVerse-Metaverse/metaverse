// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIDialogWidget.h"
#include "UILoginWidget.h"
#include "UIRegisterWidget.h"
#include "UIResetPasswordWidget.h"
#include "Blueprint/UserWidget.h"
#include "UIAuthMenuRootWidget.generated.h"

class UUIRegisterWidget;
/**
 * 
 */
UCLASS()
class VEUI_API UUIAuthMenuRootWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	virtual void NativeOnInitializedShared() override;
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUILoginWidget* LoginWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIRegisterWidget* RegisterWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIResetPasswordWidget* ResetPasswordWidget;

#pragma region Widget Control
	/** Fades in the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowLoginWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the register widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowRegisterWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowResetPasswordWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideLoginWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the register widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideRegisterWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideResetPasswordWidget(bool bAnimate = true, bool bPlaySound = true);
#pragma endregion

private:
#pragma region Navigation
	bool bLoginWidgetVisible;
	bool bRegisterWidgetVisible;
	bool bResetPasswordWidgetVisible;

public:
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToRegisterScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToLoginScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToResetPasswordScreen(bool bAnimate = true, bool bPlaySound = true);
#pragma endregion
};
