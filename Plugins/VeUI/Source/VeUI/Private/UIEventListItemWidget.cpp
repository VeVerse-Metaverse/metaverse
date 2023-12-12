// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIEventListItemWidget.h"

#include "ApiEventMetadataObject.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "VeShared.h"
#include "Components/UILoadingWidget.h"
#include "Components/UIImageWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIEventListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {

	if (const UApiEventMetadataObject* ModObject = Cast<UApiEventMetadataObject>(ListItemObject)) {
		Metadata = ModObject->Metadata;

		if (!Metadata.IsValid()) return;

		// PreviewImage
		if (PreviewImageWidget) {
			PreviewImageWidget->ShowImage(Metadata->Files, EApiFileType::ImagePreview);
		}

		if (IsValid(TitleTextWidget)) {
			if (Metadata->Title.IsEmpty()) {
				TitleTextWidget->SetText(NSLOCTEXT("VeUI", "ModNameUnknown", "Unnamed"));
			} else {
				TitleTextWidget->SetText(FText::FromString(TruncateString(Metadata->Title)));
			}
		}

		if (IsValid(OwnerTextWidget)) {
			if (Metadata->Owner.Id.IsValid() && !Metadata->Owner.Name.IsEmpty()) {
				OwnerTextWidget->SetText(FText::FromString(Metadata->Owner.Name));
			} else {
				OwnerTextWidget->SetText(NSLOCTEXT("VeUI", "ModAuthorUnknown", "Unknown"));
			}
		}

		if (StartsDateTextWidget) {
			if (Metadata->StartsAt == 0) {
				StartsDateTextWidget->SetText(NSLOCTEXT("VeUI", "Events_StartsDateUnknown", "No starts date"));
			} else {
				// StartsDateTextWidget->SetText(FText::FromString(Metadata.StartsAt.ToString()));
				StartsDateTextWidget->SetText(FText::AsDate(Metadata->StartsAt));
			}
		}

		if (EndsDateTextWidget) {
			if (Metadata->EndsAt == 0) {
				EndsDateTextWidget->SetText(NSLOCTEXT("VeUI", "Events_EndsDateUnknown", "No ends date"));
			} else {
				// EndsDateTextWidget->SetText(FText::FromString(Metadata.StartsAt.ToString()));
				EndsDateTextWidget->SetText(FText::AsDate(Metadata->EndsAt));
			}
		}
		
		//
		// if (IsValid(PriceTextWidget)) {
		// 	if (Metadata->Price > 0) {
		// 		PriceTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ModPriceFormat", "{0}"), FText::AsCurrency(Metadata->Price)));
		// 	} else {
		// 		PriceTextWidget->SetText(NSLOCTEXT("VeUI", "ModPriceFree", "Free"));
		// 	}
		// }

		// if (IsValid(RatingTextWidget)) {
		// 	if (Metadata->Likes + Metadata->Dislikes > 0) {
		// 		const float Value = (static_cast<float>(Metadata->Likes) / (Metadata->Likes + Metadata->Dislikes) * 5 - 1) / 4 * 5;
		// 		RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ModRatingFormat", "{0}"), FText::AsNumber(Value)));
		// 	} else {
		// 		RatingTextWidget->SetText(NSLOCTEXT("VeUI", "ModRatingNone", "Not rated"));
		// 	}
		// }

		// if (IsValid(DownloadSizeTextWidget)) {
		// 	if (Metadata->DownloadSize > 0) {
		// 		FNumberFormattingOptions NumberFormattingOptions;
		// 		NumberFormattingOptions.SetMaximumFractionalDigits(2);
		// 		DownloadSizeTextWidget->SetText(
		// 			FText::Format(NSLOCTEXT("VeUI", "ModDownloadSizeFormat", "{0}"), FText::AsMemory(Metadata->DownloadSize, &NumberFormattingOptions, nullptr, SI)));
		// 	} else {
		// 		DownloadSizeTextWidget->SetText(NSLOCTEXT("VeUI", "ModDownloadSizeUnknown", "?"));
		// 	}
		// }

		// if (IsValid(DownloadCountTextWidget)) {
		// 	DownloadCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ModDownloadCountFormat", "{0}"), FText::AsNumber(Metadata->Downloads)));
		// }
	}
}


#undef LOCTEXT_NAMESPACE
