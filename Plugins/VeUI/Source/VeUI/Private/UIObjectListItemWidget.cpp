// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIObjectListItemWidget.h"

#include "ApiObjectMetadataObject.h"
#include "Components/TextBlock.h"
#include "Components/UIImageWidget.h"
#include "VeShared.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIObjectListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	if (const UApiObjectMetadataObject* ObjectObject = Cast<UApiObjectMetadataObject>(ListItemObject)) {
		Metadata = ObjectObject->Metadata;

		if (PreviewImageWidget) {
			const TArray<EApiFileType> Types = {
				EApiFileType::ImagePreview,
				// EApiFileType::ImageFull,
				// EApiFileType::TextureDiffuse,
			};

			if (const auto* FileMetadata = FindFileMetadata(Metadata->Files, Types)) {
				PreviewImageWidget->ShowImage(FileMetadata->Url);
			} else {
				PreviewImageWidget->ShowDefaultImage();
			}
		}

		if (NameTextWidget) {
			if (Metadata->Name.IsEmpty()) {
				NameTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectNameUnknown", "Unnamed"));
			} else {
				NameTextWidget->SetText(FText::FromString(TruncateString(Metadata->Name, 16)));
			}
		}

		if (AuthorTextWidget) {
			if (Metadata->Owner.Name.IsEmpty()) {
				AuthorTextWidget->SetText(NSLOCTEXT("VeUI", "OwnerNameUnknown", "Unknown"));
			} else {
				AuthorTextWidget->SetText(FText::FromString(Metadata->Owner.Name));
			}
		}

		if (RatingTextWidget) {
			if (Metadata->TotalLikes + Metadata->TotalDislikes > 0) {
				const float Value = (static_cast<float>(Metadata->TotalLikes) / (Metadata->TotalLikes + Metadata->TotalDislikes) * 5 - 1) / 4 * 5;
				RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ObjectRatingFormat", "{0}"), FText::AsNumber(Value)));
			} else {
				RatingTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectRatingNone", "Not rated"));
			}
		}
	}
}


#undef LOCTEXT_NAMESPACE
