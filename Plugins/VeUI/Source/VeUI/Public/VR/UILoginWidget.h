// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIButtonWidget.h"
#include "Components/UIFormInputWidget.h"
#include "UIWidgetBase.h"

#include "UILoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUILoginWidget : public UUIWidgetBase
 {
	GENERATED_BODY()

protected:
	void Login() const;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitializedShared() override;
	virtual void ResetState();

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* EmailInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* PasswordInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* LoginButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ResetPasswordButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* RegisterButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CloseButtonWidget;

	/** Validation result message. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* MessageWidget;

	/** Sound to play on message fade in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* MessageFadeInSound;

	/** Sound to play on message fade out. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* MessageFadeOutSound;

	/** Sound to play on successful login. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* LoginSuccessSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	FText MessageText;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Animation", meta=(BindAnimOptional))
	UWidgetAnimation* MessageFadeInAnimation;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Animation", meta=(BindAnimOptional))
	UWidgetAnimation* MessageFadeOutAnimation;

	UFUNCTION(BlueprintCallable, Category="Widget|Animation")
	void ShowMessage();

	UFUNCTION(BlueprintCallable, Category="Widget|Animation")
	void HideMessage();

	UFUNCTION(BlueprintCallable)
	void SetMessageText(const FText& InText);

	virtual void Hide() override;

public:
	FOnButtonClicked OnLoginButtonClicked;
	FOnButtonClicked OnRegisterButtonClicked;
	FOnButtonClicked OnRestorePasswordButtonClicked;
	FOnButtonClicked OnCloseButtonClicked;
};
