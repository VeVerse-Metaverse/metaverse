// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once
#include "Components/UIButtonWidget.h"
#include "UIWidgetBase.h"

#include "UIKeyWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnKeyClicked, const FKey& /*InKey*/);

/**
 * 
 */
UCLASS()
class VEUI_API UUIKeyWidget : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

public:
	/** Called when key is clicked. */
	FOnKeyClicked& GetOnClicked() { return OnClicked; }

	/** Called from the parent keyboard to update upper case state. */
	virtual void SetUppercase(bool bInIsUppercase);

protected:
	/** Updates key label. */
	void UpdateLabel() const;

	/** Flag to track the letter case. */
	bool bIsUppercase = false;

	/** Button clicked callback delegate. */
	FOnKeyClicked OnClicked;

	/** Clickable button widget. */
	UPROPERTY(meta=(BindWidget))
	UUIButtonWidget* ButtonWidget;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ButtonHighlightAnimation = nullptr;

public:
	/** Key sent when in lower case mode. */
	UPROPERTY(EditAnywhere)
	FKey Key = EKeys::Invalid;

	/** Key sent when in upper case mode. */
	UPROPERTY(EditAnywhere)
	FKey UpperCaseKey = EKeys::Invalid;

	/** Default label text. */
	UPROPERTY(EditAnywhere)
	FText Label;

	/** Upper case label text. */
	UPROPERTY(EditAnywhere)
	FText UpperCaseLabel;
};

UCLASS()
class VEUI_API UUIHighlightableKeyWidget final : public UUIKeyWidget {
	GENERATED_BODY()

public:
	virtual void SetUppercase(bool bInIsUppercase) override;

};