// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "UI/UIPlaceablePropertiesWidget.h"
#include "UIPlaceableTextPropertiesWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIPlaceableTextPropertiesWidget : public UUIPlaceablePropertiesWidget {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* BaseColorRSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	USpinBox* BaseColorGSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	USpinBox* BaseColorBSpinBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIInputWidget* BaseTextInputWidget;

	void SetProperties(FColor BaseColor, FString Text);
	FColor GetBaseColor() const;
	FString GetBaseText() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SpinBox_OnValueChanged(float InValue);

private:
	bool LockChanged = false;

};
