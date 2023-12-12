// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIUserDetailWidget.h"
#include "ApiUserMetadata.h"
#include "ApiUserSubsystem.h"
#include "UINotificationData.h"
#include "VeApi.h"
#include "VeShared.h"
#include "AnimatedTexture2D.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIUserDetailWidget::NativeConstruct() {

	//SetIsLoading(false);
	bZoomImageWidgetVisible = true;
	//bFileLinkWidgetVisible = true;

	PreviewImageWidget->SetBrushFromTexture(DefaultTexture);

	NavigateToUserDetail(false, false);

	Super::NativeConstruct();
}

void UUIUserDetailWidget::RegisterCallbacks() {
	if (IsValid(CloseButtonWidget)) {
		if (!OnCloseButtonWidgetDelegateHandle.IsValid()) {
			OnCloseButtonWidgetDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosed.IsBound()) {
					OnClosed.Broadcast();
				}
			});
		}
	}

	// if (IsValid(ComboboxWidget)) {
	// ComboboxWidget->FindOptionIndex("Mute");	
	// }

	if (IsValid(FollowButtonWidget)) {
		if (!OnFollowButtonWidgetDelegateHandle.IsValid()) {
			OnFollowButtonWidgetDelegateHandle = FollowButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIUserDetailWidget::FollowButtonClicked);
		}
	}

	if (IsValid(UnfollowButtonWidget)) {
		if (!OnUnfollowButtonWidgetDelegateHandle.IsValid()) {
			OnUnfollowButtonWidgetDelegateHandle = UnfollowButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIUserDetailWidget::UnfollowButtonClicked);
		}
	}

	if (IsValid(PreviewImageButtonWidget)) {
		if (!OnPreviewButtonClickedDelegateHandle.IsValid()) {
			OnPreviewButtonClickedDelegateHandle = PreviewImageButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (IsValid(ZoomImageWidget)) {
					FApiFileMetadata* PreviewImage = Metadata->Files.FindByPredicate([=](const FApiFileMetadata& File) {
						return File.Type == EApiFileType::ImageAvatar;
					});
					if (PreviewImage) {
						ZoomImageWidget->SetMetadata(*PreviewImage);
					}
				}
				ShowZoomImageWidget();
			});
		}
	}
	
	if (IsValid(ZoomImageWidget)) {
		if (!OnZoomImageWidgetClosedDelegateHandle.IsValid())
			OnZoomImageWidgetClosedDelegateHandle = ZoomImageWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				HideZoomImageWidget();
			});
	}

	// if (IsValid(EditButtonWidget)) {
	// 	if (!OnEditButtonClickedClosedDelegateHandle.IsValid()) {
	// 		OnEditButtonClickedClosedDelegateHandle = EditButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
	// 			if (RPMAvatarLinkWidget) {
	// 				RPMAvatarLinkWidget->SetMetadata(Metadata);
	// 			}
	// 			NavigateToAddFileLink();
	// 		});
	// 	}
	// }
	
	// if (IsValid(RPMAvatarLinkWidget)) {
	// 	if (!OnAddFileLinkWidgetClosedDelegateHandle.IsValid())
	// 		OnAddFileLinkWidgetClosedDelegateHandle = RPMAvatarLinkWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
	// 			NavigateToUserDetail();
	// 		});
	// }
	//
	// if (IsValid(RPMAvatarLinkWidget)) {
	// 	if (!OnSubmitButtonClickedClosedDelegateHandle.IsValid())
	// 		OnSubmitButtonClickedClosedDelegateHandle = RPMAvatarLinkWidget->OnSubmitDelegate.AddWeakLambda(this, [this]() {
	// 			NavigateToUserDetail();
	// 		});
	// }

	// if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 	if (const TSharedPtr<FApiUserSubsystem> UserSubsystem = ApiModule->GetUserSubsystem()) {
	// 		{
	// 			UserSubsystem->GetOnFollowRequestCompleted().AddUObject(this, &UUIUserDetailWidget::OnFollowRequestCompleted);
	// 			UserSubsystem->GetOnUnfollowRequestCompleted().AddUObject(this, &UUIUserDetailWidget::UnfollowRequestCompleted);
	// 			UserSubsystem->GetOnIsFollowingRequestCompleted().AddUObject(this, &UUIUserDetailWidget::IsFollowingRequestCompleted);
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(User)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUIUserDetailWidget::UnregisterCallbacks() {
	if (OnCloseButtonWidgetDelegateHandle.IsValid()) {
		if (IsValid(CloseButtonWidget)) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonWidgetDelegateHandle);
			OnCloseButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnFollowButtonWidgetDelegateHandle.IsValid()) {
		if (FollowButtonWidget) {
			FollowButtonWidget->GetOnButtonClicked().Remove(OnFollowButtonWidgetDelegateHandle);
			OnFollowButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnUnfollowButtonWidgetDelegateHandle.IsValid()) {
		if (UnfollowButtonWidget) {
			UnfollowButtonWidget->GetOnButtonClicked().Remove(OnUnfollowButtonWidgetDelegateHandle);
			OnUnfollowButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnPreviewButtonClickedDelegateHandle.IsValid()) {
		if (PreviewImageButtonWidget) {
			PreviewImageButtonWidget->GetOnButtonClicked().Remove(OnPreviewButtonClickedDelegateHandle);
			OnPreviewButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnZoomImageWidgetClosedDelegateHandle.IsValid()) {
		if (ZoomImageWidget) {
			ZoomImageWidget->OnClosedDelegate.Remove(OnZoomImageWidgetClosedDelegateHandle);
			OnZoomImageWidgetClosedDelegateHandle.Reset();
		}
	}

	if (OnEditButtonClickedClosedDelegateHandle.IsValid()) {
		if (EditButtonWidget) {
			EditButtonWidget->GetOnButtonClicked().Remove(OnEditButtonClickedClosedDelegateHandle);
			OnEditButtonClickedClosedDelegateHandle.Reset();
		}
	}

	// if (OnAddFileLinkWidgetClosedDelegateHandle.IsValid()) {
	// 	if (RPMAvatarLinkWidget) {
	// 		RPMAvatarLinkWidget->OnClosedDelegate.Remove(OnAddFileLinkWidgetClosedDelegateHandle);
	// 		OnAddFileLinkWidgetClosedDelegateHandle.Reset();
	// 	}
	// }
	//
	// if (OnSubmitButtonClickedClosedDelegateHandle.IsValid()) {
	// 	if (RPMAvatarLinkWidget) {
	// 		RPMAvatarLinkWidget->OnSubmitDelegate.Remove(OnSubmitButtonClickedClosedDelegateHandle);
	// 		OnSubmitButtonClickedClosedDelegateHandle.Reset();
	// 	}
	// }
}

// void UUIUserDetailWidget::SetIsLoading(bool bInIsLoading) const {
// 	if (LoadingWidget) {
// 		if (bInIsLoading) {
// 			LoadingWidget->Show();
// 		} else {
// 			LoadingWidget->Hide();
// 		}
// 	}
// }

void UUIUserDetailWidget::FollowButtonClicked() {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiUserSubsystem> UserSubsystem = ApiModule->GetUserSubsystem()) {
			{
				if (Metadata && Metadata->Id.IsValid()) {
					const TSharedRef<FOnGenericRequestCompleted> CallbackPtr = MakeShareable(new FOnGenericRequestCompleted());

					CallbackPtr->BindWeakLambda(this, [this](const bool bInSuccessful, const FString& InError) {
						if (!bInSuccessful) {
							LogErrorF("failed to request: %s", *InError);
						}

						// Finish processing.
						// SetIsProcessing(false);

						OnFollowRequestCompleted(bInSuccessful, InError);
					});

					if (!UserSubsystem->Follow(Metadata->Id, CallbackPtr)) {
						LogErrorF("failed to request");
					} else {
						// Start processing.
						// SetIsProcessing(true);
					}
				}
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(User)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUIUserDetailWidget::UnfollowButtonClicked() {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiUserSubsystem> UserSubsystem = ApiModule->GetUserSubsystem()) {
			{
				if (Metadata && Metadata->Id.IsValid()) {
					const TSharedRef<FOnGenericRequestCompleted> CallbackPtr = MakeShareable(new FOnGenericRequestCompleted());

					CallbackPtr->BindWeakLambda(this, [this](const bool bInSuccessful, const FString& InError) {
						if (!bInSuccessful) {
							LogErrorF("failed to request: %s", *InError);
						}

						// Finish processing.
						// SetIsProcessing(false);

						OnFollowRequestCompleted(bInSuccessful, InError);
					});

					if (!UserSubsystem->Unfollow(Metadata->Id, CallbackPtr)) {
						LogErrorF("failed to request");
					} else {
						// Start processing.
						// SetIsProcessing(true);
					}
				}
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(User)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUIUserDetailWidget::RequestIsFollowing() {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiUserSubsystem> UserSubsystem = ApiModule->GetUserSubsystem()) {
			if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
				const FApiUserMetadata AuthUserMetadata = AuthSubsystem->GetUserMetadata();
				if (Metadata && Metadata->Id.IsValid() && AuthUserMetadata.Id.IsValid()) {
					const TSharedRef<FOnOkRequestCompleted> CallbackPtr = MakeShareable(new FOnOkRequestCompleted());

					CallbackPtr->BindWeakLambda(this, [this](const bool bInOk, const bool bInSuccessful, const FString& InError) {
						if (!bInSuccessful) {
							LogErrorF("failed to request: %s", *InError);
						}

						// Finish processing.
						// SetIsProcessing(false);

						OnIsFollowingRequestCompleted(bInOk, bInSuccessful, InError);
					});

					if (!UserSubsystem->IsFollowing(AuthUserMetadata.Id, Metadata->Id, CallbackPtr)) {
						LogErrorF("failed to request");
					} else {
						// Start processing.
						// SetIsProcessing(true);
					}
				}
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(User)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUIUserDetailWidget::OnIsFollowingRequestCompleted(const bool bInOk, const bool bInSuccessful, const FString& InError) {
	bIsFollowing = bInOk;
	UpdateButtons();
}

void UUIUserDetailWidget::OnFollowRequestCompleted(const bool bInSuccessful, const FString& InError) {
	FUINotificationData NotificationData;
	NotificationData.Lifetime = 5.f;

	if (bInSuccessful) {
		NotificationData.Header = LOCTEXT("FollowRequestSuccessNotificationHeader", "Success");
		NotificationData.Type = EUINotificationType::Success;
		NotificationData.Message = LOCTEXT("FollowRequestSuccessNotificationMessage", "Following.");
	} else {
		NotificationData.Header = LOCTEXT("FollowRequestErrorNotificationHeader", "Error");
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = FText::Format(LOCTEXT("FollowRequestErrorNotificationMessage", "Failed to follow: {0}"), FText::FromString(InError));
	}

	AddNotification(NotificationData);

	RequestIsFollowing();
}

void UUIUserDetailWidget::UnfollowRequestCompleted(bool bInSuccessful, const FString& InError) {
	FUINotificationData NotificationData;
	NotificationData.Lifetime = 5.f;

	if (bInSuccessful) {
		NotificationData.Header = LOCTEXT("UnfollowRequestSuccessNotificationHeader", "Success");
		NotificationData.Type = EUINotificationType::Success;
		NotificationData.Message = LOCTEXT("UnfollowRequestSuccessNotificationMessage", "Unfollowing.");
	} else {
		NotificationData.Header = LOCTEXT("UnfollowRequestErrorNotificationHeader", "Error");
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = FText::Format(LOCTEXT("UnfollowRequestErrorNotificationMessage", "Failed to Unfollow: {0}"), FText::FromString(InError));
	}

	AddNotification(NotificationData);

	RequestIsFollowing();
}

void UUIUserDetailWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		return;
	}

	// SetTexture(InResult.Texture);
	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
	
	// // todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
	// RUN_DELAYED(0.1f, this, [this](){
	// 			SetIsLoading(false);
	// 			});
}

void UUIUserDetailWidget::SetTexture(UTexture2D* Texture) {
	PreviewImageWidget->SetBrushFromTexture(Texture);

	const FVector2D Size = {
		static_cast<float>(Texture->GetSizeX()),
		static_cast<float>(Texture->GetSizeY())
	};

	PreviewImageWidget->SetDesiredSizeOverride(Size);
}

void UUIUserDetailWidget::SetMetadata(TSharedPtr<FApiUserMetadata> InUser) {
	Metadata = InUser;
	PreviewImageWidget->SetBrushFromTexture(DefaultTexture);

	if (NameTextWidget) {
		if (Metadata->Name.IsEmpty()) {
			NameTextWidget->SetText(NSLOCTEXT("VeUI", "UserNameUnknown", "Unnamed"));
		} else {
			NameTextWidget->SetText(FText::FromString(Metadata->Name));
		}
	}

	if (DescriptionTextWidget) {
		if (Metadata->Description.IsEmpty()) {
			DescriptionTextWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata->Description));
		}
	}

	if (ViewCountTextWidget) {
		ViewCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "UserViewCountFormat", "{0}"), FText::AsNumber(Metadata->Views)));
	}

	const auto PreviewImage = InUser->Files.FindByPredicate([=](const FApiFileMetadata& File) {
		return File.Type == EApiFileType::ImageAvatar;
	});

	FString DefaultAvatarUrl = InUser->GetDefaultAvatarUrl();

	// Clear cached image
	if (DefaultTexture) {
		SetTexture(DefaultTexture);
	}
	
	if (PreviewImage) {
		if (!PreviewImage->Url.IsEmpty()) {
			RequestTexture(this, PreviewImage->Url, TEXTUREGROUP_UI);
		}
	} else if (!DefaultAvatarUrl.IsEmpty()) {
		RequestTexture(this, InUser->GetDefaultAvatarUrl(), TEXTUREGROUP_UI);
	}

	if (Metadata) {
		RequestIsFollowing();
	}

	if (LikeCountTextWidget) {
		LikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "UserLikeCountFormat", "{0}"), FText::AsNumber(InUser->TotalLikes)));
	}
	
	if (DislikeCountTextWidget) {
		DislikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "UserLikeCountFormat", "{0}"), FText::AsNumber(InUser->TotalDislikes)));
	}
	
	if (RatingTextWidget) {
		const float Rating = InUser->GetRating();
		if (Rating < 0.f) {
			RatingTextWidget->SetText(NSLOCTEXT("VeUI", "UserRatingNone", "-"));
		} else {
			RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "UserRatingFormat", "{0}"), Rating));
		}
	}
}

