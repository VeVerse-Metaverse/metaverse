// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIPageContentWidget.h"
#include "UIAuthMainPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIAuthMainPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:

#pragma region Widgets
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIFormInputWidget* EmailInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIFormInputWidget* PasswordInputWidget;

	/** Validation result message. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* MessageWidget;

#pragma endregion Widgets

#pragma region Buttons
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* LoginButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* RegisterButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* ResetPasswordButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* QuitButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Register Page"))
	TSubclassOf<UUIPageContentWidget> RegisterPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Resset Page"))
	TSubclassOf<UUIPageContentWidget> ResetPageClass;
	
	/** Sound to play on successful login. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* LoginSuccessSound;

	UPROPERTY(EditDefaultsOnly, Category="Game")
	float ExitDelay = 1.0f;

protected:
	virtual void NativeOnInitializedShared() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void NativeOnLoginBegin() const;
	void NativeOnLoginEnd() const;

	void ShowMessageWidget() const;
	void HideMessageWidget() const;
	
	void LoginButton_OnClicked() const;
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

	void ShowMessage();
	void HideMessage();
	void SetMessageText(const FText& InText);
};
