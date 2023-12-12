// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPageContentWidget.h"
#include "ApiSpaceMetadata.h"
#include "VeWorldMetadata.h"
#include "DownloadChunkSubsystem.h"
#include "UISpaceDetailPageWidget.generated.h"

class UTextBlock;
class UImage;
class UUIButtonWidget;


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUISpaceDetailPageWidget : public UUIPageContentWidget {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UUISpaceDetailPageWidget, FOnSpaceClicked, TSharedPtr<FApiSpaceMetadata>);

public:
#pragma region Widgets

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageButtonWidget* PreviewImageWidget;

	/** Displays the space name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* NameTextWidget;

	/** Displays the space description. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* DescriptionTextWidget;

	/** Displays the owner name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* OwnerTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* ViewCountTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* MarkdownTextWidget;

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

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* EditButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* TeleportOnlineButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* TeleportOfflineButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CreateCopyButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* DeleteButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonImageWidget* LikeButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonImageWidget* DisLikeButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

	/** Form Page Class for edit a space. */
	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Form Page"))
	TSubclassOf<class UUISpaceFormPageWidget> FormPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Preview Page"))
	TSubclassOf<class UUIPreviewImagePageWidget> PreviewPageClass;
	
	void SetMetadata(const FVeWorldMetadata& InMetadata);

protected:
	
	virtual void NativeOnInitialized() override;
	virtual void NativeOnRefresh() override;

	FVeWorldMetadata Metadata;

	void MetadataRequest(const FGuid& InId);
	void OnMetadataRequestCompleted(const FApiSpaceMetadata& InMetadata, const bool bSuccessful, const FString& Error);
	void UpdateButtons();
	void DeleteRequest();

	void DeleteButton_OnClicked();


	void SetWorldItem(TSharedPtr<class FGameFrameworkWorldItem> InWorldItem);
	void WorldItem_OnStateChanged(const class FGameFrameworkWorldItem& InWorldItem);

	void OpenWorldOfflineButton_OnClicked();
	void OpenWorldOnlineButton_OnClicked();
	void CreateCopyButton_OnClicked();
	
	FDateTime LastProgressTime;
	bool bModPakFileExists = false;
	bool bIsProcessing = false;

private:
	FDownloadSubscriptionPtr BoundDownload;
	TWeakPtr<class FGameFrameworkWorldItem> WorldItem;

	bool UserIsOwner = false;
	bool bIsPressLiked = false;
	bool bIsPressDisLiked = false;
	bool bIsLiked = false;
	bool bIsDisLiked = false;
};
