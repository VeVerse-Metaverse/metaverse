// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIPageContentWidget.h"
#include "UIAboutPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIAboutPageWidget : public UUIPageContentWidget
{
	GENERATED_BODY()

public:
#pragma region Widgets
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* TitleTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* ProjectNameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* PtojectVersionTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonWidget* WebsiteLinkButtonWidget;

#pragma endregion Widgets

protected:
	virtual void NativeOnInitialized() override;

	FString GetCopyright() const;
	FString GetProjectVersion() const;
	FString GetProjectHomepage() const;
	FString GetProjectSupportContact() const;
	
};
