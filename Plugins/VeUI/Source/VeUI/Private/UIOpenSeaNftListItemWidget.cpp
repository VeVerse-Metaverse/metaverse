// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UIOpenSeaNftListItemWidget.h"

#include "ApiFileMetadata.h"
#include "OpenSeaAssetMetadataObject.h"
#include "VeShared.h"
#include "AnimatedTexture2D.h"



void UUIOpenSeaNftListItemWidget::NativeConstruct() {
	SetIsLoading(false);

	Super::NativeConstruct();
}

void UUIOpenSeaNftListItemWidget::NativeOnEntryReleased() {
	PreviewImageWidget->SetBrushFromTexture(DefaultTexture);
	// ReleaseAllTextures();

	IUserObjectListEntry::NativeOnEntryReleased();
}

void UUIOpenSeaNftListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	PreviewImageWidget->SetBrushFromTexture(DefaultTexture);

	if (const UOpenSeaAssetMetadataObject* AssetsObject = Cast<UOpenSeaAssetMetadataObject>(ListItemObject)) {
		Asset = AssetsObject->Metadata;

		if (NameTextWidget) {
			if (Asset.Name.IsEmpty()) {
				NameTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectNameUnknown", "Unnamed"));
			} else {
				NameTextWidget->SetText(FText::FromString(TruncateString(Asset.Name)));
			}
		}
	}

	const auto PreviewImage = Asset.ImageUrl;

	if (!PreviewImage.IsEmpty()) {
		RequestTexture(this, PreviewImage, TEXTUREGROUP_UI);
		SetIsLoading(true);
	}
}

void UUIOpenSeaNftListItemWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		SetIsLoading(false);
		if (DefaultTexture) {
			PreviewImageWidget->SetBrushFromTexture(DefaultTexture);
		}
		return;
	}

	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);

	// todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
	RUN_DELAYED(0.1f, this, [this](){
				SetIsLoading(false);
				});
}

void UUIOpenSeaNftListItemWidget::SetIsLoading(const bool bInIsLoading) const {
	if (LoadingWidget) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}
