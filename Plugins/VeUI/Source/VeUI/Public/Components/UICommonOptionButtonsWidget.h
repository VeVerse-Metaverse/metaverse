// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIButtonWidget.h"
#include "UIWidgetBase.h"

#include "UICommonOptionButtonsWidget.generated.h"


/**
 * 
 */
UCLASS()
class VEUI_API UUICommonOptionButtonsWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	// DECLARE_MULTICAST_DELEGATE_TwoParams(FOnOptionButtons, UUICommonOptionButtonsWidget* /*OptionButtons*/, int32 /*Value*/);
	
public:
#pragma region Fields
	
	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	class UHorizontalBox* ContainerWidget;

#pragma endregion Fields

#pragma region Buttons
#pragma endregion Buttons
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	FText OptionName;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	TArray<FText> OptionList;

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<UUIButtonWidget> ButtonClass;
	
	int32 GetValue() const { return CurrentValue; }

	FOnButtonClicked& GetOnOptionChanged() { return OnOptionChanged; }
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	int32 CurrentValue = INDEX_NONE;

private:
	FOnButtonClicked OnOptionChanged;
	
};
