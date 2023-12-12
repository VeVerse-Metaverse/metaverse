// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIPageContentWidget.h"
#include "UIPlaceableFilePageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIPlaceableFilePageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIInputWidget* FileInputWidget = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonWidget* OpenFileButtonWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonWidget* CloseButtonWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonWidget* SubmitButtonWidget;

#pragma endregion Widgets
	
	DECLARE_EVENT(UUIPlaceableFilePageWidget, FOnClosed);

	DECLARE_EVENT_TwoParams(UUIPlaceableFilePageWidget, FOnFileChanged, const FString& FileName, const FString& FileType);

	FOnFileChanged OnFileChanged;

	FOnClosed OnClosedDelegate;

protected:
	virtual void NativeOnInitialized() override;

	FString FilePath;
	FString FileType;

	void SubmitButton_OnClicked();
	void OpenFileButton_OnClicked();

};
