// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Api2Request.h"
#include "UIPageContentWidget.h"
#include "ApiObjectMetadata.h"
#include "UIObjectDetailPageWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIObjectDetailPageWidget : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageButtonWidget* PreviewImageWidget;
	
	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* ViewCountTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* DescriptionTextWidget;

	/** Displays the owner name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* OwnerTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* TypeTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* ArtistTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* DateTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* MediumTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* SourceTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* SourceUrlTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* LicenseTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* CopyrightTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* CreditTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* OriginTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* LocationTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* WidthTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* HeightTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* LikeCountTextWidget;
	
	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* DislikeCountTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* RatingTextWidget;

#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonWidget* EditButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonImageWidget* LikeButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonImageWidget* DisLikeButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonWidget* DeleteButtonWidget;

#pragma endregion Buttons

	/** Form Page Class for edit a object. */
	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Form Page"))
	TSubclassOf<class UUIObjectFormPageWidget> FormPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Preview Page"))
	TSubclassOf<class UUIPreviewImagePageWidget> PreviewPageClass;
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnRefresh() override;

	FApiObjectMetadata Metadata;

	void MetadataRequest(const FGuid& InId);
	void OnMetadataRequestCompleted(const FApiObjectMetadata& InMetadata,  const EApi2ResponseCode InResponseCode, const FString& Error);
	void SetMetadata(const FApiObjectMetadata& InMetadata);
	void UpdateButtons();

	void OnDeleteButtonClicked();
	void DeleteRequest();
	void OnDeleteRequestCompleted(const EApi2ResponseCode InResponseCode, const FString& Error);

private:
	bool UserIsOwner = false;
	bool bIsPressLiked = false;
	bool bIsPressDisLiked = false;
	
};
