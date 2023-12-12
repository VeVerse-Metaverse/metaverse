// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIPlaceableShapePropertiesWidget.h"

#include "Components/SpinBox.h"


void UUIPlaceableShapePropertiesWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (BaseColorRSpinBoxWidget) {
		BaseColorRSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableShapePropertiesWidget::SpinBox_OnValueChanged);
	}
	if (BaseColorGSpinBoxWidget) {
		BaseColorGSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableShapePropertiesWidget::SpinBox_OnValueChanged);
	}
	if (BaseColorBSpinBoxWidget) {
		BaseColorBSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableShapePropertiesWidget::SpinBox_OnValueChanged);
	}

	if (MetallicSpinBoxWidget) {
		MetallicSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableShapePropertiesWidget::SpinBox_OnValueChanged);
	}
	if (RoughnessSpinBoxWidget) {
		RoughnessSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIPlaceableShapePropertiesWidget::SpinBox_OnValueChanged);
	}

	LockChanged = true;
}

void UUIPlaceableShapePropertiesWidget::NativeConstruct() {
	Super::NativeConstruct();
	LockChanged = false;
}

void UUIPlaceableShapePropertiesWidget::SpinBox_OnValueChanged(float InValue) {
	if (!LockChanged) {
		NativeOnChanged();
	}
}

void UUIPlaceableShapePropertiesWidget::SetProperties(FColor BaseColor, float Metallic, float Roughness) {
	if (BaseColorRSpinBoxWidget) {
		BaseColorRSpinBoxWidget->SetValue(BaseColor.R);
	}
	if (BaseColorGSpinBoxWidget) {
		BaseColorGSpinBoxWidget->SetValue(BaseColor.G);
	}
	if (BaseColorBSpinBoxWidget) {
		BaseColorBSpinBoxWidget->SetValue(BaseColor.B);
	}

	if (MetallicSpinBoxWidget) {
		MetallicSpinBoxWidget->SetValue(Metallic);
	}
	if (RoughnessSpinBoxWidget) {
		RoughnessSpinBoxWidget->SetValue(Roughness);
	}

	SetSaveEnabled(true);
	SetResetEnabled(true);
}

FColor UUIPlaceableShapePropertiesWidget::GetBaseColor() const {
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

float UUIPlaceableShapePropertiesWidget::GetMetallic() const {
	if (MetallicSpinBoxWidget) {
		return MetallicSpinBoxWidget->GetValue();
	}
	return 0.0f;
}

float UUIPlaceableShapePropertiesWidget::GetRoughness() const {
	if (RoughnessSpinBoxWidget) {
		return RoughnessSpinBoxWidget->GetValue();
	}
	return 0.5f;
}
