// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiObjectMetadata.h"
#include "ManagedTextureRequester.h"
#include "Components/UIButtonWidget.h"
#include "UIWidgetBase.h"
#include "VR/UIZoomImageWidget.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "UIObjectDetailWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIObjectDetailWidget : public UUIWidgetBase
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

	void OnObjectFormEditButtonClicked() const;
	void OnObjectDeleteButtonClicked();
	void OnObjectDeleted(const bool bSuccessful, const FString& Error) const;

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

public:
	/** Metadata of the widget to display details of. */
	TSharedPtr<FApiObjectMetadata> Metadata;

	void SetMetadata(TSharedPtr<FApiObjectMetadata> InMetadata);
	void RequestObjectDeleteConfirmation();
	void DeleteObject();

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

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UScaleBox* ScaleImageBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIZoomImageWidget* ZoomImageWidget;

	DECLARE_EVENT(UUIObjectDetailWidget, FOnClosed);
	FOnClosed OnClosed;

	DECLARE_EVENT_OneParam(UUIObjectDetailWidget, FOnEditButtonClicked, TSharedPtr<FApiObjectMetadata>);
	FOnEditButtonClicked OnEditButtonClicked;

	DECLARE_EVENT(UUIObjectDetailWidget, FOnDeleteButtonClicked);
	FOnDeleteButtonClicked OnDeleteButtonClicked;

#pragma region Object Fields
	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UImage* PreviewImageWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* ViewCountTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* TypeTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* ArtistTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* DateTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* DescriptionTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* MediumTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* SourceTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* SourceUrlTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* LicenseTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* CopyrightTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* CreditTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* OriginTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* LocationTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* WidthTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* HeightTextWidget;

#pragma endregion

private:
	bool bZoomImageWidgetVisible;

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideZoomImageWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowZoomImageWidget(bool bAnimate = true, bool bPlaySound = true);
};
