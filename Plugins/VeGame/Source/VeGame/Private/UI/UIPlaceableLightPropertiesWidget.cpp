// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIPlaceableLightPropertiesWidget.h"

#include "Components/SpinBox.h"


void UUIPlaceableLightPropertiesWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (BaseColorRSpinBoxWidget) {
		BaseColorRSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableLightPropertiesWidget::SpinBox_OnValueChanged);
	}
	if (BaseColorGSpinBoxWidget) {
		BaseColorGSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableLightPropertiesWidget::SpinBox_OnValueChanged);
	}
	if (BaseColorBSpinBoxWidget) {
		BaseColorBSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableLightPropertiesWidget::SpinBox_OnValueChanged);
	}
	if (IntensitySpinBoxWidget) {
		IntensitySpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableLightPropertiesWidget::SpinBox_OnValueChanged);
	}
	if (AttenuationRadiusSpinBoxWidget) {
		AttenuationRadiusSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableLightPropertiesWidget::SpinBox_OnValueChanged);
	}

	LockChanged = true;
}

void UUIPlaceableLightPropertiesWidget::NativeConstruct() {
	Super::NativeConstruct();
	LockChanged = false;
}

void UUIPlaceableLightPropertiesWidget::SpinBox_OnValueChanged(float InValue) {
	if (!LockChanged) {
		NativeOnChanged();
	}
}

void UUIPlaceableLightPropertiesWidget::SetProperties(FColor BaseColor, float Intensity, float AttenuationRadius) {
	if (BaseColorRSpinBoxWidget) {
		BaseColorRSpinBoxWidget->SetValue(BaseColor.R);
	}
	if (BaseColorGSpinBoxWidget) {
		BaseColorGSpinBoxWidget->SetValue(BaseColor.G);
	}
	if (BaseColorBSpinBoxWidget) {
		BaseColorBSpinBoxWidget->SetValue(BaseColor.B);
	}
	if (IntensitySpinBoxWidget) {
		IntensitySpinBoxWidget->SetValue(Intensity);
	}
	if (AttenuationRadiusSpinBoxWidget) {
		AttenuationRadiusSpinBoxWidget->SetValue(AttenuationRadius);
	}

	SetSaveEnabled(true);
	SetResetEnabled(true);
}

FColor UUIPlaceableLightPropertiesWidget::GetBaseColor() const {
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

float UUIPlaceableLightPropertiesWidget::GetIntensity() const {
	if (IntensitySpinBoxWidget) {
		return IntensitySpinBoxWidget->GetValue();
	}
	return 5000.0f;
}

float UUIPlaceableLightPropertiesWidget::GetAttenuationRadius() const {
	if (AttenuationRadiusSpinBoxWidget) {
		return AttenuationRadiusSpinBoxWidget->GetValue();
	}
	return 1000.0f;
}
