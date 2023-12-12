// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISocialFriendInviteWidget.h"

#include "VeApi.h"
#include "ApiEntityMetadata.h"
#include "ApiUserMetadataObject.h"
#include "UINotificationData.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "AnimatedTexture2D.h"
#include "Api2UserSubsystem.h"
#include "VeApi2.h"


#define LOCTEXT_NAMESPACE "VeUI"


void UUISocialFriendInviteWidget::RegisterCallbacks() {

	if (!OnAcceptButtonClickedDelegateHandle.IsValid()) {
		if (AcceptButtonWidget) {
			OnAcceptButtonClickedDelegateHandle = AcceptButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISocialFriendInviteWidget::OnAcceptButtonClicked);
		}
	}

	// if (!OnIgnoreButtonClickedDelegateHandle.IsValid()) {
	// 	if (IgnoreButtonWidget) {
	// 		OnIgnoreButtonClickedDelegateHandle = IgnoreButtonWidget->OnButtonClicked.AddUObject(this, &UUISocialFriendInviteWidget::OnIgnoreButtonClicked);
	// 	}
	// }
	//
	// if (!OnBlockButtonClickedDelegateHandle.IsValid()) {
	// 	if (BlockButtonWidget) {
	// 		OnBlockButtonClickedDelegateHandle = BlockButtonWidget->OnButtonClicked.AddUObject(this, &UUISocialFriendInviteWidget::OnBlockButtonClicked);
	// 	}
	// }
}

void UUISocialFriendInviteWidget::UnregisterCallbacks() {
	if (AcceptButtonWidget) {
		if (OnAcceptButtonClickedDelegateHandle.IsValid()) {
			AcceptButtonWidget->GetOnButtonClicked().Remove(OnAcceptButtonClickedDelegateHandle);
			OnAcceptButtonClickedDelegateHandle.Reset();
		}
	}

	// if (IgnoreButtonWidget) {
	// 	if (OnIgnoreButtonClickedDelegateHandle.IsValid()) {
	// 		IgnoreButtonWidget->OnButtonClicked.Remove(OnIgnoreButtonClickedDelegateHandle);
	// 		OnIgnoreButtonClickedDelegateHandle.Reset();
	// 	}
	// }
	//
	// if (BlockButtonWidget) {
	// 	if (OnBlockButtonClickedDelegateHandle.IsValid()) {
	// 		BlockButtonWidget->OnButtonClicked.Remove(OnBlockButtonClickedDelegateHandle);
	// 		OnBlockButtonClickedDelegateHandle.Reset();
	// 	}
	// }
}

void UUISocialFriendInviteWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	if (const UApiUserMetadataObject* UserObject = Cast<UApiUserMetadataObject>(ListItemObject)) {
		TSharedPtr<FApiUserMetadata> UserMetadata = UserObject->Metadata;
		if (UserMetadata.IsValid()) {

			UserId = UserMetadata->Id;

			if (NameTextWidget) {
				if (UserMetadata->Name.IsEmpty()) {
					NameTextWidget->SetText(NSLOCTEXT("VeUI", "UserNameUnknown", "Unnamed"));
				} else {
					NameTextWidget->SetText(FText::FromString(UserMetadata->Name));
				}
			}

			SetStatus(UserMetadata->GetStatus());

			if (LevelTextWidget) {
				LevelTextWidget->SetText(FText::AsNumber(UserMetadata->Level));
			}

			if (RankTextWidget) {
				const FString rank = UserMetadata->GetRank();
				if (rank.IsEmpty()) {
					RankTextWidget->SetText(NSLOCTEXT("VeUI", "UserRankUnknown", "Unknown"));
				} else {
					RankTextWidget->SetText(FText::FromString(rank));
				}
			}

			const auto PreviewImage = UserMetadata->Files.FindByPredicate([=](const FApiFileMetadata& File) {
				return File.Type == EApiFileType::ImageAvatar;
			});

			const FString DefaultAvatarUrl = UserMetadata->GetDefaultAvatarUrl();
			if (PreviewImage && !PreviewImage->Url.IsEmpty()) {
				SetIsLoading(true);
				RequestTexture(this, PreviewImage->Url, TEXTUREGROUP_UI);
			} else if (!DefaultAvatarUrl.IsEmpty()) {
				SetIsLoading(true);
				RequestTexture(this, DefaultAvatarUrl, TEXTUREGROUP_UI);
			}

			// if (TextMutualFriendNumber) {
			// 	if (User->MutualFriends()) {
			// 		TextMutualFriendNumber->SetText(NSLOCTEXT("VeUI", "UserMutual FriendsUnknown", "Unnamed"));
			// 	} else {
			// 		TextMutualFriendNumber->SetText(FText::AsNumber(User->MutualFriends()));
			// 	}
			// }
		}
	}
}

void UUISocialFriendInviteWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		SetIsLoading(false);
		return;
	}

	// SetTexture(InResult.Texture);
	ASSIGN_WIDGET_TEXTURE(AvatarImageWidget, InResult);

	// SetIsLoading(false);

	// todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
	// RUN_DELAYED(0.1f, this, [this](){
	// 			SetIsLoading(false);
	// 			});
}

void UUISocialFriendInviteWidget::SetStatus(EApiPresenceStatus Status) {
	FLinearColor Color;
	FText Text;

	switch (Status) {
	case EApiPresenceStatus::Offline:
		Color = FColor(0xff808080);
		Text = NSLOCTEXT("VeUI", "OfflineUserStatus", "Offline");
		break;
	case EApiPresenceStatus::Available:
		Color = FColor(0xff0080ff);
		Text = NSLOCTEXT("VeUI", "OnlineUserStatus", "Online");
		break;
	case EApiPresenceStatus::Playing:
		Color = FColor(0xff00e015);
		Text = NSLOCTEXT("VeUI", "PlayingUserStatus", "Playing");
		break;
	}

	if (AvatarBorderWidget) {
		AvatarBorderWidget->SetBrushColor(Color);
	}
	if (StatusTextWidget) {
		StatusTextWidget->SetColorAndOpacity(Color);
		StatusTextWidget->SetText(Text);
	}
}

void UUISocialFriendInviteWidget::SetIsLoading(bool bInIsLoading) {
	// if (LoadingWidget) {
	// 	if (bInIsLoading) {
	// 		LoadingWidget->Show();
	// 	} else {
	// 		LoadingWidget->Hide();
	// 	}
	// }
}

void UUISocialFriendInviteWidget::SetTexture(UTexture2D* Texture) {
	AvatarImageWidget->SetBrushFromTexture(Texture);

	const FVector2D Size = {
		static_cast<float>(Texture->GetSizeX()),
		static_cast<float>(Texture->GetSizeY())
	};

	AvatarImageWidget->SetDesiredSizeOverride(Size);
}

void UUISocialFriendInviteWidget::OnAcceptButtonClicked() {
	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (UserSubsystem) {
		// 1. Create a callback.
		const TSharedRef<FOnGenericRequestCompleted2> CallbackPtr = MakeShareable(new FOnGenericRequestCompleted2());

		// 2. Bind callback.
		CallbackPtr->BindWeakLambda(this, [this](const EApi2ResponseCode InResponseCode, const FString& InError) {
			const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;

			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// Finish processing.
			// SetIsProcessing(false);

			OnFollowRequestCompleted(bInSuccessful, InError);
		});

		// 3. Make the request using the callback.
		UserSubsystem->Follow(UserId, CallbackPtr);
	}
}

void UUISocialFriendInviteWidget::OnFollowRequestCompleted(bool bInSuccessful, const FString& InError) {
	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;

		if (bInSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("SuccessFriendNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("SuccessFriendNotificationMessage", "You accepted a friend request.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadUserBatchError", "Error");
			NotificationData.Message = FText::FromString("Failed to accept a friend request.");
		}

		NotificationSubsystem->AddNotification(NotificationData);
	}
}

// void UUISocialFriendInviteWidget::OnIgnoreButtonClicked() {
//
// 	FUINotificationData NotificationData;
// 	NotificationData.Lifetime = 3.f;
// 	NotificationData.Header = LOCTEXT("IgnoreFriendNotificationHeader", "Ignore");
// 	NotificationData.Type = EUINotificationType::Success;
// 	NotificationData.Message = LOCTEXT("IgnoreFriendNotificationMessage", "You ignored a friend request.");
// 	//LogWarningF("Failed to create an object: %s", *Error);
// 	GetHUD()->AddNotification(NotificationData);
// }
//
// void UUISocialFriendInviteWidget::OnBlockButtonClicked() {
//
// 	FUINotificationData NotificationData;
// 	NotificationData.Lifetime = 3.f;
// 	NotificationData.Header = LOCTEXT("BlockFriendNotificationHeader", "Block");
// 	NotificationData.Type = EUINotificationType::Success;
// 	NotificationData.Message = LOCTEXT("BlockFriendNotificationMessage", "You blocked a user.");
// 	//LogWarningF("Failed to create an object: %s", *Error);
// 	GetHUD()->AddNotification(NotificationData);
// }


#undef LOCTEXT_NAMESPACE
