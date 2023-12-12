// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIUserDetailPageWidget.h"

#include "VeApi.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2UserSubsystem.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Components/UIButtonWidget.h"
#include "Components/TextBlock.h"
#include "Components/UIImageButtonWidget.h"
#include "UIPageManagerWidget.h"
#include "UIPreviewImagePageWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIUserDetailPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PreviewImageWidget) {
		PreviewImageWidget->OnClicked.AddWeakLambda(this, [this]() {
			if (auto Page = GetPageManager()->OpenPage<UUIPreviewImagePageWidget>(PreviewPageClass, PreviewImageWidget->GetURL())) {
				Page->SetParentPage(this);
			}
		});
	}

	if (FollowButtonWidget) {
		FollowButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIUserDetailPageWidget::FollowButton_OnButtonClicked);
	}

	if (UnfollowButtonWidget) {
		UnfollowButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIUserDetailPageWidget::UnfollowButton_OnButtonClicked);
	}
}

void UUIUserDetailPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	bIsCurrentUser = false;

	const FGuid OptionsId = FGuid(GetOptions());
	if (!OptionsId.IsValid()) {
		LogWarningF("Filed options value");
		return;
	}

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		bIsCurrentUser = OptionsId == AuthSubsystem->GetUserId();
	}

	UpdateButtons();
	MetadataRequest(OptionsId);
}

void UUIUserDetailPageWidget::MetadataRequest(const FGuid& InId) {
	GET_MODULE_SUBSYSTEM(UserSubsystem, Api, User);
	if (UserSubsystem) {
		// 1. Create a callback.
		TSharedPtr<FOnUserRequestCompleted> CallbackPtr = MakeShareable(new FOnUserRequestCompleted());

		// 2. Bind callback.
		CallbackPtr->BindWeakLambda(this, [=](const FApiUserMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// SetIsProcessing(false);
			OnMetadataRequestCompleted(InMetadata, bInSuccessful, InError);

			CallbackPtr.Reset();
		});

		// 3. Make the request using the callback.
		if (UserSubsystem->GetUser(InId, CallbackPtr)) {
			// SetIsProcessing(true);
		} else {
			LogErrorF("failed to request");
		}
	}
}

void UUIUserDetailPageWidget::OnMetadataRequestCompleted(const FApiUserMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		SetMetadata(InMetadata);
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadUserError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a user data.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIUserDetailPageWidget::SetMetadata(const FApiUserMetadata& InMetadata) {
	Metadata = InMetadata;

	FText Name = (InMetadata.Name.IsEmpty()) ? NSLOCTEXT("VeUI", "UserNameUnknown", "Unnamed") : FText::FromString(InMetadata.Name);
	SetTitle(Name);
	if (NameTextWidget) {
		NameTextWidget->SetText(Name);
	}

	if (DescriptionTextWidget) {
		if (InMetadata.Description.IsEmpty()) {
			DescriptionTextWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			DescriptionTextWidget->SetText(FText::FromString(InMetadata.Description));
		}
	}

	if (ViewCountTextWidget) {
		ViewCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "UserViewCountFormat", "{0}"), FText::AsNumber(InMetadata.Views)));
	}

	// PreviewImage
	if (PreviewImageWidget) {
		if (!PreviewImageWidget->ShowImage(InMetadata.Files, EApiFileType::ImageAvatar)) {
			PreviewImageWidget->ShowImage(InMetadata.GetDefaultAvatarUrl());
		}
	}

	// if (Metadata) {
	RequestIsFollowing();
	// }

	if (LikeCountTextWidget) {
		LikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "UserLikeCountFormat", "{0}"), FText::AsNumber(InMetadata.TotalLikes)));
	}

	if (DislikeCountTextWidget) {
		DislikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "UserLikeCountFormat", "{0}"), FText::AsNumber(InMetadata.TotalDislikes)));
	}

	if (RatingTextWidget) {
		const float Rating = InMetadata.GetRating();
		if (Rating < 0.f) {
			RatingTextWidget->SetText(NSLOCTEXT("VeUI", "UserRatingNone", "-"));
		} else {
			RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "UserRatingFormat", "{0}"), Rating));
		}
	}
}

void UUIUserDetailPageWidget::FollowButton_OnButtonClicked() {
	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (UserSubsystem) {
		if (Metadata.Id.IsValid()) {
			const TSharedRef<FOnGenericRequestCompleted2> CallbackPtr = MakeShareable(new FOnGenericRequestCompleted2());

			CallbackPtr->BindWeakLambda(this, [this](const EApi2ResponseCode InResponseCode, const FString& InError) {
				const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;

				if (!bInSuccessful) {
					LogErrorF("failed to request: %s", *InError);
				}

				// SetIsProcessing(false);
				OnFollowRequestCompleted(bInSuccessful, InError);
			});

			UserSubsystem->Follow(Metadata.Id, CallbackPtr);
		}
	}
}

void UUIUserDetailPageWidget::OnFollowRequestCompleted(const bool bInSuccessful, const FString& InError) {
	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;

		if (bInSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("FollowRequestSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("FollowRequestSuccessNotificationMessage", "Following.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("FollowRequestErrorNotificationHeader", "Error");
			NotificationData.Message = FText::Format(LOCTEXT("FollowRequestErrorNotificationMessage", "Failed to follow: {0}"), FText::FromString(InError));
		}

		NotificationSubsystem->AddNotification(NotificationData);
	}

	RequestIsFollowing();
}

