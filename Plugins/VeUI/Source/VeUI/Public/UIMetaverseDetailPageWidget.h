// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPageContentWidget.h"
#include "ApiPackageMetadata.h"
#include "DownloadChunkSubsystem.h"
#include "UIMetaverseDetailPageWidget.generated.h"

class UTextBlock;
class UImage;
class UUIButtonWidget;
class UWrapBox;

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIMetaverseDetailPageWidget final : public UUIPageContentWidget  {
	GENERATED_BODY()

public:
#pragma region Widgets

	/** Displays the mod name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* TitleTextWidget;

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageWidget* PreviewImageWidget;

	/** Displays the mod price. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* PriceTextWidget;

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

#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* PreviewImageButtonWidget;

	/** Displays total count of downloads. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* InstallButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* DeleteButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Preview Page"))
	TSubclassOf<class UUIPreviewImagePageWidget> PreviewPageClass;

protected:
	virtual void NativeOnInitialized() override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeOnRefresh() override;
	//virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	FApiPackageMetadata Metadata;

	void MetadataRequest(const FGuid& InId);
	void OnMetadataRequestCompleted(const FApiPackageMetadata& InMetadata, const bool bSuccessful, const FString& Error);
	void SetMetadata(const FApiPackageMetadata& InMetadata);
	void SetTexture(UTexture2D* Texture);

	void OnDeleteButtonClicked();
	void DeleteMetaverse();
	void OnInstallButtonClicked();
	void UpdateButtons();

	bool GetPakFileExists();
	bool GetPakMetadataExists();

	bool bMetaversePakExists = false;
	bool bIsProcessing = false;
	FDateTime LastProgressTime;

	FString GetMetaversePakPath() const;
	FApiFileMetadata* GetMetaversePakFileMetadata();

	void OnMetaversePakDownloadProgress(const FChunkDownloadProgress& Result);
	void OnMetaversePakDownloadComplete(const FFileDownloadResult& Result);

private:
	FDelegateHandle OnPreviewButtonClickedDelegateHandle;
	FDelegateHandle OnDeleteButtonWidgetDelegateHandle;
	FDelegateHandle OnInstallButtonWidgetDelegateHandle;

	FDownloadSubscriptionPtr BoundDownload;
};
