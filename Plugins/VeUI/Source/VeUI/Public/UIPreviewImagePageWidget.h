// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIPageContentWidget.h"
#include "ManagedTextureRequester.h"
#include "ApiFileMetadata.h"
#include "UIPreviewImagePageWidget.generated.h"

class UImage;
class UUIButtonWidget;


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIPreviewImagePageWidget : public UUIPageContentWidget, public IManagedTextureRequester {
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

protected:
	// virtual void NativeOnInitialized() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnRefresh() override;
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	FString URL;

	void SetIsProcessing(const bool bInIsLoading) const;

	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	FVector2D DesiredSize;

};