void UUIUserDetailPageWidget::UnfollowButton_OnButtonClicked() {
	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (UserSubsystem) {
		if (Metadata.Id.IsValid()) {
			const TSharedRef<FOnGenericRequestCompleted2> CallbackPtr = MakeShareable(new FOnGenericRequestCompleted2());

			CallbackPtr->BindWeakLambda(this, [this](const EApi2ResponseCode InResponseCode, const FString& InError) {
				const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;

				if (!bInSuccessful) {
					LogErrorF("failed to request: %s", *InError);
				}

				// SetIsProcessing(false);
				OnUnfollowRequestCompleted(bInSuccessful, InError);
			});

			UserSubsystem->Unfollow(Metadata.Id, CallbackPtr);
		}
	}
}

void UUIUserDetailPageWidget::OnUnfollowRequestCompleted(bool bInSuccessful, const FString& InError) {
	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;

		if (bInSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("UnfollowRequestSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("UnfollowRequestSuccessNotificationMessage", "Unfollowing.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("UnfollowRequestErrorNotificationHeader", "Error");
			NotificationData.Message = FText::Format(LOCTEXT("UnfollowRequestErrorNotificationMessage", "Failed to unfollow: {0}"), FText::FromString(InError));
		}

		NotificationSubsystem->AddNotification(NotificationData);
	}

	RequestIsFollowing();
}

void UUIUserDetailPageWidget::RequestIsFollowing() {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (!AuthSubsystem) {
		return;
	}

	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (UserSubsystem) {
		const FApiUserMetadata AuthUserMetadata = AuthSubsystem->GetUserMetadata();
		if (Metadata.Id.IsValid() && AuthUserMetadata.Id.IsValid()) {
			const TSharedRef<FOnOkRequestCompleted2> CallbackPtr = MakeShareable(new FOnOkRequestCompleted2());

			CallbackPtr->BindWeakLambda(this, [this](const bool bInOk , const EApi2ResponseCode InResponseCode, const FString& InError) {
				const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;

				if (!bInSuccessful) {
					LogErrorF("failed to request: %s", *InError);
				}

				// SetIsProcessing(false);
				OnIsFollowingRequestCompleted(bInOk, bInSuccessful, InError);
			});

			UserSubsystem->IsFollowing(AuthUserMetadata.Id, Metadata.Id, CallbackPtr);
		}
	}
}

void UUIUserDetailPageWidget::OnIsFollowingRequestCompleted(const bool bInOk, const bool bInSuccessful, const FString& InError) {
	bIsFollowing = bInOk;
	UpdateButtons();
}

void UUIUserDetailPageWidget::UpdateButtons() {

	const bool Follow = !bIsCurrentUser && !bIsFollowing;
	const bool Unfollow = !bIsCurrentUser && bIsFollowing;

	FollowButtonWidget->SetVisibility(Follow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	FollowButtonWidget->SetIsEnabled(Follow);

	UnfollowButtonWidget->SetVisibility(Unfollow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	UnfollowButtonWidget->SetIsEnabled(Unfollow);

	//
	// if (FollowersButtonWidget) {
	// 	//if (bisFollowers)
	// 	//FollowersButtonWidget->SetVisibility(ESlateVisibility::Visible);
	// 	FollowersButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	// 	FollowersButtonWidget->SetIsEnabled(false);		
	// }
	//
	// if (FollowingButtonWidget) {
	// 	//if (bisMFollowing)
	// 	//FollowingButtonWidget->SetVisibility(ESlateVisibility::Visible);
	// 	FollowingButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	// 	FollowingButtonWidget->SetIsEnabled(false);		
	// }
	//
	// if (MuteButtonWidget) {
	// 	//if (bisMuted)
	// 	//MuteButtonWidget->SetVisibility(ESlateVisibility::Visible);
	// 	MuteButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	// 	MuteButtonWidget->SetIsEnabled(false);		
	// }
	//
	// if (UnmuteButtonWidget) {
	// 	//if (bisMuted)
	// 	//UnmuteButtonWidget->SetVisibility(ESlateVisibility::Visible);
	// 	UnmuteButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	// 	UnmuteButtonWidget->SetIsEnabled(false);
	// }
	//
	// if (DeactivateButtonWidget) {
	// 	//if (bisMuted)
	// 	//DeactivateButtonWidget->SetVisibility(ESlateVisibility::Visible);
	// 	DeactivateButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	// 	DeactivateButtonWidget->SetIsEnabled(false);
	// }
	// 	
	// if (ActivateButtonWidget) {
	// 	//if (bisMuted)
	// 	//ActivateButtonWidget->SetVisibility(ESlateVisibility::Visible);
	// 	ActivateButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	// 	ActivateButtonWidget->SetIsEnabled(false);
	// }
	// 		
	// if (BlockButtonWidget) {
	// 	//if (bisMuted)
	// 	//BlockButtonWidget->SetVisibility(ESlateVisibility::Visible);
	// 	BlockButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	// 	BlockButtonWidget->SetIsEnabled(false);
	// }
	// 		
	// if (UnblockButtonWidget) {
	// 	//if (bisMuted)
	// 	//UnblockButtonWidget->SetVisibility(ESlateVisibility::Visible);
	// 	UnblockButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
	// 	UnblockButtonWidget->SetIsEnabled(false);
	// }
	//
}


#undef LOCTEXT_NAMESPACE
