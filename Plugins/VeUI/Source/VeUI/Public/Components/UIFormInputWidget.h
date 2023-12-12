// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "Components/UIInputWidget.h"
#include "UIFormInputWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIFormInputWidget : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	virtual void NativeOnInitializedShared() override;
	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	bool bShowTitle;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	FText TitleText;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	FText InputHintText;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	bool bIsPassword;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	FText MessageText;

	/** Field title or description. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* TitleWidget;

	/** Input. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInputWidget* InputWidget;

	/** Validation result message. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* MessageWidget;

	/** Sound to play on message fade in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* MessageFadeInSound;

	/** Sound to play on message fade out. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* MessageFadeOutSound;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* MessageFadeInAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* MessageFadeOutAnimation;

	/** Shows the validation message. */
	UFUNCTION(BlueprintCallable, Category="Widget|Animation")
	void ShowMessage(bool bAnimate = true, bool bPlaySound = true);

	/** Hides the validation message. */
	UFUNCTION(BlueprintCallable, Category="Widget|Animation")
	void HideMessage(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable)
	void ShowTitleWidget() const;

	UFUNCTION(BlueprintCallable)
	void HideTitleWidget() const;

	UFUNCTION(BlueprintCallable)
	void SetMessageText(const FText& InText, bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable)
	FText GetText() const;

	UFUNCTION(BlueprintCallable)
	void SetText(const FText& InText) const;

	UFUNCTION(BlueprintCallable)
	void ClearText();

	UFUNCTION(BlueprintCallable)
	void SetIsReadOnly(bool bReadonly);
	
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category="Widget|Content")
	FString ValidationRegexPattern;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	FText ValidationErrorMessage;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Validate();

	FOnInputFocusChanged OnInputFocusChanged;

private:
	UFUNCTION()
	void OnTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);

	UFUNCTION()
	void OnTextChangedCallback(const FText& InText);

public:
	FOnInputTextChanged OnTextChanged;
	FOnInputTextCommited OnTextCommitted;
};
