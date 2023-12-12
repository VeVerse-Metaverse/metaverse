// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.
#pragma once


#include "UIPageContentWidget.h"
#include "ManagedTextureRequester.h"
#include "UIInspectorImagePreviewWidget.generated.h"

class UImage;
class UUIButtonWidget;


/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIInspectorImagePreviewWidget : public UUIWidgetBase, public IManagedTextureRequester {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UImage* PreviewImageWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUILoadingWidget* LoadingWidget;

#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ApproximationButton;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* DistanceButton;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

	UPROPERTY(EditAnywhere, Category="Widget")
	FVector2D DefaultSize = {1920, 1080};

	int MaxZoom = 10;
	int MinZoom = -4;
	int CurrentZoomStep = 0;
	float ZoomFactor = 0.8f;
	float PanSpeed = 1.f;
	bool CanMoveImage = true;
	bool IsDraggingImage = false;

	void ZoomObject(FGeometry InGeometry, FPointerEvent InMouseEvent);
	void ScaleImage(bool bInZoom);
	void ZoomReset();

	void SetUrl(const FString& InUrl);

protected:
	// virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	void NativeOnRefresh();
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	FString URL;

	void SetIsProcessing(const bool bInIsLoading) const;

	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	FDelegateHandle OnApproximationButtonClickedDelegateHandle;
	FDelegateHandle OnDistanceButtonClickedDelegateHandle;

};
