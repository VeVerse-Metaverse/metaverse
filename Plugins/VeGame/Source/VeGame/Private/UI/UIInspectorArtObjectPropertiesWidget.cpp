// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIInspectorArtObjectPropertiesWidget.h"

#include "Api2ObjectSubsystem.h"
#include "Api2RatingSubsystem.h"
#include "ApiObjectMetadata.h"
#include "ApiRatingMetadata.h"
#include "VeApi2.h"
#include "VeGameModule.h"
#include "VeShared.h"
#include "VeUI.h"
#include "Components/UIButtonImageWidget.h"
#include "Components/UITextPropertyWidget.h"

#define LOCTEXT_NAMESPACE "VeGame"


void UUIInspectorArtObjectPropertiesWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(LikeButtonWidget)) {
		LikeButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GET_MODULE_SUBSYSTEM(RatingSubsystem, Api2, Rating);
			if (RatingSubsystem) {
				if (bIsPressLiked == false) {
					// 1. Create a callback.
					const auto CallbackRef = MakeShared<FOnRatingRequestCompleted2>();

					// 2. Bind callback.
					CallbackRef->BindWeakLambda(this, [=](const FApiRatingMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
						if (InResponseCode != EApi2ResponseCode::Ok) {
							VeLogErrorFunc("failed to request: %s", *InError);
						}
						if (InResponseCode == EApi2ResponseCode::Ok) {
							LikeButtonWidget->SetButtonPreset(EUIButtonImagePreset::Action);
							bIsPressLiked = true;
							MetadataRequest(Metadata.Id);
						}
					});

					FApiRatingMetadata LikeMetadata;
					LikeMetadata.Id = Metadata.Id;
					LikeMetadata.Value = 1;

					// 3. Make the request using the callback.
					RatingSubsystem->UpdateLike(LikeMetadata.Id, CallbackRef);
				} else {
					// 1. Create a callback.
					const auto CallbackRef = MakeShared<FOnRatingRequestCompleted2>();

					// 2. Bind callback.
					CallbackRef->BindWeakLambda(this, [=](const FApiRatingMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
						if (InResponseCode != EApi2ResponseCode::Ok) {
							VeLogErrorFunc("failed to request: %s", *InError);
						}
						if (InResponseCode == EApi2ResponseCode::Ok) {
							LikeButtonWidget->SetButtonPreset(EUIButtonImagePreset::Default);
							bIsPressLiked = false;
							MetadataRequest(Metadata.Id);
						}
					});

					FApiRatingMetadata LikeMetadata;
					LikeMetadata.Id = Metadata.Id;
					LikeMetadata.Value = 0;

					// 3. Make the request using the callback.
					RatingSubsystem->UpdateUnlike(LikeMetadata.Id, CallbackRef);
				}
			}
		});
	}

	if (IsValid(DisLikeButtonWidget)) {
		DisLikeButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			GET_MODULE_SUBSYSTEM(RatingSubsystem, Api2, Rating);
			if (RatingSubsystem) {
				if (bIsPressDisLiked == false) {
					// 1. Create a callback.
					const auto CallbackRef = MakeShared<FOnRatingRequestCompleted2>();

					// 2. Bind callback.
					CallbackRef->BindWeakLambda(this, [=](const FApiRatingMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
						if (InResponseCode != EApi2ResponseCode::Ok) {
							VeLogErrorFunc("failed to request: %s", *InError);
						}

						if (InResponseCode == EApi2ResponseCode::Ok) {
							DisLikeButtonWidget->SetButtonPreset(EUIButtonImagePreset::Danger);
							bIsPressDisLiked = true;
							MetadataRequest(Metadata.Id);
						}
					});

					FApiRatingMetadata LikeMetadata;
					LikeMetadata.Id = Metadata.Id;
					LikeMetadata.Value = -1;

					// 3. Make the request using the callback.
					RatingSubsystem->UpdateDisLike(LikeMetadata.Id, CallbackRef);
				} else {
					// 1. Create a callback.
					const auto CallbackRef = MakeShared<FOnRatingRequestCompleted2>();

					// 2. Bind callback.
					CallbackRef->BindWeakLambda(this, [=](const FApiRatingMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
						if (InResponseCode != EApi2ResponseCode::Ok) {
							VeLogErrorFunc("failed to request: %s", *InError);
						}
						if (InResponseCode == EApi2ResponseCode::Ok) {
							DisLikeButtonWidget->SetButtonPreset(EUIButtonImagePreset::Default);
							bIsPressDisLiked = false;
							MetadataRequest(Metadata.Id);
						}
					});

					FApiRatingMetadata LikeMetadata;
					LikeMetadata.Id = Metadata.Id;
					LikeMetadata.Value = 0;

					// 3. Make the request using the callback.
					RatingSubsystem->UpdateUnlike(LikeMetadata.Id, CallbackRef);
				}
			}
		});
	}
}

void UUIInspectorArtObjectPropertiesWidget::MetadataRequest(const FGuid& InId) {
	GET_MODULE_SUBSYSTEM(ObjectSubsystem, Api2, Object);
	if (ObjectSubsystem) {
		// 1. Create a callback.
		const auto CallbackPtr = MakeShared<FOnObjectRequestCompleted2>();

		// 2. Bind callback.
		CallbackPtr->BindWeakLambda(this, [=](const FApiObjectMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			if (InResponseCode != EApi2ResponseCode::Ok) {
				LogErrorF("failed to request: %s", *InError);
			}

			// SetIsProcessing(false);
			SetProperties(InMetadata);
		});

		// 3. Make the request using the callback.
		ObjectSubsystem->Get(InId, CallbackPtr);
	}
}


