// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "UI/UIPlaceablePropertiesWidget.h"
#include "UIPlaceableShapePropertiesWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIPlaceableShapePropertiesWidget : public UUIPlaceablePropertiesWidget {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* BaseColorRSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* BaseColorGSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* BaseColorBSpinBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* MetallicSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* RoughnessSpinBoxWidget;

	void SetProperties(FColor BaseColor, float Metallic, float Roughness);
	FColor GetBaseColor() const;
	float GetMetallic() const;
	float GetRoughness() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SpinBox_OnValueChanged(float InValue);

private:
	bool LockChanged = false;
	
};
