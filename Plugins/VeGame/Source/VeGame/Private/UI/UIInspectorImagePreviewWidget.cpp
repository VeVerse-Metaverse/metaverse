// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIInspectorImagePreviewWidget.h"

#include "UIGalleryImageWidget.h"
#include "Components/UIButtonWidget.h"
#include "VeShared.h"
#include "Components/UILoadingWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "AnimatedTexture2D.h"


#define LOCTEXT_NAMESPACE "VeUI"

void UUIInspectorImagePreviewWidget::NativeConstruct() {
	Super::NativeConstruct();

	SetIsProcessing(false);
}

void UUIInspectorImagePreviewWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (!OnApproximationButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(ApproximationButton)) {
			OnApproximationButtonClickedDelegateHandle = ApproximationButton->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				ScaleImage(true);
			});
		}
	}

	if (!OnDistanceButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(DistanceButton)) {
			OnDistanceButtonClickedDelegateHandle = DistanceButton->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				ScaleImage(false);
			});
		}
	}
}

void UUIInspectorImagePreviewWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	if (OnApproximationButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(ApproximationButton)) {
			ApproximationButton->GetOnButtonClicked().Remove(OnApproximationButtonClickedDelegateHandle);
		}
		OnApproximationButtonClickedDelegateHandle.Reset();
	}

	if (OnDistanceButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(DistanceButton)) {
			DistanceButton->GetOnButtonClicked().Remove(OnDistanceButtonClickedDelegateHandle);
		}
		OnDistanceButtonClickedDelegateHandle.Reset();
	}
}

void UUIInspectorImagePreviewWidget::NativeOnRefresh() {
	SetIsProcessing(true);
	RequestTexture(this, URL, TEXTUREGROUP_UI);
}

void UUIInspectorImagePreviewWidget::ZoomReset() {
	FWidgetTransform Transform;
	PreviewImageWidget->SetRenderTransform(Transform);
	CurrentZoomStep = 0;
}

void UUIInspectorImagePreviewWidget::SetUrl(const FString& InUrl) {
	URL = InUrl;
	NativeOnRefresh();
}

void UUIInspectorImagePreviewWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {

	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		SetIsProcessing(false);
		return;
	}

	if (IsValid(PreviewImageWidget)) {
		if (UTexture2D* Texture2D = InResult.AsTexture2D()) {
			PreviewImageWidget->SetBrushFromTexture(Texture2D);
		} else if (UAnimatedTexture2D* AnimatedTexture2D = InResult.AsAnimatedTexture2D()) {
			PreviewImageWidget->SetBrushResourceObject(AnimatedTexture2D);
		}

		FVector2D DesiredSize = {InResult.Texture->GetSurfaceWidth(), InResult.Texture->GetSurfaceHeight()};

		FVector2D MultySize = {
			DefaultSize.X / DesiredSize.X,
			DefaultSize.Y / DesiredSize.Y
		};

		float ImageScale = (MultySize.X < MultySize.Y) ? MultySize.X : MultySize.Y;
		if (ImageScale) {
			DesiredSize = DesiredSize * ImageScale;
		}

		PreviewImageWidget->SetDesiredSizeOverride(DesiredSize);
	};

	// todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
	RUN_DELAYED(0.1f, this, [this](){
		SetIsProcessing(false);
		});

}

void UUIInspectorImagePreviewWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

FReply UUIInspectorImagePreviewWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	CanMoveImage = true;
	if (CanMoveImage) {
		ZoomObject(InGeometry, InMouseEvent);
	}

	return FReply::Handled();
}

FReply UUIInspectorImagePreviewWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {

	IsDraggingImage = InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton && CanMoveImage;

	if (IsDraggingImage) {
		return FReply::Unhandled();
	}

	return FReply::Handled();
}

FReply UUIInspectorImagePreviewWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	IsDraggingImage = InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && CanMoveImage;

	if (IsDraggingImage) {
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UUIInspectorImagePreviewWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	if (CanMoveImage && IsDraggingImage) {
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		PreviewImageWidget->SetRenderTranslation(InMouseEvent.GetCursorDelta() * PanSpeed / Scale + PreviewImageWidget->GetRenderTransform().Translation);
	}

	return FReply::Handled();
}

void UUIInspectorImagePreviewWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
	IsDraggingImage = false;
}

void UUIInspectorImagePreviewWidget::ZoomObject(FGeometry InGeometry, FPointerEvent InMouseEvent) {

	if (InMouseEvent.GetWheelDelta() > 0 && CurrentZoomStep < MaxZoom) {
		CurrentZoomStep++;
	} else if (InMouseEvent.GetWheelDelta() <= 0 && CurrentZoomStep > MinZoom) {
		CurrentZoomStep--;
	} else {
		return;
	}

	const FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	const FVector2D LocalSize = InGeometry.GetLocalSize() / 2.f;
	const FVector2D LocalTranslation = LocalPosition - LocalSize - PreviewImageWidget->GetRenderTransform().Translation;

	float LocalZoomFactor = ZoomFactor;
	if (InMouseEvent.GetWheelDelta() > 0) {
		LocalZoomFactor = 1.f / ZoomFactor;
	}

	const FVector2D RenderScale = PreviewImageWidget->GetRenderTransform().Translation - (LocalZoomFactor - 1.f) * LocalTranslation;
	PreviewImageWidget->SetRenderScale(LocalZoomFactor * PreviewImageWidget->GetRenderTransform().Scale);
	PreviewImageWidget->SetRenderTranslation(RenderScale);
}

void UUIInspectorImagePreviewWidget::ScaleImage(bool bInZoom) {
	if (bInZoom && CurrentZoomStep < MaxZoom) {
		CurrentZoomStep++;
	} else if (!bInZoom && CurrentZoomStep > MinZoom) {
		CurrentZoomStep--;
	} else {
		return;
	}

	float LocalZoomFactor = ZoomFactor;
	if (bInZoom) {
		LocalZoomFactor = 1.f / ZoomFactor;
	}

	PreviewImageWidget->SetRenderScale(LocalZoomFactor * PreviewImageWidget->GetRenderTransform().Scale);
}


#undef LOCTEXT_NAMESPACE
