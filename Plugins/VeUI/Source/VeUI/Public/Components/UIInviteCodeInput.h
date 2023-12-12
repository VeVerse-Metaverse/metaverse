// Author: Egor A. Pristavka
// Copyright (c) 2022 LE7EL AS. All right reserved.

// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "Components/UIInviteInputWidget.h"
#include "UIInviteCodeInput.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIInviteCodeInput final : public UUIWidgetBase
 {
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	bool bShowTitle;

	/** Title widget text. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	FText TitleText;

	/** Text that appears inside each character input as a hint. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	FText InputHintText;

	/** Validation result message text. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	FText MessageText;

	/** Field title or description. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* TitleWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInviteInputWidget* Character0InputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInviteInputWidget* Character1InputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInviteInputWidget* Character2InputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInviteInputWidget* Character3InputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInviteInputWidget* Character4InputWidget;

	UPROPERTY()
	TArray<UUIInviteInputWidget*> CharacterInputWidgets;

	/** Validation result message. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* MessageWidget;

	UFUNCTION(BlueprintCallable)
	void ShowTitleWidget();

	UFUNCTION(BlueprintCallable)
	void HideTitleWidget();

	UFUNCTION(BlueprintCallable)
	FText GetText() const;

	UFUNCTION(BlueprintCallable)
	void SetText(FText InText);

	UFUNCTION(BlueprintCallable)
	void ClearText();

private:
	UFUNCTION()
	void OnCharacter0TextChangedCallback(const FText& InText);

	UFUNCTION()
	void OnCharacter1TextChangedCallback(const FText& InText);

	UFUNCTION()
	void OnCharacter2TextChangedCallback(const FText& InText);

	UFUNCTION()
	void OnCharacter3TextChangedCallback(const FText& InText);
	
	UFUNCTION()
	void OnCharacter4TextChangedCallback(const FText& InText);

public:
	FOnInputTextChanged OnInviteCodeCommitted;
};
