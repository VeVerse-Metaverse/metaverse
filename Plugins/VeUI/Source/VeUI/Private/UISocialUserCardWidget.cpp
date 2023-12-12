// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISocialUserCardWidget.h"

#include "VeApi.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "AnimatedTexture2D.h"


#define LOCTEXT_NAMESPACE "VeUI"


void UUISocialUserCardWidget::SetUserMetadata(const FApiUserMetadata& InUserMetadata) {
	UserMetadata = InUserMetadata;
	Refresh();
}

void UUISocialUserCardWidget::Refresh() {
	// Clear cached image
	if (DefaultAvatar) {
		SetTexture(DefaultAvatar);
	}

	if (NameTextWidget) {
		if (UserMetadata.Name.IsEmpty()) {
			NameTextWidget->SetText(NSLOCTEXT("VeUI", "UserNameUnknown", "Unnamed"));
		} else {
			NameTextWidget->SetText(FText::FromString(UserMetadata.Name));
		}
	}

	SetStatus(UserMetadata.GetStatus());

	if (LevelTextWidget) {
		LevelTextWidget->SetText(FText::AsNumber(UserMetadata.Level));
	}

	if (RankTextWidget) {
		const FString rank = UserMetadata.GetRank();
		if (rank.IsEmpty()) {
			RankTextWidget->SetText(NSLOCTEXT("VeUI", "UserRankUnknown", "Unknown"));
		} else {
			RankTextWidget->SetText(FText::FromString(rank));
		}
	}

	if (ExperienceBarWidget) {
		ExperienceBarWidget->SetPercent(UserMetadata.GetLevelExperience());
	}

	IApiBatchQueryRequestMetadata RequestMetadata;
	RequestAvatars(RequestMetadata);


	// const auto PreviewImage = User->Files.FindByPredicate([=](const FApiFileMetadata& File) {
	// 	return File.Type == EApiFileType::ImageAvatar;
	// });

	// const FString DefaultAvatarUrl = User->GetDefaultAvatarUrl();
	// if (PreviewImage && !PreviewImage->Url.IsEmpty()) {
	// 	SetIsLoading(true);
	// 	RequestTexture(PreviewImage->Url, TEXTUREGROUP_UI);
	// } else if (!DefaultAvatarUrl.IsEmpty()) {
	// 	SetIsLoading(true);
	// 	RequestTexture(DefaultAvatarUrl);
	// }

}

void UUISocialUserCardWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		//	SetIsLoading(false);
		return;
	}

	// SetTexture(InResult.Texture);
	ASSIGN_WIDGET_TEXTURE(AvatarImageWidget, InResult);
	// // todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
	// RUN_DELAYED(0.1f, this, [this](){
	// 			SetIsLoading(false);
	// 			});
}

void UUISocialUserCardWidget::RequestAvatars(const IApiBatchRequestMetadata& RequestMetadata) {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api, Auth);
	if (AuthSubsystem) {
		if (!AuthSubsystem->IsLoggedIn()) {
			return;
		}
	}

	if (!UserMetadata.Id.IsValid()) {
		return;
	}

	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiUserSubsystem> UserSubsystem = ApiModule->GetUserSubsystem()) {
			{
				// 1. Create a callback.
				TSharedPtr<FOnFileBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnFileBatchRequestCompleted());

				// 2. Bind callback.
				CallbackPtr->BindWeakLambda(this, [=](const FApiFileBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
					if (!bInSuccessful) {
						LogErrorF("failed to request: %s", *InError);
					}

					// Finish processing.
					SetIsProcessing(false);

					OnAvatarsLoaded(InMetadata, bInSuccessful, InError);

					CallbackPtr.Reset();
				});

				// 3. Make the request using the callback.
				if (!UserSubsystem->GetAvatarBatch(UserMetadata.Id, RequestMetadata, CallbackPtr)) {
					LogErrorF("failed to request");
				} else {
					SetIsProcessing(true);
				}
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(User)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUISocialUserCardWidget::OnAvatarsLoaded(const FApiFileBatchMetadata& Metadata, bool bSuccessful, const FString& Error) {
	FString AvatarUrl;

	if (bSuccessful) {
		if (Metadata.Entities.IsValidIndex(0)) {
			AvatarUrl = Metadata.Entities[0]->Url;
		} else {
			AvatarUrl = UserMetadata.GetDefaultAvatarUrl();
		}
	} else {
		AvatarUrl = UserMetadata.GetDefaultAvatarUrl();

		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api, Auth);
		if (AuthSubsystem) {
			if (AuthSubsystem->IsLoggedIn()) {
				// GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
				// if (NotificationSubsystem) {
				// 	FUINotificationData NotificationData;
				// 	NotificationData.Type = EUINotificationType::Failure;
				// 	NotificationData.Header = LOCTEXT("LoadAvatarBatchError", "Error");
				// 	NotificationData.Message = FText::FromString("Failed to load avatar");
				// 	NotificationSubsystem->AddNotification(NotificationData);
				// }
				LogWarningF("failed to load avatar");
			}
		}
	}

	if (!AvatarUrl.IsEmpty()) {
		RequestTexture(this, AvatarUrl, TEXTUREGROUP_UI);
	}
}

void UUISocialUserCardWidget::SetIsProcessing(bool bInIsLoading) {}

void UUISocialUserCardWidget::SetStatus(EApiPresenceStatus Status) {
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

void UUISocialUserCardWidget::SetTexture(UTexture2D* Texture) {
	AvatarImageWidget->SetBrushFromTexture(Texture);

	const FVector2D Size = {
		static_cast<float>(Texture->GetSizeX()),
		static_cast<float>(Texture->GetSizeY())
	};

	AvatarImageWidget->SetDesiredSizeOverride(Size);
}


#undef LOCTEXT_NAMESPACE
