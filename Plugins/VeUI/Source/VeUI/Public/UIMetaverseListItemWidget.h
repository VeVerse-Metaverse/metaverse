// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"

#include "Blueprint/IUserObjectListEntry.h"
#include "ApiPackageMetadata.h"
#include "UIMetaverseListItemWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class UUIMetaverseListItemWidget final : public UUIWidgetBase, public IUserObjectListEntry {
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
	class UTextBlock* AuthorTextWidget;

	/** Displays the mod price. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* PriceTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* RatingTextWidget;

	/** Displays the download size. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* DownloadSizeTextWidget;

	/** Displays total count of downloads. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* DownloadCountTextWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUILoadingWidget* LoadingWidget;

	
	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;
	
protected:
	virtual void NativeConstruct() override;
	//virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	/** Called when the tile view releases this widget. */
	virtual void NativeOnEntryReleased() override;

	void SetIsLoading(bool bInIsLoading) const;
	//void SetTexture(UTexture2D* Texture);

	TSharedPtr<FApiPackageMetadata> Metadata;

};
