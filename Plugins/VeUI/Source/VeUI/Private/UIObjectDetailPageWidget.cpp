// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIObjectDetailPageWidget.h"

#include "VeShared.h"
#include "VeUI.h"
#include "Subsystems/UIDialogSubsystem.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIImageButtonWidget.h"
#include "Components/UIButtonImageWidget.h"
#include "UIPageManagerWidget.h"
#include "UIPreviewImagePageWidget.h"
#include "UIObjectFormPageWidget.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2ObjectSubsystem.h"
#include "Api2RatingSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIObjectDetailPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PreviewImageWidget) {
		PreviewImageWidget->OnClicked.AddWeakLambda(this, [=]() {
			const TArray<EApiFileType> Types = {
				EApiFileType::ImagePreview,
				EApiFileType::ImageFull,
				// EApiFileType::TextureDiffuse,
			};

			const auto* FileMetadata = FindFileMetadata(Metadata.Files, Types);
			if (FileMetadata && !FileMetadata->Url.IsEmpty()) {
				if (auto Page = GetPageManager()->OpenPage<UUIPreviewImagePageWidget>(PreviewPageClass, FileMetadata->Url)) {
					Page->SetParentPage(this);
				}
			}
		});
	}

	if (IsValid(EditButtonWidget)) {
		EditButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(FormPageClass)) {
				if (auto Page = GetPageManager()->OpenPage<UUIObjectFormPageWidget>(FormPageClass)) {
					Page->SetParentPage(this);
					Page->SetMetadata(Metadata);
				}
			}
		});
	}

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
							LogErrorF("failed to request: %s", *InError);
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
							LogErrorF("failed to request: %s", *InError);
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
							LogErrorF("failed to request: %s", *InError);
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
							LogErrorF("failed to request: %s", *InError);
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

	if (IsValid(DeleteButtonWidget)) {
		DeleteButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectDetailPageWidget::OnDeleteButtonClicked);
	}
}

void UUIObjectDetailPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	const FGuid OptionsId = FGuid(GetOptions());
	if (!OptionsId.IsValid()) {
		LogWarningF("Filed options value");
		return;
	}

	UpdateButtons();
	MetadataRequest(OptionsId);
}

void UUIObjectDetailPageWidget::MetadataRequest(const FGuid& InId) {
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
			OnMetadataRequestCompleted(InMetadata, InResponseCode, InError);
		});

		// 3. Make the request using the callback.
		ObjectSubsystem->Get(InId, CallbackPtr);
	}
}

