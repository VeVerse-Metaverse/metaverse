// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIPageContentWidget.h"
#include "Components/UIInputWidget.h"
#include "UISettingsPageWidget.generated.h"

class UUIButtonWidget;
class UComboBoxString;
class UUICommonOptionSwitcherWidget;
class UUICommonOptionButtonsWidget;
class UUICommonOptionSliderWidget;


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUISettingsPageWidget : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonOptionSliderWidget* ResolutionScaleOptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonOptionButtonsWidget* OverallQualityOptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonOptionSwitcherWidget* ViewDistanceOptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonOptionSwitcherWidget* AntiAliasingOptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonOptionSwitcherWidget* PostProcessingOptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonOptionSwitcherWidget* ShadowsOptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonOptionSwitcherWidget* TexturesOptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonOptionSwitcherWidget* EffectsOptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonOptionSwitcherWidget* FoliageOptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonOptionSwitcherWidget* ShadingOptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* ChangeNameWidget;

#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* ClearDownloadCacheButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CancelButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* SubmitButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ChangeNameButtonWidget;

#pragma endregion Buttons

protected:
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeOnRefresh() override;
	virtual void NativeOnInitializedShared() override;

	void OnSubmitButtonClicked();
	void OnOverallQualityOptionChanged();
	void OnClearDownloadCacheClicked();

private:
	FDelegateHandle OnCancelButtonClickedDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedDelegateHandle;
	FDelegateHandle OnOverallQualityOptionClickedDelegateHandle;
	FDelegateHandle OnClearDownloadCacheClickedDelegateHandle;

};
