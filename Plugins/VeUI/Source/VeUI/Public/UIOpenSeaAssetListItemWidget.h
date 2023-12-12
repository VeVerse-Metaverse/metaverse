// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include <ManagedTextureRequester.h>

#include "ApiOpenSeaMetadataObject.h"
#include "UIWidgetBase.h"

#include "Blueprint/IUserObjectListEntry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "OpenSeaAssetMetadata.h"
#include "Components/UILoadingWidget.h"
#include "Components/SizeBox.h"
#include "UIOpenSeaAssetListItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIOpenSeaAssetListItemWidget final : public UUIWidgetBase, public IUserObjectListEntry  {
	GENERATED_BODY()

	void SetIsLoading(bool bInIsLoading) const;

	//virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;
	//void SetTexture(UTexture2D* Texture);

public:
	virtual void NativeConstruct() override;

	/** Called when the tile view releases this widget. */
	virtual void NativeOnEntryReleased() override;
	void SetOwnerAddress(const FString& InOwnerAddress);

	/** Called when the tile view sending the metadata object to be used in this widget. */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	FApiOpenSeaAssetMetadata ObjectMetadata;

	FString ImageURL;

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	USizeBox* PreviewImageBoxWidget;

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIImageWidget* PreviewImageWidget;

	/** Displays object name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* AuthorTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* RatingTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUILoadingWidget* LoadingWidget;
};