void UUIObjectDetailPageWidget::OnMetadataRequestCompleted(const FApiObjectMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& Error) {
	if (InResponseCode == EApi2ResponseCode::Ok) {
		SetMetadata(InMetadata);
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadObjectError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a object data.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIObjectDetailPageWidget::SetMetadata(const FApiObjectMetadata& InMetadata) {
	Metadata = InMetadata;

	FText Name = (InMetadata.Name.IsEmpty()) ? NSLOCTEXT("VeUI", "ObjectNameUnknown", "Unnamed") : FText::FromString(InMetadata.Name);
	SetTitle(Name);
	if (NameTextWidget) {
		NameTextWidget->SetText(Name);
	}

	if (PreviewImageWidget) {
		const TArray<EApiFileType> Types = {
			EApiFileType::ImagePreview,
			EApiFileType::ImageFull,
			// EApiFileType::TextureDiffuse,
		};

		if (const auto* FileMetadata = FindFileMetadata(Metadata.Files, Types)) {
			PreviewImageWidget->ShowImage(FileMetadata->Url);
		} else {
			PreviewImageWidget->ShowDefaultImage();
		}
	}

	if (DescriptionTextWidget) {
		if (Metadata.Description.IsEmpty()) {
			DescriptionTextWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata.Description));
		}
	}

	if (OwnerTextWidget) {
		if (Metadata.Owner.Name.IsEmpty()) {
			OwnerTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectOwnerUnknown", ""));
		} else {
			OwnerTextWidget->SetText(FText::FromString(Metadata.Owner.Name));
		}
	}

	if (ViewCountTextWidget) {
		ViewCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ObjectViewCountFormat", "{0}"), FText::AsNumber(Metadata.Views)));
	}

	if (TypeTextWidget) {
		if (Metadata.Type.IsEmpty()) {
			TypeTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectTypeUnknown", ""));
		} else {
			TypeTextWidget->SetText(FText::FromString(Metadata.Type));
		}
	}

	if (ArtistTextWidget) {
		if (Metadata.Artist.IsEmpty()) {
			ArtistTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectArtistUnknown", ""));
		} else {
			ArtistTextWidget->SetText(FText::FromString(Metadata.Artist));
		}
	}

	if (DateTextWidget) {
		if (Metadata.Date.IsEmpty()) {
			DateTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectDateUnknown", ""));
		} else {
			DateTextWidget->SetText(FText::FromString(Metadata.Date));
		}
	}

	if (MediumTextWidget) {
		if (Metadata.Medium.IsEmpty()) {
			MediumTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectMediumUnknown", ""));
		} else {
			MediumTextWidget->SetText(FText::FromString(Metadata.Medium));
		}
	}

	if (SourceTextWidget) {
		if (Metadata.Source.IsEmpty()) {
			SourceTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectSourceUnknown", ""));
		} else {
			SourceTextWidget->SetText(FText::FromString(Metadata.Source));
		}
	}

	if (SourceUrlTextWidget) {
		if (Metadata.SourceUrl.IsEmpty()) {
			SourceUrlTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectSourceUrlUnknown", ""));
		} else {
			SourceUrlTextWidget->SetText(FText::FromString(Metadata.SourceUrl));
		}
	}

	if (LicenseTextWidget) {
		if (Metadata.License.IsEmpty()) {
			LicenseTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectLicenseUnknown", ""));
		} else {
			LicenseTextWidget->SetText(FText::FromString(Metadata.License));
		}
	}

	if (CopyrightTextWidget) {
		if (Metadata.Copyright.IsEmpty()) {
			CopyrightTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectCopyrightUnknown", ""));
		} else {
			CopyrightTextWidget->SetText(FText::FromString(Metadata.Copyright));
		}
	}

	if (CreditTextWidget) {
		if (Metadata.Credit.IsEmpty()) {
			CreditTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectCreditUnknown", ""));
		} else {
			CreditTextWidget->SetText(FText::FromString(Metadata.Credit));
		}
	}

	if (OriginTextWidget) {
		if (Metadata.Origin.IsEmpty()) {
			OriginTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectOriginUnknown", ""));
		} else {
			OriginTextWidget->SetText(FText::FromString(Metadata.Origin));
		}
	}

	if (LocationTextWidget) {
		if (Metadata.Location.IsEmpty()) {
			LocationTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectLocationUnknown", ""));
		} else {
			LocationTextWidget->SetText(FText::FromString(Metadata.Location));
		}
	}

	if (WidthTextWidget) {
		WidthTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ObjectWidthFormat", "{0}"), FText::AsNumber(Metadata.Width)));
	}

	if (HeightTextWidget) {
		HeightTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ObjectHeightFormat", "{0}"), FText::AsNumber(Metadata.Height)));
	}

	if (LikeCountTextWidget) {
		LikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ObjectLikeCountFormat", "{0}"), FText::AsNumber(Metadata.TotalLikes)));
	}

	if (DislikeCountTextWidget) {
		DislikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ObjectLikeCountFormat", "{0}"), FText::AsNumber(Metadata.TotalDislikes)));
	}

	if (RatingTextWidget) {
		if (Metadata.TotalLikes + Metadata.TotalDislikes > 0) {
			const float Value = (static_cast<float>(Metadata.TotalLikes) / (Metadata.TotalLikes + Metadata.TotalDislikes) * 5 - 1) / 4 * 5;
			RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ObjectRatingFormat", "{0}"), FText::AsNumber(Value)));
		} else {
			RatingTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectRatingNone", "-"));
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

	GET_MODULE_SUBSYSTEM(AuthSubsystem2, Api2, Auth);
	if (AuthSubsystem2) {
		UserIsOwner = AuthSubsystem2->GetUserId() == InMetadata.Owner.Id;
	} else {
		UserIsOwner = false;
	}

	UpdateButtons();
}

void UUIObjectDetailPageWidget::UpdateButtons() {
	const bool Edit = UserIsOwner;
	const bool Delete = UserIsOwner;

	EditButtonWidget->SetVisibility(Edit ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	EditButtonWidget->SetIsEnabled(Edit);

	DeleteButtonWidget->SetVisibility(Delete ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	DeleteButtonWidget->SetIsEnabled(Delete);
}

void UUIObjectDetailPageWidget::OnDeleteButtonClicked() {
	GET_MODULE_SUBSYSTEM(DialogSubsystem, UI, Dialog);
	if (DialogSubsystem) {
		FUIDialogData DialogData;
		DialogData.Type = EUIDialogType::OkCancel;
		DialogData.HeaderText = LOCTEXT("ConfirmObjectDeleteDialogHeader", "Confirm delete");
		DialogData.MessageText = LOCTEXT("ConfirmObjectDeleteDialogMessage", "Do you really want to delete this object?");
		DialogData.FirstButtonText = LOCTEXT("ConfirmObjectDeleteDialogButtonOk", "OK");
		DialogData.SecondButtonText = LOCTEXT("ConfirmObjectDeleteDialogButtonCancel", "Cancel");
		DialogData.bCloseOnButtonClick = true;

		FUIDialogButtonClicked OnDialogButtonClicked;
		OnDialogButtonClicked.BindWeakLambda(this, [this](UUIDialogWidget*, const uint8& InButtonIndex) {
			if (InButtonIndex == 0) {
				DeleteRequest();
			}
		});

		DialogSubsystem->ShowDialog(DialogData, OnDialogButtonClicked);
	}
}

void UUIObjectDetailPageWidget::DeleteRequest() {
	GET_MODULE_SUBSYSTEM(ObjectSubsystem, Api2, Object);
	if (ObjectSubsystem) {
		const auto CallbackPtr = MakeShared<FOnObjectRequestCompleted2>();

		CallbackPtr->BindWeakLambda(this, [=](const FApiObjectMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			if (InResponseCode != EApi2ResponseCode::Ok) {
				LogErrorF("failed to request: %s", *InError);
			}

			// SetIsProcessing(false);
			OnDeleteRequestCompleted(InResponseCode, InError);
		});

		ObjectSubsystem->Delete(Metadata.Id, CallbackPtr);
	}

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Header = LOCTEXT("DeleteObjectErrorNotificationHeader", "Error");
		NotificationData.Message = LOCTEXT("DeleteObjectErrorNotificationMessage", "Failed to delete an object.");
		NotificationSubsystem->AddNotification(NotificationData);
	}
}

void UUIObjectDetailPageWidget::OnDeleteRequestCompleted(const EApi2ResponseCode InResponseCode, const FString& Error) {
	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;

		if (InResponseCode == EApi2ResponseCode::Ok) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("DeleteObjectSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("DeleteObjectSuccessNotificationMessage", "Successfully deleted an object.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("DeleteObjectErrorNotificationHeader", "Error");
			NotificationData.Message = FText::Format(LOCTEXT("DeleteObjectErrorNotificationMessage", "Failed to delete an object: {0}"), FText::FromString(Error));
		}

		NotificationSubsystem->AddNotification(NotificationData);
	}

	if (InResponseCode == EApi2ResponseCode::Ok) {
		GetPageManager()->ShowPrevPage(true);
		GetPageManager()->RemovePage(this);
	}
}


#undef LOCTEXT_NAMESPACE
