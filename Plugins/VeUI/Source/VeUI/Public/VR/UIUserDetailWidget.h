// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiOkMetadata.h"
#include "ApiUserMetadata.h"
#include "ManagedTextureRequester.h"
#include "Components/UIButtonWidget.h"
#include "Components/UILoadingWidget.h"
#include "UIWidget.h"
#include "UIZoomImageWidget.h"
#include "Components/ComboBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "Components/TextBlock.h"
#include "UIUserDetailWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIUserDetailWidget final : public UUIWidget, public IManagedTextureRequester {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void NativeConstruct() override;

	//void SetIsLoading(bool bInIsLoading) const;

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;
	void UpdateButtons();
	void FollowButtonClicked();
	void UnfollowButtonClicked();
	void OnFollowRequestCompleted(bool bInSuccessful, const FString& InError);
	void UnfollowRequestCompleted(bool bInSuccessful, const FString& InError);
	void RequestIsFollowing();
	void OnIsFollowingRequestCompleted(bool bInOk, bool bInSuccessful, const FString& InError);

	FDelegateHandle OnCloseButtonWidgetDelegateHandle;
	FDelegateHandle OnFollowButtonWidgetDelegateHandle;
	FDelegateHandle OnUnfollowButtonWidgetDelegateHandle;
	FDelegateHandle OnPreviewButtonClickedDelegateHandle;
	FDelegateHandle OnZoomImageWidgetClosedDelegateHandle;
	FDelegateHandle OnAddFileLinkWidgetClosedDelegateHandle;
	FDelegateHandle OnEditButtonClickedClosedDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedClosedDelegateHandle;


public:
	void SetMetadata(TSharedPtr<FApiUserMetadata> InUser);

	TSharedPtr<FApiUserMetadata> Metadata;

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;
	
#pragma region Buttons
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* EditButtonWidget;

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

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UComboBoxString* ComboboxWidget;
	
#pragma endregion
	DECLARE_EVENT(UUIUserDetailWidget, FOnClosed);

	FOnClosed OnClosed;

	/** Displays the space name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* NameTextWidget;

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* PreviewImageWidget;

	/** Displays the space description. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* DescriptionTextWidget;

	/** Displays average rating (1 to 5). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* ViewCountTextWidget;
	
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIRPMAvatarLinkWidget* RPMAvatarLinkWidget;
	
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

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIZoomImageWidget* ZoomImageWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UScaleBox* ScaleImageBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* PreviewImageButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* MarkdownTextWidget;

private:
	bool bZoomImageWidgetVisible;
	//bool bFileLinkWidgetVisible;
	bool bIsFollowing = false;

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideZoomImageWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowZoomImageWidget(bool bAnimate = true, bool bPlaySound = true);
	
	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void HideFileLinkWidget(bool bAnimate = true, bool bPlaySound = true);
	//
	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void ShowFileLinkWidget(bool bAnimate = true, bool bPlaySound = true);

	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void NavigateToAddFileLink(bool bAnimate = true, bool bPlaySound = true);
	
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToUserDetail(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToZoomImage(bool bAnimate = true, bool bPlaySound = true);

	void SetTexture(UTexture2D* Texture);
};
