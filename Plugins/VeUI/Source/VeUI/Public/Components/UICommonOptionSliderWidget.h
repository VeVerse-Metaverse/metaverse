// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIButtonWidget.h"
#include "UIWidgetBase.h"

#include "Components/Slider.h"
#include "UICommonOptionSliderWidget.generated.h"


/**
 * 
 */
UCLASS()
class VEUI_API UUICommonOptionSliderWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
#pragma region Fields
	
	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	USlider* SliderWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	UTextBlock* ValueTextWidget;

#pragma endregion Fields

#pragma region Buttons
#pragma endregion Buttons

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	FText OptionName;

	int32 GetMaxValue() const;
	void SetMaxValue(int32 Value);

	int32 GetMinValue() const;
	void SetMinValue(int32 Value);
	
	int32 GetValue() const;
	void SetValue(int32 Value);
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	int32 MinValue = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	int32 MaxValue = 100;

	void UpdateInfo();

	UFUNCTION()
	void OnSliderValueChangedCallback(float Value);
	
};
