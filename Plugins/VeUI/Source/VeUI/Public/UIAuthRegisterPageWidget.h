// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPageContentWidget.h"
#include "UIAuthRegisterPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIAuthRegisterPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:

#pragma region Widgets
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIFormInputWidget* EmailInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIFormInputWidget* NameInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIFormInputWidget* PasswordInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIFormInputWidget* PasswordConfirmationInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIInviteCodeInput* InviteCodeInputWidget;

	/** Validation result message. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* MessageWidget;

#pragma endregion Widgets

#pragma region Buttons
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* LoginButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* RegisterButtonWidget;

#pragma endregion Buttons

	/** Sound to play on successful login. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* RegisterSuccessSound;

protected:
	virtual void NativeOnInitializedShared() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Register() const;
	virtual void ResetState();

	//--------------------------------------------------------------------------
	// Message

public:
	/** Sound to play on message fade in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* MessageFadeInSound;

	/** Sound to play on message fade out. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* MessageFadeOutSound;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Animation", meta=(BindAnimOptional))
	UWidgetAnimation* MessageFadeInAnimation;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Animation", meta=(BindAnimOptional))
	UWidgetAnimation* MessageFadeOutAnimation;

protected:
	FText MessageText;

	void SetMessageText(const FText& InText);
	void ShowMessage();
	void HideMessage();
};
