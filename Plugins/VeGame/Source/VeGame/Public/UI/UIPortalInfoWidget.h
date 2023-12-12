// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Components/UIButtonWidget.h"
#include "UIProgressWidget.h"
#include "UIWidgetBase.h"
#include "VePortal.h"
#include "Components/TextBlock.h"
#include "UIPortalInfoWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIPortalInfoWidget : public UUIWidgetBase {
	GENERATED_BODY()

	DECLARE_EVENT(UUIPortalInfoWidget, FUIPortalInfoWidgetEvent);

public:
	void UpdateName(const FString& InName) const;
	void UpdateNameColor(FLinearColor InColor) const;
	void UpdateStateColor(FLinearColor InColor) const;
	void UpdateState(EVePortalState InState) const;
	void UpdateLoadingProgress(float InPercent) const;

	void UpdateDownloadSize(int64 InDownloadSize);

	void ShowLoadingProgress() const;
	void HideLoadingProgress() const;
	void ShowDownloadButton() const;
	void HideDownloadButton() const;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float LoadingProgress;

	/** "Portal to" text widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* PortalTextWidget;

	/** Portal name widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* NameTextWidget;

	/** Portal status widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* StateTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UButton* DownloadButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* ConfirmButtonTextBlock;

	/** Loading progress indicator. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIProgressWidget* LoadingProgressWidget;

	FUIPortalInfoWidgetEvent& GetOnDownloadClicked() { return OnDownloadClicked; }

protected:
	virtual void NativeOnInitialized() override;

private:
	FUIPortalInfoWidgetEvent OnDownloadClicked;

	UFUNCTION()
	void DownloadButton_OnClicked();

};