void UUIUserDetailWidget::UpdateButtons() {
	if (FollowButtonWidget) {
		if (bIsFollowing) {
			FollowButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			FollowButtonWidget->SetIsEnabled(false);
		} else {
			FollowButtonWidget->SetVisibility(ESlateVisibility::Visible);
			FollowButtonWidget->SetIsEnabled(true);
		}
	}

	if (UnfollowButtonWidget) {
		if (!bIsFollowing) {
			UnfollowButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			UnfollowButtonWidget->SetIsEnabled(false);
		} else {
			UnfollowButtonWidget->SetVisibility(ESlateVisibility::Visible);
			UnfollowButtonWidget->SetIsEnabled(true);
		}
	}
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

void UUIUserDetailWidget::ShowZoomImageWidget(bool bAnimate, bool bPlaySound) {
	if (!bZoomImageWidgetVisible) {
		bZoomImageWidgetVisible = true;
		if (ZoomImageWidget) {
			// ZoomImageWidget->ResetState();
			// ZoomImageWidget->ResetAnimationState();
			ZoomImageWidget->Show();
		}
	}
}

// void UUIUserDetailWidget::ShowFileLinkWidget(bool bAnimate, bool bPlaySound) {
// 	if (!bFileLinkWidgetVisible) {
// 		bFileLinkWidgetVisible = true;
// 		if (RPMAvatarLinkWidget) {
// 			RPMAvatarLinkWidget->ResetState();
// 			RPMAvatarLinkWidget->ResetAnimationState();
// 			RPMAvatarLinkWidget->Show(bAnimate, bPlaySound);
// 		}
// 	}
// }

void UUIUserDetailWidget::HideZoomImageWidget(bool bAnimate, bool bPlaySound) {
	if (bZoomImageWidgetVisible) {
		bZoomImageWidgetVisible = false;
		if (ZoomImageWidget) {
			ZoomImageWidget->Hide();
		}
	}
}

// void UUIUserDetailWidget::HideFileLinkWidget(bool bAnimate, bool bPlaySound) {
// 	if (bFileLinkWidgetVisible) {
// 		bFileLinkWidgetVisible = false;
// 		if (RPMAvatarLinkWidget) {
// 			RPMAvatarLinkWidget->Hide(bAnimate, bPlaySound);
// 		}
// 	}
// }

// void UUIUserDetailWidget::NavigateToAddFileLink(bool bAnimate, bool bPlaySound) {
// 	HideZoomImageWidget();
// 	ShowFileLinkWidget();
// }

void UUIUserDetailWidget::NavigateToZoomImage(bool bAnimate, bool bPlaySound) {
	ShowZoomImageWidget();
	//HideFileLinkWidget();
}

void UUIUserDetailWidget::NavigateToUserDetail(bool bAnimate, bool bPlaySound) {
	//HideFileLinkWidget();
	HideZoomImageWidget();
}

