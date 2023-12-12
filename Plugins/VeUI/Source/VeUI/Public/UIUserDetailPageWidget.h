// Author: Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIPageContentWidget.h"
#include "ApiUserMetadata.h"
#include "UIUserDetailPageWidget.generated.h"

class UTextBlock;
class UImage;
class UUIButtonWidget;

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIUserDetailPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	/** Displays the space name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageButtonWidget* PreviewImageWidget;

	/** Displays the space description. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* DescriptionTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* ViewCountTextWidget;

	/** Displays likes. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* LikeCountTextWidget;

	/** Displays dislikes. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* DislikeCountTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* RatingTextWidget;

	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUILoadingWidget* LoadingWidget;

#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* FollowButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* UnfollowButtonWidget;

	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* FollowersButtonWidget;
	//
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* FollowingButtonWidget;
	//
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* MuteButtonWidget;
	//
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* UnmuteButtonWidget;
	// 	
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* DeactivateButtonWidget;
	// 		
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* ActivateButtonWidget;
	// 			
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* BlockButtonWidget;
	// 				
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* UnblockButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Preview Page"))
	TSubclassOf<class UUIPreviewImagePageWidget> PreviewPageClass;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnRefresh() override;

	FApiUserMetadata Metadata;

	void MetadataRequest(const FGuid& InId);
	void OnMetadataRequestCompleted(const FApiUserMetadata& InMetadata, const bool bSuccessful, const FString& Error);
	void SetMetadata(const FApiUserMetadata& InMetadata);

	void UpdateButtons();
	void FollowButton_OnButtonClicked();
	void OnFollowRequestCompleted(bool bInSuccessful, const FString& InError);
	void UnfollowButton_OnButtonClicked();
	void OnUnfollowRequestCompleted(bool bInSuccessful, const FString& InError);
	void RequestIsFollowing();
	void OnIsFollowingRequestCompleted(bool bInOk, bool bInSuccessful, const FString& InError);

private:
	bool bIsFollowing = false;
	bool bIsCurrentUser = false;
};
