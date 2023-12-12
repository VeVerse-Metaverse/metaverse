// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIPreviewImagePageWidget.h"

#include "UILog.h"
#include "VeApi.h"
#include "UIGalleryImageWidget.h"
#include "Components/UIButtonWidget.h"
#include "VeShared.h"
#include "Components/UILoadingWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "AnimatedTexture2D.h"


#define LOCTEXT_NAMESPACE "VeUI"


// void UUIPreviewPageWidget::NativeOnInitialized() {
// 	Super::NativeOnInitialized();
// }

void UUIPreviewImagePageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(ApproximationButton)) {
		ApproximationButton->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			ScaleImage(true);
		});
	}

	if (IsValid(DistanceButton)) {
		DistanceButton->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			ScaleImage(false);
		});
	}
}

void UUIPreviewImagePageWidget::NativeConstruct() {
	Super::NativeConstruct();

	SetIsProcessing(false);

	if (PreviewImageWidget) {
		PreviewImageWidget->SetDesiredSizeOverride(DesiredSize);
	}
}

void UUIPreviewImagePageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	URL = GetOptions();
	if (URL.IsEmpty()) {
		LogWarningF("No options");
		return;
	}

	SetIsProcessing(true);
	RequestTexture(this, URL, TEXTUREGROUP_UI);
}

void UUIPreviewImagePageWidget::ZoomReset() {
	FWidgetTransform Transform;
	PreviewImageWidget->SetRenderTransform(Transform);
	CurrentZoomStep = 0;
}

void UUIPreviewImagePageWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {

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

		DesiredSize = FVector2D(InResult.Texture->GetSurfaceWidth(), InResult.Texture->GetSurfaceHeight());

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

void UUIPreviewImagePageWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

FReply UUIPreviewImagePageWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	CanMoveImage = true;
	if (CanMoveImage) {
		ZoomObject(InGeometry, InMouseEvent);
	}

	return FReply::Handled();
}

FReply UUIPreviewImagePageWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {

	IsDraggingImage = InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton && CanMoveImage;

	if (IsDraggingImage) {
		return FReply::Unhandled();
	}

	return FReply::Handled();
}

FReply UUIPreviewImagePageWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	IsDraggingImage = InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && CanMoveImage;

	if (IsDraggingImage) {
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UUIPreviewImagePageWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	if (CanMoveImage && IsDraggingImage) {
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		PreviewImageWidget->SetRenderTranslation(InMouseEvent.GetCursorDelta() * PanSpeed / Scale + PreviewImageWidget->GetRenderTransform().Translation);
	}

	return FReply::Handled();
}

void UUIPreviewImagePageWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
	IsDraggingImage = false;
}

void UUIPreviewImagePageWidget::ZoomObject(FGeometry InGeometry, FPointerEvent InMouseEvent) {

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

void UUIPreviewImagePageWidget::ScaleImage(bool bInZoom) {
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
