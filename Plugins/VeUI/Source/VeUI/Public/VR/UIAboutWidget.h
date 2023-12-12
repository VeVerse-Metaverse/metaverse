// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIButtonWidget.h"
#include "UIWidget.h"
#include "Components/TextBlock.h"
#include "UObject/Object.h"
#include "UIAboutWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIAboutWidget : public UUIWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* TitleTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* VersionTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* WebsiteLinkButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	DECLARE_EVENT(UUIModDetailWidget, FOnClosed);
	FOnClosed OnClosed;
};
