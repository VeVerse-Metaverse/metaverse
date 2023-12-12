// Author: Egor A. Pristavka
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIWidgetBase.h"
#include "VePlaceableMetadata.h"
#include "UIEditorAssetBrowserItemDragWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIEditorAssetBrowserItemDragWidget final : public UUIWidgetBase {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVePlaceableClassMetadata PlaceableClass;

	UFUNCTION(BlueprintCallable)
	void SetPlaceableClass(const FVePlaceableClassMetadata& InPlaceableClass) { PlaceableClass = InPlaceableClass; }

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIImageWidget* PreviewImageWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* DescriptionTextWidget;

};
