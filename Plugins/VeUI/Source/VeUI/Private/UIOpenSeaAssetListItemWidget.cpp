// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UIOpenSeaAssetListItemWidget.h"
#include "OpenSeaAssetMetadataObject.h"
#include "VeShared.h"
#include "AnimatedTexture2D.h"
#include "ApiFileMetadata.h"
#include "ApiOpenSeaMetadataObject.h"
#include "Components/UIImageWidget.h"


void UUIOpenSeaAssetListItemWidget::NativeConstruct() {
	SetIsLoading(false);

	Super::NativeConstruct();
}

void UUIOpenSeaAssetListItemWidget::NativeOnEntryReleased() {
	if (PreviewImageWidget) {
		if (!ImageURL.IsEmpty()) {
			PreviewImageWidget->ShowImage(ImageURL);
		}
	}

	IUserObjectListEntry::NativeOnEntryReleased();
}

void UUIOpenSeaAssetListItemWidget::SetOwnerAddress(const FString& InOwnerAddress) {
	const FString OwnerAddress = InOwnerAddress;
	if (IsValid(AuthorTextWidget)) {
		AuthorTextWidget->SetText(FText::FromString(FString::Printf(TEXT("%s...%s"), *OwnerAddress.Left(6), *OwnerAddress.Right(4))));
	}
}

void UUIOpenSeaAssetListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {

	if (const UApiOpenSeaMetadataObject* ObjectObject = Cast<UApiOpenSeaMetadataObject>(ListItemObject)) {
		ObjectMetadata = ObjectObject->Metadata;

		if (NameTextWidget) {
			if (ObjectMetadata.AssetName.IsEmpty()) {
				NameTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectNameUnknown", "Unnamed"));
			} else {
				NameTextWidget->SetText(FText::FromString(TruncateString(ObjectMetadata.AssetName, 16)));
			}
		}

		SetOwnerAddress(ObjectMetadata.Owner);
	}

	//FString ImageURL;
	if (!ObjectMetadata.ImageThumbnailUrl.IsEmpty()) {
		ImageURL = ObjectMetadata.ImageThumbnailUrl;
	} else if (!ObjectMetadata.ImagePreviewUrl.IsEmpty()) {
		ImageURL = ObjectMetadata.ImagePreviewUrl;
	} else if (!ObjectMetadata.ImageUrl.IsEmpty()) {
		ImageURL = ObjectMetadata.ImageUrl;
	} else if (!ObjectMetadata.GetImageUrl().IsEmpty()) {
		if (ObjectMetadata.GetImageUrl().EndsWith(".jpg") || ObjectMetadata.GetImageUrl().EndsWith(".png")) {
			ImageURL = ObjectMetadata.GetImageUrl();
		} else {
			LogWarningF("token metadata of unknown format: %s", *ObjectMetadata.GetImageUrl());
		}
	} else {
		LogWarningF("token has no image: %s", *ObjectMetadata.AssetName);
	}

	if (PreviewImageWidget) {
		if (!ImageURL.IsEmpty()) {
			PreviewImageWidget->ShowImage(ImageURL);
		}
	}
}

// void UUIOpenSeaAssetListItemWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
// if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
// 	SetIsLoading(false);
// 	SetTexture(DefaultTexture);
// 	return;
// }
//
// ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
//
// // todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
// RUN_DELAYED(0.1f, this, [this](){
// 			SetIsLoading(false);
// 			});
// }

// void UUIOpenSeaAssetListItemWidget::SetTexture(UTexture2D* Texture) {
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

void UUIOpenSeaAssetListItemWidget::SetIsLoading(const bool bInIsLoading) const {
	if (LoadingWidget) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}
