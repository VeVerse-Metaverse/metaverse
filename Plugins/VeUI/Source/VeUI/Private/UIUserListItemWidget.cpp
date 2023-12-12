// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIUserListItemWidget.h"

#include "ApiUserMetadataObject.h"
#include "Components/TextBlock.h"
#include "VeShared.h"
#include "Components/UILoadingWidget.h"
#include "Components/UIImageWidget.h"


#define LOCTEXT_NAMESPACE "VeUI"


void UUIUserListItemWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (PreviewImageWidget) {
		if (!PreviewImageWidget->ShowImage(Metadata->Files, EApiFileType::ImageAvatar)) {
			PreviewImageWidget->ShowImage(Metadata->GetDefaultAvatarUrl());
		}else {
			PreviewImageWidget->ShowDefaultImage();
		}
	}
}

void UUIUserListItemWidget::NativeOnEntryReleased() {
	IUserObjectListEntry::NativeOnEntryReleased();

	// ReleaseAllTextures();
}

void UUIUserListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {

	if (const UApiUserMetadataObject* UserObject = Cast<UApiUserMetadataObject>(ListItemObject)) {
		Metadata = UserObject->Metadata;
		if (Metadata.IsValid()) {
			if (NameTextWidget) {
				if (Metadata->Name.IsEmpty()) {
					NameTextWidget->SetText(NSLOCTEXT("VeUI", "UserNameUnknown", "Unnamed"));
				} else {
					NameTextWidget->SetText(FText::FromString(TruncateString(Metadata->Name, 16)));
				}
			}

			if (LevelTextWidget) {
				LevelTextWidget->SetText(FText::AsNumber(Metadata->Level));
			}

			if (RankTextWidget) {
				if (Metadata->Rank.IsEmpty()) {
					RankTextWidget->SetText(NSLOCTEXT("VeUI", "UserRankUnknown", "Unknown"));
				} else {
					RankTextWidget->SetText(FText::FromString(Metadata->Rank.Mid(0, 1).ToUpper() + Metadata->Rank.Mid(1)));
				}
			}

			if (RatingTextWidget) {
				const float Rating = Metadata->GetRating();
				if (Rating < 0.f) {
					RatingTextWidget->SetText(NSLOCTEXT("VeUI", "UserRatingNone", "-"));
				} else {
					RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "UserRatingFormat", "{0}"), Rating));
				}
			}

			// PreviewImage
			if (PreviewImageWidget) {
				if (!PreviewImageWidget->ShowImage(Metadata->Files, EApiFileType::ImageAvatar)) {
					PreviewImageWidget->ShowImage(Metadata->GetDefaultAvatarUrl());
				}else {
					PreviewImageWidget->ShowDefaultImage();
				}
			}
		}
	}
}



#undef LOCTEXT_NAMESPACE
