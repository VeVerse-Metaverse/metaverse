// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiSpaceMetadata.h"
#include "Components/UIButtonWidget.h"
#include "UIWidget.h"
#include "Components/Image.h"
#include "UISpaceDetailWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUISpaceDetailWidget : public UUIWidget
{
	GENERATED_BODY()
	
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnCloseButtonWidgetDelegateHandle;
	FDelegateHandle OnSpaceEditButtonClickedDelegateHandle;
	FDelegateHandle OnSpaceVisitButtonClickedDelegateHandle;

	void OnSpaceFormButtonClicked();
	void OnSpaceVisitButtonClicked();
	
protected:
	virtual void NativeConstruct() override;

public:
	void SetMetadata(TSharedPtr<FApiSpaceMetadata> InSpace);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	TSharedPtr<FApiSpaceMetadata> Metadata;

	DECLARE_EVENT(UUISpaceDetailWidget, FOnClosed);
	FOnClosed OnClosed;

	DECLARE_EVENT_OneParam(UUISpaceDetailWidget, FOnSpaceClicked, TSharedPtr<FApiSpaceMetadata>);
	FOnSpaceClicked OnSpaceEditButtonClicked;

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

	// /** Displays average rating (1 to 5). */
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	// UTextBlock* LikeCountTextWidget;
	//
	// /** Displays average rating (1 to 5). */
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	// UTextBlock* DislikeCountTextWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* EditButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* VisitButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* MarkdownTextWidget;
};
