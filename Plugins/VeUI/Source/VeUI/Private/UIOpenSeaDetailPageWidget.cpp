// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIOpenSeaDetailPageWidget.h"

#include "VeUI.h"
#include "VeShared.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "UIPageManagerWidget.h"
#include "UIPreviewImagePageWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/UIButtonWidget.h"
#include "AnimatedTexture2D.h"
#include "ApiOpenSeaMetadata.h"
#include "VeApi2.h"
#include "Api2OpenSeaSubsystem.h"
#include "Components/UIImageWidget.h"


#define LOCTEXT_NAMESPACE "VeUI"


void UUIOpenSeaDetailPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (NameTextWidget) {
		NameTextWidget->SetText(FText());
	}

	if (DescriptionTextWidget) {
		DescriptionTextWidget->SetText(FText());
	}

	//SetTexture(DefaultTexture);
}

void UUIOpenSeaDetailPageWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (!OnPreviewButtonClickedDelegateHandle.IsValid()) {
		if (PreviewImageButtonWidget) {
			OnPreviewButtonClickedDelegateHandle = PreviewImageButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (auto Page = GetPageManager()->OpenPage<UUIPreviewImagePageWidget>(PreviewPageClass, ImageURL)) {
					Page->SetParentPage(this);
				}
			});
		}
	}
}

void UUIOpenSeaDetailPageWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	if (OnPreviewButtonClickedDelegateHandle.IsValid()) {
		if (PreviewImageButtonWidget) {
			PreviewImageButtonWidget->GetOnButtonClicked().Remove(OnPreviewButtonClickedDelegateHandle);
		}
		OnPreviewButtonClickedDelegateHandle.Reset();
	}
}

void UUIOpenSeaDetailPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	// if (IsValid(DefaultTexture)) {
	// 	PreviewImageWidget->SetBrushFromTexture(DefaultTexture);
	// }
	
	if (PreviewImageWidget) {
		if (!ImageURL.IsEmpty()) {
			PreviewImageWidget->ShowImage(ImageURL);
		}
	}

	if (Metadata.Id.IsValid()) {
		SetMetadata(Metadata);
	}

	const FGuid OptionsId = FGuid(GetOptions());
	
	MetadataRequest(OptionsId);
}

void UUIOpenSeaDetailPageWidget::NativeConstruct() {
	Super::NativeConstruct();
	const FGuid OptionsId = FGuid(GetOptions());

	if (OptionsId.IsValid()) {
		MetadataRequest(OptionsId);
	}
}


void UUIOpenSeaDetailPageWidget::MetadataRequest(const FGuid& InId) {
	GET_MODULE_SUBSYSTEM(OpenSeaSubsystem, Api2, OpenSea);

	if (OpenSeaSubsystem) {
		// 1. Callback
		TSharedRef<FOnOpenSeaRequestCompleted> CallbackPtr = MakeShareable(new FOnOpenSeaRequestCompleted{});

		// 2. Lambda
		CallbackPtr->BindWeakLambda(this, [=](const FApiOpenSeaAssetMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) mutable {
			if (InResponseCode != EApi2ResponseCode::Ok) {
				LogErrorF("failed to request opensea asset: %s", *InError);
			}

			if (InResponseCode == EApi2ResponseCode::Ok) {
				OnMetadataResponse(InMetadata, InResponseCode, InError);
			}

		});

		// 3. Request
		OpenSeaSubsystem->Get(InId, CallbackPtr);
	}
}

void UUIOpenSeaDetailPageWidget::OnMetadataResponse(const FApiOpenSeaAssetMetadata& InMetadata,const EApi2ResponseCode InResponseCode, const FString& InError) {
	if (InResponseCode != EApi2ResponseCode::NotFound) {
		SetMetadata(InMetadata);
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadOpenSeaError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a OpenSea data.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIOpenSeaDetailPageWidget::SetMetadata(const FApiOpenSeaAssetMetadata& InMetadata) {
	Metadata = InMetadata;

	FText Name = (InMetadata.AssetName.IsEmpty()) ? NSLOCTEXT("VeUI", "AssetNameUnknown", "Unnamed") : FText::FromString(InMetadata.AssetName);
	SetTitle(Name);
	if (NameTextWidget) {
		NameTextWidget->SetText(Name);
	}

	if (DescriptionTextWidget) {
		if (Metadata.AssetDescription.IsEmpty()) {
			DescriptionTextWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata.AssetDescription));
		}
	}

	if (OpenPageButtonWidget) {
		OpenPageButtonWidget->URL = InMetadata.Permalink;
	}

	// AnimationUrl = MPEG4
	// AInteractivePainting::OnPlaceableMetadataReceived
	// AInteractivePainting::OnFileURLReceived

	ImageURL = "";
	if (!InMetadata.ImageUrl.IsEmpty()) {
		ImageURL = InMetadata.ImageUrl;
	} else if (!InMetadata.ImagePreviewUrl.IsEmpty()) {
		ImageURL = InMetadata.ImagePreviewUrl;
	} else if (!InMetadata.ImageThumbnailUrl.IsEmpty()) {
		ImageURL = InMetadata.ImageThumbnailUrl;
	} else if (!InMetadata.GetImageUrl().IsEmpty()) {
		if (InMetadata.GetImageUrl().EndsWith(".jpg") || InMetadata.GetImageUrl().EndsWith(".png")) {
			ImageURL = InMetadata.GetImageUrl();
		} else {
			LogWarningF("token metadata of unknown format: %s", *InMetadata.GetImageUrl());
		}
	} else {
		LogWarningF("token has no image: %s", *InMetadata.AssetName);
	}
	
	if (PreviewImageWidget) {
		if (!ImageURL.IsEmpty()) {
			PreviewImageWidget->ShowImage(ImageURL);
		}
	}
}
//
// void UUIOpenSeaDetailPageWidget::SetTexture(UTexture2D* Texture) {
// 	if (!PreviewImageWidget || !Texture) {
// 		return;
// 	}
//
// 	PreviewImageWidget->SetBrushFromTexture(Texture);
//
// 	const FVector2D Size = {
// 		static_cast<float>(Texture->GetSizeX()),
// 		static_cast<float>(Texture->GetSizeY())
// 	};
//
// 	PreviewImageWidget->SetDesiredSizeOverride(Size);
// }
//
// void UUIOpenSeaDetailPageWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
// 	// LogScreenLogF("Texture Loaded at ObjectDetailWidget %s", *InResult.Url);
//
// 	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
// }


#undef LOCTEXT_NAMESPACE
