// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "ApiFileMetadata.h"
#include "ManagedTextureRequester.h"
#include "Components/UIButtonWidget.h"
#include "UIGalleryImageWidget.h"
#include "Components/UILoadingWidget.h"
#include "UIWidgetBase.h"

#include "Components/Image.h"
#include "UIZoomImageWidget.generated.h"

UCLASS()
class VEUI_API UUIZoomImageWidget : public UUIWidgetBase,  public IManagedTextureRequester {
	GENERATED_BODY()
	
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;
	void SetIsLoading(const bool bInIsLoading) const;
	
	FDelegateHandle OnCloseButtonWidgetDelegateHandle;
	
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
public:
	FApiFileMetadata Metadata;
	
	void ZoomObject(FGeometry InGeometry,FPointerEvent InMouseEvent);
	void ScaleImage(bool bInZoom);
	void SetMetadata(const FApiFileMetadata& InMetadata);
	void ZoomReset();
	void Reset();
	
	int MaxZoom = 10;
	int MinZoom = -4;
	int CurrentZoomStep = 0;
	float ZoomFactor = 0.8f;
	float PanSpeed = 1.f;
	float ButtonZoom = 1.f;
	bool CanMoveImage = true;
	bool IsDraggingImage = false;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUILoadingWidget* LoadingWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIGalleryImageWidget* GalleryImageWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CloseButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ApproximationButton;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* DistanceButton;
	
	DECLARE_EVENT(UUIZoomImageWidget, FOnClosed);
	FOnClosed OnClosedDelegate;
};
