// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once



#include "Components/UIFormInputWidget.h"
#include "UIWidgetBase.h"
#include "UIResetPasswordWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIResetPasswordWidget : public UUIWidgetBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIFormInputWidget* EmailInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* ResetPasswordButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* LoginButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* RegisterButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

public:
	FOnButtonClicked OnRestorePasswordButtonClicked;
	FOnButtonClicked OnLoginButtonClicked;
	FOnButtonClicked OnRegisterButtonClicked;
	FOnButtonClicked OnCloseButtonClicked;

protected:
	virtual void NativeOnInitializedShared() override;

	void ResetPasswordRequest();
	void ResetPasswordResponse(const bool bInOk, const bool bInSuccessful, const FString& Error);
	
};
