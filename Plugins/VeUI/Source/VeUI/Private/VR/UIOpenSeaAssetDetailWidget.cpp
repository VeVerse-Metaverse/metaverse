// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIOpenSeaAssetDetailWidget.h"
// #include "ApiObjectSubsystem.h"
#include "VeApi.h"
#include "AnimatedTexture2D.h"


#define LOCTEXT_NAMESPACE "VeUI"

void UUIOpenSeaAssetDetailWidget::NativeConstruct() {
	bZoomImageWidgetVisible = true;

	HideZoomImageWidget(false, false);

	Super::NativeConstruct();
}

void UUIOpenSeaAssetDetailWidget::RegisterCallbacks() {
	if (CloseButtonWidget) {
		if (!OnCloseButtonWidgetDelegateHandle.IsValid()) {
			OnCloseButtonWidgetDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosed.IsBound()) {
					OnClosed.Broadcast();
				}
			});
		}
	}

	if (PreviewImageButtonWidget) {
		if (!OnPreviewButtonClickedDelegateHandle.IsValid()) {
			OnPreviewButtonClickedDelegateHandle = PreviewImageButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (ZoomImageWidget) {
					// FApiFileMetadata* PreviewImage = Metadata.Files.FindByPredicate([=](const FApiFileMetadata& File) {
					// 	return File.Type == EApiFileType::ImageFull;
					// });
					// if (PreviewImage) {
					// 	ZoomImageWidget->SetMetadata(*PreviewImage);
					// }
				}
				ShowZoomImageWidget();
			});
		}
	}

	if (ZoomImageWidget) {
		if (!OnZoomImageWidgetClosedDelegateHandle.IsValid())
			OnZoomImageWidgetClosedDelegateHandle = ZoomImageWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				HideZoomImageWidget();
			});
	}

	// if (!OnObjectEditButtonClickedDelegateHandle.IsValid()) {
	// 	if (EditButtonWidget) {
	// 		OnObjectEditButtonClickedDelegateHandle = EditButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIOpenSeaAssetDetailWidget::OnObjectFormEditButtonClicked);
	// 	}
	// }

	// if (!OnObjectDeleteButtonClickedDelegateHandle.IsValid()) {
		// if (DeleteButtonWidget) {
			// OnObjectDeleteButtonClickedDelegateHandle = DeleteButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIOpenSeaAssetDetailWidget::OnObjectDeleteButtonClicked);
		// }
	// }

	// if (!OnObjectDeletedDelegateHandle.IsValid()) {
	// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 		if (const TSharedPtr<FApiObjectSubsystem> ObjectSubsystem = ApiModule->GetObjectSubsystem()) {
	// 			{
	// 				OnObjectDeletedDelegateHandle = ObjectSubsystem->GetOnDeleted().AddUObject(this, &UUIOpenSeaNftDetailWidget::OnObjectDeleted);
	// 			}
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Object)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	// }
}

void UUIOpenSeaAssetDetailWidget::UnregisterCallbacks() {
	if (OnCloseButtonWidgetDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonWidgetDelegateHandle);
			OnCloseButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnObjectEditButtonClickedDelegateHandle.IsValid()) {
		if (EditButtonWidget) {
			EditButtonWidget->GetOnButtonClicked().Remove(OnObjectEditButtonClickedDelegateHandle);
			OnObjectEditButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnObjectDeleteButtonClickedDelegateHandle.IsValid()) {
		if (DeleteButtonWidget) {
			DeleteButtonWidget->GetOnButtonClicked().Remove(OnObjectDeleteButtonClickedDelegateHandle);
			OnObjectDeleteButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnPreviewButtonClickedDelegateHandle.IsValid()) {
		if (PreviewImageButtonWidget) {
			PreviewImageButtonWidget->GetOnButtonClicked().Remove(OnPreviewButtonClickedDelegateHandle);
			OnPreviewButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnZoomImageWidgetClosedDelegateHandle.IsValid()) {
		if (ZoomImageWidget) {
			ZoomImageWidget->OnClosedDelegate.Remove(OnZoomImageWidgetClosedDelegateHandle);
			OnZoomImageWidgetClosedDelegateHandle.Reset();
		}
	}
}

// void UUIOpenSeaAssetDetailWidget::OnObjectDeleteButtonClicked() {
// 	if (OnDeleteButtonClicked.IsBound()) {
// 		OnDeleteButtonClicked.Broadcast();
// 	}
//
// 	// RequestObjectDeleteConfirmation();
// }

// void UUIOpenSeaAssetDetailWidget::OnObjectDeleted(const bool bSuccessful, const FString& Error) const {
// 	FUINotificationData NotificationData;
// 	NotificationData.Lifetime = 5.f;
//
// 	if (bSuccessful) {
// 		NotificationData.Header = LOCTEXT("DeleteObjectSuccessNotificationHeader", "Success");
// 		NotificationData.Type = EUINotificationType::Success;
// 		NotificationData.Message = LOCTEXT("DeleteObjectSuccessNotificationMessage", "Successfully deleted an object.");
// 	} else {
// 		NotificationData.Header = LOCTEXT("DeleteObjectErrorNotificationHeader", "Error");
// 		NotificationData.Type = EUINotificationType::Failure;
// 		NotificationData.Message = FText::Format(LOCTEXT("DeleteObjectErrorNotificationMessage", "Failed to delete an object: {0}"), FText::FromString(Error));
// 	}
// 	AddNotification(NotificationData);
// }

void UUIOpenSeaAssetDetailWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	// LogScreenLogF("Texture Loaded at ObjectDetailWidget %s", *InResult.Url);
	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
}

void UUIOpenSeaAssetDetailWidget::SetMetadata(const FOpenSeaAssetMetadata& InMetadata) {
	Metadata = InMetadata;

	if (NameTextWidget) {
		if (Metadata.Name.IsEmpty()) {
			NameTextWidget->SetText(NSLOCTEXT("VeUI", "AssetNameUnknown", "Unnamed"));
		} else {
			NameTextWidget->SetText(FText::FromString(Metadata.Name));
		}
	}

	if (DescriptionTextWidget) {
		if (Metadata.Description.IsEmpty()) {
			DescriptionTextWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata.Description));
		}
	}
}


void UUIOpenSeaAssetDetailWidget::HideZoomImageWidget(bool bAnimate, bool bPlaySound) {
	if (bZoomImageWidgetVisible) {
		bZoomImageWidgetVisible = false;
		if (ZoomImageWidget) {
			ZoomImageWidget->Hide();
		}
	}
}

void UUIOpenSeaAssetDetailWidget::ShowZoomImageWidget(bool bAnimate, bool bPlaySound) {

	if (!bZoomImageWidgetVisible) {
		bZoomImageWidgetVisible = true;
		if (ZoomImageWidget) {
			// ZoomImageWidget->ResetState();
			// ZoomImageWidget->ResetAnimationState();
			ZoomImageWidget->Show();
		}
	}
}

#undef LOCTEXT_NAMESPACE
