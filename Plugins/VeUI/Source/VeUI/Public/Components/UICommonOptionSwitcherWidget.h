// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIButtonWidget.h"
#include "UIWidgetBase.h"

#include "UICommonOptionSwitcherWidget.generated.h"


/**
 * 
 */
UCLASS()
class VEUI_API UUICommonOptionSwitcherWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
#pragma region Fields
	
	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	UTextBlock* OptionTextWidget;

#pragma endregion Fields

#pragma region Buttons
	
	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	UUIButtonWidget* PrevButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	UUIButtonWidget* NextButtonWidget;

#pragma endregion Buttons
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	FText OptionName;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	TArray<FText> OptionList;

	int32 GetValue() const { return CurrentIndex; }
	bool SetValue(int32 Value);
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	int32 CurrentIndex = INDEX_NONE;

	void UpdateInfo();
	
private:
	FDelegateHandle OnPrevPageButtonClickedDelegateHandle;
	FDelegateHandle OnNextPageButtonClickedDelegateHandle;
	
};
