// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIWidget.h"
#include "Components/UIButtonWidget.h"
#include "Components/ComboBoxString.h"
#include "UISettingsWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUISettingsWidget : public UUIWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void NativeConstruct() override;

	FDelegateHandle OnCancelButtonClickedDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedDelegateHandle;
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	
	void ChooseQualitySettings();
	
public:

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CancelButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* SubmitButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UComboBoxString* ComboBoxWidget;

	DECLARE_EVENT(UGameUserSettings,FUserSettings)
	FUserSettings UserSettings;

	DECLARE_EVENT(UUIUserAddFileLink, FOnClosed);
	FOnClosed OnClosedDelegate;
	
	DECLARE_EVENT(UUIUserAddFileLink, FOnSubmit);
	FOnSubmit OnSubmitDelegate;
};
