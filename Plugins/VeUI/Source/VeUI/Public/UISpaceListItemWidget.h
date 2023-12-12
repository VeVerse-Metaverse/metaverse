// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "VeWorldMetadata.h"
#include "UISpaceListItemWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUISpaceListItemWidget : public UUIWidgetBase, public IUserObjectListEntry {
	GENERATED_BODY()

public:
#pragma region Widgets

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageWidget* PreviewImageWidget;

	/** Displays space name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* NameTextWidget;

	/** Displays owner name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* OwnerTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* RatingTextWidget;

#pragma endregion Widgets

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	FVeWorldMetadata Metadata;

};
