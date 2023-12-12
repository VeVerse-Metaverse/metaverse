// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISpaceListItemWidget.h"

#include "ApiSpaceMetadataObject.h"
#include "VeFileMetadata.h"
#include "VeShared.h"
#include "VeWorldMetadata.h"
#include "Components/UIImageWidget.h"
#include "Components/TextBlock.h"


#define LOCTEXT_NAMESPACE "VeUI"


void UUISpaceListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	if (const UApiSpaceMetadataObject* SpaceObject = Cast<UApiSpaceMetadataObject>(ListItemObject)) {
		Metadata = FVeWorldMetadata(*SpaceObject->Metadata);

		if (PreviewImageWidget) {
			const TArray<EApiFileType> Types = {
				EApiFileType::ImagePreview,
				// EApiFileType::ImageFull,
				// EApiFileType::TextureDiffuse,
			};

			if (const auto* FileMetadata = FindFileMetadata(Metadata.Files, Types)) {
				PreviewImageWidget->ShowImage(FileMetadata->Url);
			} else {
				PreviewImageWidget->ShowDefaultImage();
			}
		}

		if (NameTextWidget) {
			if (Metadata.Name.IsEmpty()) {
				NameTextWidget->SetText(NSLOCTEXT("VeUI", "WorldNameUnknown", "Unnamed"));
			} else {
				NameTextWidget->SetText(FText::FromString(TruncateString(Metadata.Name, 16)));

			}
		}

		if (OwnerTextWidget) {
			if (Metadata.Owner.Name.IsEmpty()) {
				OwnerTextWidget->SetText(NSLOCTEXT("VeUI", "WorldOwnerUnknown", "Unknown"));
			} else {
				OwnerTextWidget->SetText(FText::FromString(TruncateString(Metadata.Owner.Name, 16)));
			}
		}
		
		if (RatingTextWidget) {
			if (Metadata.Likes + Metadata.Dislikes > 0) {
				const float Value = (static_cast<float>(Metadata.Likes) / (Metadata.Likes + Metadata.Dislikes) * 5 - 1) / 4 * 5;
				RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "WorldRatingFormat", "{0}"), FText::AsNumber(Value)));
			} else {
				RatingTextWidget->SetText(NSLOCTEXT("VeUI", "WorldRatingNone", "-"));
			}
		}
	}
}


#undef LOCTEXT_NAMESPACE
