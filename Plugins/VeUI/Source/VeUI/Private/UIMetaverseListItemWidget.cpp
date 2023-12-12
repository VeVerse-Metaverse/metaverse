// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIMetaverseListItemWidget.h"

#include "ApiPackageMetadataObject.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "VeShared.h"
#include "Components/UILoadingWidget.h"
#include "AnimatedTexture2D.h"
#include "Components/UIImageWidget.h"


#define LOCTEXT_NAMESPACE "VeUI"


void UUIMetaverseListItemWidget::NativeConstruct() {
	Super::NativeConstruct();
	
	SetIsLoading(false);
}

void UUIMetaverseListItemWidget::NativeOnEntryReleased() {
	IUserObjectListEntry::NativeOnEntryReleased();

	// PreviewImageWidget->SetBrushFromTexture(DefaultTexture);
	// ReleaseAllTextures();
}

void UUIMetaverseListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	// if (DefaultTexture) {
	// 	SetTexture(DefaultTexture);
	// }
	
	if (const UApiPackageMetadataObject* ModObject = Cast<UApiPackageMetadataObject>(ListItemObject)) {
		Metadata = ModObject->Metadata;

		if (!Metadata.IsValid()) return;

		if (IsValid(TitleTextWidget)) {
			if (Metadata->Title.IsEmpty()) {
				TitleTextWidget->SetText(NSLOCTEXT("VeUI", "ModNameUnknown", "Unnamed"));
			} else {
				TitleTextWidget->SetText(FText::FromString(TruncateString(Metadata->Title)));
			}
		}

		if (IsValid(AuthorTextWidget)) {
			if (Metadata->Owner.IsValid() && !Metadata->Owner->Name.IsEmpty()) {
				AuthorTextWidget->SetText(FText::FromString(Metadata->Owner->Name));
			} else {
				AuthorTextWidget->SetText(NSLOCTEXT("VeUI", "ModAuthorUnknown", "Unknown"));
			}
		}

		if (IsValid(PriceTextWidget)) {
			if (Metadata->Price > 0) {
				PriceTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ModPriceFormat", "{0}"), FText::AsCurrency(Metadata->Price)));
			} else {
				PriceTextWidget->SetText(NSLOCTEXT("VeUI", "ModPriceFree", "Free"));
			}
		}

		if (IsValid(RatingTextWidget)) {
			if (Metadata->Likes + Metadata->Dislikes > 0) {
				const float Value = (static_cast<float>(Metadata->Likes) / (Metadata->Likes + Metadata->Dislikes) * 5 - 1) / 4 * 5;
				RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ModRatingFormat", "{0}"), FText::AsNumber(Value)));
			} else {
				RatingTextWidget->SetText(NSLOCTEXT("VeUI", "ModRatingNone", "Not rated"));
			}
		}

		if (IsValid(DownloadSizeTextWidget)) {
			if (Metadata->DownloadSize > 0) {
				FNumberFormattingOptions NumberFormattingOptions;
				NumberFormattingOptions.SetMaximumFractionalDigits(2);
				DownloadSizeTextWidget->SetText(
					FText::Format(NSLOCTEXT("VeUI", "ModDownloadSizeFormat", "{0}"), FText::AsMemory(Metadata->DownloadSize, &NumberFormattingOptions, nullptr, SI)));
			} else {
				DownloadSizeTextWidget->SetText(NSLOCTEXT("VeUI", "ModDownloadSizeUnknown", "?"));
			}
		}

		if (IsValid(DownloadCountTextWidget)) {
			DownloadCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ModDownloadCountFormat", "{0}"), FText::AsNumber(Metadata->Downloads)));
		}

		// PreviewImage
		if (PreviewImageWidget) {
			const TArray<EApiFileType> Types = {
				EApiFileType::ImagePreview,
				EApiFileType::ImageFull,
			};
	
			if (const auto* FileMetadata = FindFileMetadata(Metadata->Files, Types)) {
				PreviewImageWidget->ShowImage(FileMetadata->Url);
			}
		}
	}
}

void UUIMetaverseListItemWidget::SetIsLoading(bool bInIsLoading) const {
	if (LoadingWidget) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

// void UUIMetaverseListItemWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
// 	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
// 		SetIsLoading(false);
// 		return;
// 	}
//
// 	// SetTexture(InResult.Texture);
// 	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
//
// 	// SetIsLoading(false);
// 	
// 	// todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
// 	RUN_DELAYED(0.1f, this, [this](){
// 				SetIsLoading(false);
// 				});
// }

// void UUIMetaverseListItemWidget::SetTexture(UTexture2D* Texture) {
// 	PreviewImageWidget->SetBrushFromTexture(Texture);
//
// 	const FVector2D Size = {
// 		static_cast<float>(Texture->GetSizeX()),
// 		static_cast<float>(Texture->GetSizeY())
// 	};
// 	
// 	PreviewImageWidget->SetDesiredSizeOverride(Size);
// }


#undef LOCTEXT_NAMESPACE
