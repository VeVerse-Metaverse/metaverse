// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.
#pragma once

#include "UIWidgetBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ApiObjectMetadata.h"
#include "UIPlaceableArtObjectItemPropertiesWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIPlaceableArtObjectItemPropertiesWidget final : public UUIWidgetBase, public IUserObjectListEntry {
	GENERATED_BODY()

public:
#pragma region Widgets
	
	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageWidget* ImageWidget;

	/** Displays object name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UBorder* MainBorderWidget;

#pragma endregion Widgets

protected:
	virtual void NativeOnInitialized() override;	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	TSharedPtr<FApiObjectMetadata> Metadata;

	UPROPERTY(EditAnywhere, Category="ListItem")
	FLinearColor NormalColor = FLinearColor(0.000000, 0.000000, 0.000000, 0.250000);

	UPROPERTY(EditAnywhere, Category="ListItem")
	FLinearColor SelectedColor = FLinearColor(0.000000, 0.200000, 1.000000, 1.000000);
	
};
