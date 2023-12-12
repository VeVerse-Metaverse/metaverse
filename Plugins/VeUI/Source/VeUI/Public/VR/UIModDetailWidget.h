// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiPackageMetadata.h"
#include "DownloadChunkSubsystem.h"
#include "ManagedTextureRequester.h"
#include "Components/UIButtonWidget.h"
#include "UIDialogWidget.h"
#include "UIModBrowserWidget.h"
#include "UIWidget.h"
#include "UIZoomImageWidget.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

#include "UIModDetailWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIModDetailWidget final : public UUIWidget, public IManagedTextureRequester {
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	FDelegateHandle OnCloseButtonWidgetDelegateHandle;
	FDelegateHandle OnPreviewButtonClickedDelegateHandle;
	FDelegateHandle OnZoomImageWidgetClosedDelegateHandle;
	FDelegateHandle OnInstallButtonWidgetDelegateHandle;
	FDelegateHandle OnDeleteButtonWidgetDelegateHandle;
	FDelegateHandle OnVisitButtonWidgetDelegateHandle;
	FDelegateHandle OnHostButtonWidgetDelegateHandle;
	FDelegateHandle OnJoinButtonWidgetDelegateHandle;

	void InstallModButtonClicked();
	void DeleteModButtonClicked();
	void VisitModButtonClicked();
	void HostModButtonClicked();
	void JoinModButtonClicked();
	// void OpenModButtonClicked();
	void UpdateButtons();
	void RequestObjectDeleteConfirmation();

	bool GetPakFileExists();
	bool GetPakMetadataExists();

	bool bModPakFileExists = false;
	bool bIsProcessing = false;
	FDateTime LastProgressTime;

	FString GetModPakPath();
	FApiFileMetadata* GetModPakFileMetadata();

	void OnModFileDownloadProgress(const FChunkDownloadProgress& Result);
	void OnModFileDownloadComplete(const FFileDownloadResult& Result);

public:
	void SetMetadata(TSharedPtr<FApiPackageMetadata> InMod);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* PreviewImageButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UScaleBox* ScaleImageBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIZoomImageWidget* ZoomImageWidget;

	TSharedPtr<FApiPackageMetadata> Metadata;

	DECLARE_EVENT(UUIModDetailWidget, FOnClosed);

	FOnClosed OnClosed;

	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* OpenButtonWidget;

#pragma region Mod Fields
	/** Displays the mod name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* NameTextWidget;

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* PreviewImageWidget;

	/** Displays the mod price. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* PriceTextWidget;

	/** Displays total count of downloads. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* InstallButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* DeleteButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* VisitButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* HostButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* JoinButtonWidget;

	/** Displays the mod author's name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* DescriptionTextWidget;

	/** Displays the mod author's name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* AuthorTextWidget;

	/** Displays the mod author's name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UWrapBox* TagContainerWidget;

	/** Displays the mod author's name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UWrapBox* PlatformContainerWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* VersionTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* ReleaseDateTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* ViewCountTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* DownloadCountTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* LikeCountTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* DislikeCountTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* RatingTextWidget;

	/** Displays the download size. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* DownloadSizeTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* MarkdownTextWidget;

#pragma endregion

private:
	FDownloadSubscriptionPtr BoundDownload;
	bool bZoomImageWidgetVisible;

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideZoomImageWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowZoomImageWidget(bool bAnimate = true, bool bPlaySound = true);
};
