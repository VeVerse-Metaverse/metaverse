// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPageContentWidget.h"
#include "ApiEventMetadata.h"
#include "DownloadChunkSubsystem.h"
#include "UIEventDetailPageWidget.generated.h"

class UTextBlock;
class UImage;
class UUIButtonWidget;
class UWrapBox;

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIEventDetailPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	/** Displays the mod name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* TitleTextWidget;

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageButtonWidget* PreviewImageWidget;

	/** Displays the mod price. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* PriceTextWidget;

	/** Displays the mod author's name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* SummaryTextWidget;

	/** Displays the mod author's name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* DescriptionTextWidget;

	/** Displays the mod author's name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* OwnerTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* StartsDateTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* EndsDateTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* ViewCountTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* LikeCountTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* DislikeCountTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* RatingTextWidget;

#pragma endregion Widgets

#pragma region Buttons

	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* PreviewImageButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* VisitButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Preview Page"))
	TSubclassOf<class UUIPreviewImagePageWidget> PreviewPageClass;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnRefresh() override;

	FApiEventMetadata Metadata;

	void MetadataRequest(const FGuid& InId);
	void OnMetadataRequestCompleted(const FApiEventMetadata& InMetadata, const bool bSuccessful, const FString& Error);
	void SetMetadata(const FApiEventMetadata& InMetadata);

	void OnVisitButtonClicked();

	bool GetPakFileExists();
	bool GetPakMetadataExists();

	bool bEventPakExists = false;
	bool bIsProcessing = false;
	FDateTime LastProgressTime;

	FString GetEventPakPath() const;
	FApiFileMetadata* GetEventPakFileMetadata();

	void OnEventPakDownloadProgress(const FChunkDownloadProgress& Result);
	void OnEventPakDownloadComplete(const FFileDownloadResult& Result);
};
