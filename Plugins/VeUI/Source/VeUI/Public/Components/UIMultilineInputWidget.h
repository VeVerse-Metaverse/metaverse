// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIWidgetBase.h"
#include "Components/UIButtonWidget.h"
#include "Components/EditableText.h"
#include "UIMultilineInputWidget.generated.h"

class UMultiLineEditableText;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInputTextChanged, const FText&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInputTextCommited, const FText&, ETextCommit::Type);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInputFocusChanged, bool);

/**
* 
*/
UCLASS()
class VEUI_API UUIMultilineInputWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	FText HintText;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UMultiLineEditableText* EditableTextWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	bool bShowClearButton;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* ClearButtonWidget;

	/** Root widget fade in animation. */
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* FocusInAnimation;

	/** Root widget fade out animation (optional). Reversed fade in animation will be used if not defined. */
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* FocusOutAnimation;

	/** Sound to play on focus in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* FocusInSound;

	/** Sound to play on focus out. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* FocusOutSound;

	/** Sound to play on text input. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* TextChangeSound;

	/** Sound to play on text commit. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* TextCommitSound;

	FOnInputFocusChanged OnInputFocusChanged;

	/** Plays the fade in animation. */
	UUMGSequencePlayer* AnimateFocusIn();

	/** Plays the fade out animation. */
	UUMGSequencePlayer* AnimateFocusOut();

	UFUNCTION(BlueprintCallable)
	void SetHintText(const FText& InText) const;

	UFUNCTION(BlueprintCallable)
	FText GetText() const;

	UFUNCTION(BlueprintCallable)
	void SetText(const FText& InText) const;

	UFUNCTION(BlueprintCallable)
	void SetWidgetFocus() const;

private:
	UFUNCTION()
	void OnTextChangedCallback(const FText& InText);

	UFUNCTION()
	void OnTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);

public:
	FOnInputTextChanged OnTextChanged;
	FOnInputTextCommited OnTextCommitted;
};