void UUIInspectorArtObjectPropertiesWidget::SetProperties(const FApiObjectMetadata& InMetadata) {
	Metadata = InMetadata;

	if (NameWidget) {
		NameWidget->SetText(FText::FromString(InMetadata.Name));
	}

	if (DescriptionWidget) {
		DescriptionWidget->SetText(FText::FromString(InMetadata.Description));
	}

	if (OwnerTextPropertyWidget) {
		if (InMetadata.Owner.Name.IsEmpty()) {
			OwnerTextPropertyWidget->SetVisible(false);
		} else {
			OwnerTextPropertyWidget->SetVisible(true);
			OwnerTextPropertyWidget->Value = FText::FromString(InMetadata.Owner.Name);
		}
	}

	if (ArtistTextPropertyWidget) {
		ArtistTextPropertyWidget->Value = FText::FromString(InMetadata.Artist);
	}

	if (SizeTextPropertyWidget) {
		if (InMetadata.Width > 0.0f && InMetadata.Height > 0.0f) {
			FNumberFormattingOptions NumberFormatOptions;
			NumberFormatOptions.MinimumFractionalDigits = 1;
			NumberFormatOptions.MaximumFractionalDigits = 1;
			SizeTextPropertyWidget->Value = FText::Format(LOCTEXT("SizeTextProperty", "{0} cm x {1} cm"), FText::AsNumber(InMetadata.Width, &NumberFormatOptions), FText::AsNumber(InMetadata.Height, &NumberFormatOptions));
		} else {
			SizeTextPropertyWidget->Value = FText::GetEmpty();
		}
	}

	if (TypeTextPropertyWidget) {
		TypeTextPropertyWidget->Value = FText::FromString(InMetadata.Type);
	}

	if (DateTextPropertyWidget) {
		DateTextPropertyWidget->Value = FText::FromString(InMetadata.Date);
	}

	if (DimensionsTextPropertyWidget) {
		DimensionsTextPropertyWidget->Value = FText::FromString(InMetadata.Dimensions);
	}

	if (MediumTextPropertyWidget) {
		MediumTextPropertyWidget->Value = FText::FromString(InMetadata.Medium);
	}

	if (SourceTextPropertyWidget) {
		SourceTextPropertyWidget->Value = FText::FromString(InMetadata.Source);
	}

	if (SourceUrlTextPropertyWidget) {
		SourceUrlTextPropertyWidget->Value = FText::FromString(InMetadata.SourceUrl);
	}

	if (LicenseTextPropertyWidget) {
		LicenseTextPropertyWidget->Value = FText::FromString(InMetadata.License);
	}

	if (CopyrightTextPropertyWidget) {
		CopyrightTextPropertyWidget->Value = FText::FromString(InMetadata.Copyright);
	}

	if (CreditTextPropertyWidget) {
		CreditTextPropertyWidget->Value = FText::FromString(InMetadata.Credit);
	}

	if (OriginTextPropertyWidget) {
		OriginTextPropertyWidget->Value = FText::FromString(InMetadata.Origin);
	}

	if (LocationTextPropertyWidget) {
		LocationTextPropertyWidget->Value = FText::FromString(InMetadata.Location);
	}

	if (LikeCountTextWidget) {
		LikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeGame", "ObjectLikeCountFormat", "{0}"), FText::AsNumber(InMetadata.TotalLikes)));
	}

	if (DislikeCountTextWidget) {
		DislikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeGame", "ObjectLikeCountFormat", "{0}"), FText::AsNumber(InMetadata.TotalDislikes)));
	}

	if (RatingTextWidget) {
		if (InMetadata.TotalLikes + InMetadata.TotalDislikes > 0) {
			const float Value = (static_cast<float>(InMetadata.TotalLikes) / (InMetadata.TotalLikes + InMetadata.TotalDislikes) * 5 - 1) / 4 * 5;
			RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeGame", "ObjectRatingFormat", "{0}"), FText::AsNumber(Value)));
		} else {
			RatingTextWidget->SetText(NSLOCTEXT("VeGame", "ObjectRatingNone", "-"));
		}
	}

	if (Metadata.Liked == 1) {
		LikeButtonWidget->SetButtonPreset(EUIButtonImagePreset::Action);
		DisLikeButtonWidget->SetButtonPreset(EUIButtonImagePreset::Default);
		bIsPressLiked = true;
		bIsPressDisLiked = false;
	} else if (Metadata.Liked == -1) {
		DisLikeButtonWidget->SetButtonPreset(EUIButtonImagePreset::Danger);
		LikeButtonWidget->SetButtonPreset(EUIButtonImagePreset::Default);
		bIsPressDisLiked = true;
		bIsPressLiked = false;
	} else {
		LikeButtonWidget->SetButtonPreset(EUIButtonImagePreset::Default);
		DisLikeButtonWidget->SetButtonPreset(EUIButtonImagePreset::Default);
		bIsPressLiked = false;
		bIsPressDisLiked = false;
	}
}


#undef LOCTEXT_NAMESPACE
