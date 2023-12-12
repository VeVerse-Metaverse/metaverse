// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIEditorOpenSeaAssetItemWidget.h"

#include "UI/UIPlaceableArtObjectItemPropertiesWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "VeShared.h"
#include "Components/UIImageWidget.h"
#include "ApiOpenSeaMetadataObject.h"
#include "Components/Border.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIEditorOpenSeaAssetItemWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	MainBorderWidget->SetBrushColor(NormalColor);
}

void UUIEditorOpenSeaAssetItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	const auto* ObjectObject = Cast<UApiOpenSeaMetadataObject>(ListItemObject);
	if (!ObjectObject) {
		return;
	}
	
	Metadata = ObjectObject->Metadata;

	if (NameTextWidget) {
		if (Metadata.AssetName.IsEmpty()) {
			NameTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectNameUnknown", "Unnamed"));
		} else {
			NameTextWidget->SetText(FText::FromString(TruncateString(Metadata.AssetName, 16)));
		}
	}
	
	FString ImageURL;
	if (!Metadata.ImageThumbnailUrl.IsEmpty()) {
		ImageURL = Metadata.ImageThumbnailUrl;
	} else if (!Metadata.ImagePreviewUrl.IsEmpty()) {
		ImageURL = Metadata.ImagePreviewUrl;
	} else if (!Metadata.ImageUrl.IsEmpty()) {
		ImageURL = Metadata.ImageUrl;
	} else if (!Metadata.GetImageUrl().IsEmpty()) {
		if (Metadata.GetImageUrl().EndsWith(".jpg") || Metadata.GetImageUrl().EndsWith(".png")) {
			ImageURL = Metadata.GetImageUrl();
		} else {
			LogWarningF("token metadata of unknown format: %s", *Metadata.GetImageUrl());
		}
	} else {
		LogWarningF("token has no image: %s", *Metadata.AssetName);
	}
	
	// PreviewImage
	if (ImageWidget) {
		ImageWidget->ShowImage(ImageURL);
	}

	const FLinearColor Color = (UObject::IsSelected()) ? SelectedColor : NormalColor;
	MainBorderWidget->SetBrushColor(Color);
}

void UUIEditorOpenSeaAssetItemWidget::NativeOnItemSelectionChanged(bool bIsSelected) {
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	const FLinearColor Color = (bIsSelected) ? SelectedColor : NormalColor;
	MainBorderWidget->SetBrushColor(Color);
}


#undef LOCTEXT_NAMESPACE
