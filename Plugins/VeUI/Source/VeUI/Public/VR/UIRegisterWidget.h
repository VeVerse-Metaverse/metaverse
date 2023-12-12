// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIFormInputWidget.h"
#include "Components/UIInviteCodeInput.h"

#include "UIRegisterWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIRegisterWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

protected:
	void Register() const;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitializedShared() override;
	virtual void ResetState();

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* EmailInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* NameInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* PasswordInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* PasswordConfirmationInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInviteCodeInput* InviteCodeInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* LoginButtonWidget;

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
	USoundBase* RegisterSuccessSound;

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

	FOnButtonClicked OnLoginButtonClicked;
	FOnButtonClicked OnRegisterButtonClicked;
	FOnButtonClicked OnCloseButtonClicked;

private:
	UFUNCTION()
	void OnPasswordTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);
};
