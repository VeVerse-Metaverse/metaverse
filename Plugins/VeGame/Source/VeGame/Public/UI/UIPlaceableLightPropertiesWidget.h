// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "UI/UIPlaceablePropertiesWidget.h"
#include "UIPlaceableLightPropertiesWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIPlaceableLightPropertiesWidget : public UUIPlaceablePropertiesWidget {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* BaseColorRSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* BaseColorGSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* BaseColorBSpinBoxWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* IntensitySpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* AttenuationRadiusSpinBoxWidget;

	void SetProperties(FColor BaseColor, float Intensity, float AttenuationRadius);
	FColor GetBaseColor() const;
	float GetIntensity() const;
	float GetAttenuationRadius() const;


protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SpinBox_OnValueChanged(float InValue);

private:
	bool LockChanged = false;
	
};
