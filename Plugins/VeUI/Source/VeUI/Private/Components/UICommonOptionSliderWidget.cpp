// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Components/UICommonOptionSliderWidget.h"

#include "VeShared.h"
#include "Components/Slider.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUICommonOptionSliderWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	
	if (IsValid(NameTextWidget)) {
		NameTextWidget->SetText(OptionName);
	}
}

void UUICommonOptionSliderWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(SliderWidget)) {
		SliderWidget->OnValueChanged.AddUniqueDynamic(this, &UUICommonOptionSliderWidget::OnSliderValueChangedCallback);
	}

	if (IsValid(SliderWidget)) {
		SliderWidget->SetMinValue(MinValue);
		SliderWidget->SetMaxValue(MaxValue);
	}

	UpdateInfo();
}

int32 UUICommonOptionSliderWidget::GetMaxValue() const {
	return SliderWidget->GetMaxValue();
}

void UUICommonOptionSliderWidget::SetMaxValue(const int32 Value) {
	SliderWidget->SetMaxValue(Value);
	UpdateInfo();
}

int32 UUICommonOptionSliderWidget::GetMinValue() const {
	return SliderWidget->GetMinValue();
}

void UUICommonOptionSliderWidget::SetMinValue(const int32 Value) {
	SliderWidget->SetMinValue(Value);
	UpdateInfo();
}

int32 UUICommonOptionSliderWidget::GetValue() const {
	return SliderWidget->GetValue();
}

void UUICommonOptionSliderWidget::SetValue(const int32 Value) {
	SliderWidget->SetValue(Value);
	UpdateInfo();
}

void UUICommonOptionSliderWidget::UpdateInfo() {
	if (IsValid(ValueTextWidget) && IsValid(SliderWidget)) {
		FNumberFormattingOptions NumberFormat;
		NumberFormat.MinimumFractionalDigits = 0;
		NumberFormat.MaximumFractionalDigits = 0;
		ValueTextWidget->SetText(FText::AsNumber(static_cast<int32>(SliderWidget->GetValue()), &NumberFormat));
	}
}

void UUICommonOptionSliderWidget::OnSliderValueChangedCallback(float Value) {
	UpdateInfo();
}


#undef LOCTEXT_NAMESPACE
