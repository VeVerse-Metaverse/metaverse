// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ApiObjectMetadata.h"
#include "Components/RichTextBlock.h"
#include "Components/UIFormInputWidget.h"
#include "UI/UIPlaceablePropertiesWidget.h"
#include "UIInspectorOpenSeaAssetPropertiesWidget.generated.h"


class FApiOpenSeaAssetMetadata;
/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIInspectorOpenSeaAssetPropertiesWidget : public UUIPlaceablePropertiesWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* NameWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* DescriptionWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUITextPropertyWidget* OwnerTextPropertyWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* OpenPageButtonWidget;

#pragma endregion Widgets

	void SetProperties(const FApiOpenSeaAssetMetadata& InMetadata);

protected:
	virtual void NativeOnInitialized() override;

};
