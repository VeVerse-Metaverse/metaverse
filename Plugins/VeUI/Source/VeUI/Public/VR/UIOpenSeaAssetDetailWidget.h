// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiObjectMetadata.h"
#include "ManagedTextureRequester.h"
#include "OpenSeaAssetMetadata.h"
#include "Components/UIButtonWidget.h"
#include "UIWidgetBase.h"

#include "VR/UIZoomImageWidget.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "UIOpenSeaAssetDetailWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIOpenSeaAssetDetailWidget : public UUIWidgetBase
, public IManagedTextureRequester {
	GENERATED_BODY()

protected:
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeConstruct() override;

	FDelegateHandle OnCloseButtonWidgetDelegateHandle;
	FDelegateHandle OnObjectEditButtonClickedDelegateHandle;
	FDelegateHandle OnObjectDeleteButtonClickedDelegateHandle;
	FDelegateHandle OnObjectDeletedDelegateHandle;
	FDelegateHandle OnPreviewButtonClickedDelegateHandle;
	FDelegateHandle OnZoomImageWidgetClosedDelegateHandle;

	// void OnObjectFormEditButtonClicked() const;
	// void OnObjectDeleteButtonClicked();
	// void OnObjectDeleted(const bool bSuccessful, const FString& Error) const;

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

public:
	/** Metadata of the widget to display details of. */
	FOpenSeaAssetMetadata Metadata;

	void SetMetadata(const FOpenSeaAssetMetadata& InMetadata);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* EditButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* PreviewImageButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* DeleteButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* MarkdownTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UScaleBox* ScaleImageBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIZoomImageWidget* ZoomImageWidget;

	DECLARE_EVENT(UUIObjectDetailWidget, FOnClosed);
	FOnClosed OnClosed;

#pragma region Object Fields
	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* PreviewImageWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* DescriptionTextWidget;

#pragma endregion

private:
	bool bZoomImageWidgetVisible;

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideZoomImageWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowZoomImageWidget(bool bAnimate = true, bool bPlaySound = true);
};
