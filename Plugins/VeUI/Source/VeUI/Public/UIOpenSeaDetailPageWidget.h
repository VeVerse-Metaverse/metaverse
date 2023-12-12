// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Api2Request.h"
#include "ApiOpenSeaMetadata.h"
#include "UIPageContentWidget.h"
#include "ManagedTextureRequester.h"
#include "UIOpenSeaDetailPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIOpenSeaDetailPageWidget : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIImageWidget* PreviewImageWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* MarkdownTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UScaleBox* ScaleImageBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* DescriptionTextWidget;

#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonWidget* PreviewImageButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* OpenPageButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Preview Page"))
	TSubclassOf<class UUIPreviewImagePageWidget> PreviewPageClass;
	
	void SetMetadata(const FApiOpenSeaAssetMetadata& InMetadata);

protected:
	virtual void NativeOnInitialized() override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeOnRefresh() override;
	virtual void NativeConstruct() override;
	//virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	/** Metadata of the widget to display details of. */
	FApiOpenSeaAssetMetadata Metadata;
	FString ImageURL;

	void MetadataRequest(const FGuid& InId);
	void OnMetadataResponse(const FApiOpenSeaAssetMetadata& InMetadata,const EApi2ResponseCode InResponseCode, const FString& InError);
	
	//void SetTexture(UTexture2D* Texture);

private:
	FDelegateHandle OnPreviewButtonClickedDelegateHandle;

};
