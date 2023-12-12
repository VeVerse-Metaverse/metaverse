// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"

#include "ManagedTextureRequester.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ApiEventMetadata.h"
#include "UIEventListItemWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class UUIEventListItemWidget final : public UUIWidgetBase, public IUserObjectListEntry {
	GENERATED_BODY()

public:
	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageWidget* PreviewImageWidget;

	/** Displays the mod name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* TitleTextWidget;

	/** Displays the mod author's name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* OwnerTextWidget;

	/** Displays the mod price. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* PriceTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* StartsDateTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* EndsDateTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* RatingTextWidget;

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	TSharedPtr<FApiEventMetadata> Metadata;

};
