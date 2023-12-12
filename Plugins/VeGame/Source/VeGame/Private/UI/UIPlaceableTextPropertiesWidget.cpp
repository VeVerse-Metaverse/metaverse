// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIPlaceableTextPropertiesWidget.h"

#include "Components/SpinBox.h"
#include "Components/UIInputWidget.h"


void UUIPlaceableTextPropertiesWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (BaseTextInputWidget) {
		BaseTextInputWidget->OnTextChanged.AddWeakLambda(this, [=](const FText& Text) {
			if (!LockChanged) {
				NativeOnChanged();
			}
		});
	}
	
	if (BaseColorRSpinBoxWidget) {
		BaseColorRSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableTextPropertiesWidget::SpinBox_OnValueChanged);
	}
	if (BaseColorGSpinBoxWidget) {
		BaseColorGSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableTextPropertiesWidget::SpinBox_OnValueChanged);
	}
	if (BaseColorBSpinBoxWidget) {
		BaseColorBSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableTextPropertiesWidget::SpinBox_OnValueChanged);
	}

	LockChanged = true;
}

void UUIPlaceableTextPropertiesWidget::NativeConstruct() {
	Super::NativeConstruct();
	LockChanged = false;
}

void UUIPlaceableTextPropertiesWidget::SpinBox_OnValueChanged(float InValue) {
	if (!LockChanged) {
		NativeOnChanged();
	}
}


void UUIPlaceableTextPropertiesWidget::SetProperties(FColor BaseColor, FString Text) {

	if (BaseTextInputWidget) {
		BaseTextInputWidget->SetText(FText::AsCultureInvariant(Text));
	}
	if (BaseColorRSpinBoxWidget) {
		BaseColorRSpinBoxWidget->SetValue(BaseColor.R);
	}
	if (BaseColorGSpinBoxWidget) {
		BaseColorGSpinBoxWidget->SetValue(BaseColor.G);
	}
	if (BaseColorBSpinBoxWidget) {
		BaseColorBSpinBoxWidget->SetValue(BaseColor.B);
	}

	SetSaveEnabled(true);
	SetResetEnabled(true);
}

FColor UUIPlaceableTextPropertiesWidget::GetBaseColor() const {
	FColor BaseColor = FColor::Black;

	if (BaseColorRSpinBoxWidget) {
		BaseColor.R = BaseColorRSpinBoxWidget->GetValue();
	}
	if (BaseColorGSpinBoxWidget) {
		BaseColor.G = BaseColorGSpinBoxWidget->GetValue();
	}
	if (BaseColorBSpinBoxWidget) {
		BaseColor.B = BaseColorBSpinBoxWidget->GetValue();
	}

	return BaseColor;
}

FString UUIPlaceableTextPropertiesWidget::GetBaseText() const {
	FString BaseText = "Text";

	if (BaseTextInputWidget) {
		BaseText = BaseTextInputWidget->GetText().ToString();
	}

	return BaseText;
}
