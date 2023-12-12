// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ApiObjectMetadata.h"
#include "UIObjectListItemWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIObjectListItemWidget final : public UUIWidgetBase, public IUserObjectListEntry {
	GENERATED_BODY()

public:
#pragma region Widgets

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageWidget* PreviewImageWidget;

	/** Displays object name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* AuthorTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* RatingTextWidget;

#pragma endregion Widgets

protected:
	/** Called when the tile view sending the metadata object to be used in this widget. */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	TSharedPtr<FApiObjectMetadata> Metadata;

};
