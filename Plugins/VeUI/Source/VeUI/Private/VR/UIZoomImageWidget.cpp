// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIZoomImageWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "VeShared.h"
#include "AnimatedTexture2D.h"



void UUIZoomImageWidget::SetMetadata(const FApiFileMetadata& InMetadata) {
	// if (IsValid(PreviewImageWidget)) {
	// 	PreviewImageWidget->SetMetadata(InMetadata);
	// }
	Metadata = InMetadata;

	if (!Metadata.Url.IsEmpty()) {
		RequestTexture(this, Metadata.Url, TEXTUREGROUP_UI);
		SetIsLoading(true);
	}
}

void UUIZoomImageWidget::ZoomReset() {
	FWidgetTransform Transform;
	GalleryImageWidget->SetRenderTransform(Transform);
	CurrentZoomStep = 0;
}

void UUIZoomImageWidget::RegisterCallbacks() {
	if (IsValid(CloseButtonWidget)) {
		if (!OnCloseButtonWidgetDelegateHandle.IsValid()) {
			OnCloseButtonWidgetDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
				Reset();
				ZoomReset();
			});
		}
	}

	if (ApproximationButton) {
		ApproximationButton->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			ScaleImage(true);
		});
	}

	if (DistanceButton) {
		DistanceButton->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			ScaleImage(false);
		});
	}
}

void UUIZoomImageWidget::UnregisterCallbacks() {
	if (OnCloseButtonWidgetDelegateHandle.IsValid()) {
		if (IsValid(CloseButtonWidget)) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonWidgetDelegateHandle);
			OnCloseButtonWidgetDelegateHandle.Reset();
		}
	}
}

void UUIZoomImageWidget::NativeConstruct() {
	SetIsLoading(false);

	Super::NativeConstruct();
}

void UUIZoomImageWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {

	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		SetIsLoading(false);
		return;
	}

	if (IsValid(GalleryImageWidget)) {
		ASSIGN_WIDGET_TEXTURE(GalleryImageWidget->PreviewImageWidget, InResult);
	}

	// if (IsValid(GalleryImageWidget)) {
	// 	GalleryImageWidget->PreviewImageWidget->SetBrushFromTexture(InResult.Texture);
	// }
	//
	// const FVector2D Size = {
	// 	static_cast<float>(InResult.Texture->GetSizeX()),
	// 	static_cast<float>(InResult.Texture->GetSizeY())
	// };
	//
	// if (IsValid(GalleryImageWidget)) {
	// 	GalleryImageWidget->PreviewImageWidget->SetBrushSize(Size);
	// }

	// todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
	RUN_DELAYED(0.1f, this, [this](){
				SetIsLoading(false);
				});
}

void UUIZoomImageWidget::SetIsLoading(const bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

FReply UUIZoomImageWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	CanMoveImage = true;
	if (CanMoveImage) {
		ZoomObject(InGeometry, InMouseEvent);
	}

	return FReply::Handled();
}

FReply UUIZoomImageWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {

	IsDraggingImage = InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton && CanMoveImage;

	if (IsDraggingImage) {
		return FReply::Unhandled();
	}

	return FReply::Handled();
}

FReply UUIZoomImageWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	IsDraggingImage = InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && CanMoveImage;

	if (IsDraggingImage) {
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UUIZoomImageWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	if (CanMoveImage && IsDraggingImage) {
		GalleryImageWidget->SetRenderTranslation(InMouseEvent.GetCursorDelta() * PanSpeed + GalleryImageWidget->GetRenderTransform().Translation);
	}

	return FReply::Handled();
}

void UUIZoomImageWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
	IsDraggingImage = false;
}

void UUIZoomImageWidget::Reset() {
	if (IsValid(GalleryImageWidget)) {
		if (!GalleryImageWidget->DefaultTexture) {
			GalleryImageWidget->PreviewImageWidget->SetBrushFromTexture(GalleryImageWidget->DefaultTexture);
		}
		// GalleryImageWidget->DefaultTexture;
	}
}

void UUIZoomImageWidget::ZoomObject(FGeometry InGeometry, FPointerEvent InMouseEvent) {

	if (InMouseEvent.GetWheelDelta() > 0 && CurrentZoomStep < MaxZoom) {
		CurrentZoomStep++;
	} else if (InMouseEvent.GetWheelDelta() <= 0 && CurrentZoomStep > MinZoom) {
		CurrentZoomStep--;
	} else {
		return;
	}

	const FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	const FVector2D LocalSize = InGeometry.GetLocalSize() / 2.f;
	const FVector2D LocalTranslation = LocalPosition - LocalSize - GalleryImageWidget->GetRenderTransform().Translation;

	float LocalZoomFactor = ZoomFactor;
	if (InMouseEvent.GetWheelDelta() > 0) {
		LocalZoomFactor = 1.f / ZoomFactor;
	}

	const FVector2D RenderScale = GalleryImageWidget->GetRenderTransform().Translation - (LocalZoomFactor - 1.f) * LocalTranslation;
	GalleryImageWidget->SetRenderScale(LocalZoomFactor * GalleryImageWidget->GetRenderTransform().Scale);
	GalleryImageWidget->SetRenderTranslation(RenderScale);
}

void UUIZoomImageWidget::ScaleImage(bool bInZoom) {
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

	GalleryImageWidget->SetRenderScale(LocalZoomFactor * GalleryImageWidget->GetRenderTransform().Scale);
}
