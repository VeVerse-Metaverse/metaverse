// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiUserMetadata.h"
#include "ManagedTextureRequester.h"
#include "UIExperienceBarWidget.h"
#include "Components/UILoadingWidget.h"
#include "UIWidgetBase.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UISocialUserCardWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUISocialUserCardWidget : public UUIWidgetBase
, public IManagedTextureRequester {
	GENERATED_BODY()

public:

	/** User avatar border widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UBorder* AvatarBorderWidget;

	/** User avatar image widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* AvatarImageWidget;

	/** User online status widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* StatusTextWidget;

	/** User name widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* NameTextWidget;

	/** User level widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* LevelTextWidget;

	/** Displays user rank. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* RankTextWidget;

	// /** User title text widget. */
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UTextBlock* TitleWidget;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category="Widget|Content")
	bool bShowExperienceBar;

	/** Experience bar root widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIExperienceBarWidget* ExperienceBarWidget;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultAvatar;

	void SetUserMetadata(const FApiUserMetadata& InUserMetadata);
	void Refresh();

protected:
	
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	FApiUserMetadata UserMetadata;

	void RequestAvatars(const IApiBatchRequestMetadata& RequestMetadata = IApiBatchRequestMetadata());
	void OnAvatarsLoaded(const FApiFileBatchMetadata& Metadata, bool bSuccessful, const FString& Error);
	void SetIsProcessing(bool bInIsLoading);
	void SetStatus(EApiPresenceStatus Status);
	void SetTexture(UTexture2D* Texture);	

	
};
