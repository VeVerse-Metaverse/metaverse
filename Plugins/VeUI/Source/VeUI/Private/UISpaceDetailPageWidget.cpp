// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISpaceDetailPageWidget.h"

#include "VeApi.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "ApiSpaceSubsystem.h"
#include "UIPlayerController.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIImageButtonWidget.h"
#include "Components/TextBlock.h"
#include "UIPageManagerWidget.h"
#include "UIPreviewImagePageWidget.h"
#include "UISpaceFormPageWidget.h"
#include "VeGameFramework.h"
#include "GameFrameworkWorldItem.h"
#include "GameFrameworkWorldSubsystem.h"
#include "UIDialogData.h"
#include "UIDialogWidget.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2RatingSubsystem.h"
#include "Api2WorldSubsystem.h"
#include "ApiRatingMetadata.h"
#include "UISharedAccessWidget.h"
#include "Components/UIButtonImageWidget.h"
#include "Subsystems/UIDialogSubsystem.h"
#include "UISharedAccessListPageWidget.h"


#define LOCTEXT_NAMESPACE "VeUI"


void UUISpaceDetailPageWidget::NativeOnInitialized() {
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

	if (EditButtonWidget) {
		EditButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(FormPageClass)) {
				if (auto Page = GetPageManager()->OpenPage<UUISpaceFormPageWidget>(FormPageClass)) {
					Page->SetParentPage(this);
					Page->SetMetadata(Metadata);
				}
			}
		});
	}

	if (TeleportOfflineButtonWidget) {
		TeleportOfflineButtonWidget->SetIsEnabled(false);
		TeleportOfflineButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceDetailPageWidget::OpenWorldOfflineButton_OnClicked);
	}

	if (TeleportOnlineButtonWidget) {
		TeleportOnlineButtonWidget->SetIsEnabled(false);
		TeleportOnlineButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceDetailPageWidget::OpenWorldOnlineButton_OnClicked);
	}

	if (CreateCopyButtonWidget) {
		CreateCopyButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceDetailPageWidget::CreateCopyButton_OnClicked);
	}

	if (DeleteButtonWidget) {
		DeleteButtonWidget->Hide();
		DeleteButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceDetailPageWidget::DeleteButton_OnClicked);
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
	
}

void UUISpaceDetailPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	const FGuid OptionsId = FGuid(GetOptions());
	if (!OptionsId.IsValid()) {
		LogWarningF("Filed options value");
		return;
	}

	UpdateButtons();
	MetadataRequest(OptionsId);
}

void UUISpaceDetailPageWidget::MetadataRequest(const FGuid& InId) {
	GET_MODULE_SUBSYSTEM(SpaceSubsystem, Api2, World);
	if (SpaceSubsystem) {
		// 1. Create a callback.
		const auto CallbackPtr = MakeShared<FOnWorldRequestCompleted2>();

		// 2. Bind callback.
		CallbackPtr->BindWeakLambda(this, [=](const FApiSpaceMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
			if (!bSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// SetIsProcessing(false);
			OnMetadataRequestCompleted(InMetadata, bSuccessful, InError);
		});
		FApi2GetRequestMetadata InRequestMetadata;
		
		// 3. Make the request using the callback.
		SpaceSubsystem->GetV2(InId, InRequestMetadata, CallbackPtr);
	}
}

void UUISpaceDetailPageWidget::OnMetadataRequestCompleted(const FApiSpaceMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		SetMetadata(FVeWorldMetadata(InMetadata));
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadSpaceError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a world data.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUISpaceDetailPageWidget::SetMetadata(const FVeWorldMetadata& InMetadata) {
	Metadata = InMetadata;

	FText Name = (InMetadata.Name.IsEmpty()) ? NSLOCTEXT("VeUI", "SpaceNameUnknown", "Unnamed") : FText::FromString(InMetadata.Name);
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
			DescriptionTextWidget->SetText(NSLOCTEXT("VeUI", "MSpaceDescriptionUnknown", "No description"));
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata.Description));
		}
	}

	if (OwnerTextWidget) {
		OwnerTextWidget->SetText(FText::FromString(Metadata.Owner.Name));
	}

	if (ViewCountTextWidget) {
		ViewCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "SpaceViewCountFormat", "{0}"), FText::AsNumber(Metadata.Views)));
	}

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		UserIsOwner = AuthSubsystem->GetUserId() == InMetadata.Owner.Id;
	} else {
		UserIsOwner = false;
	}

	GET_MODULE_SUBSYSTEM(WorldSubsystem, GameFramework, World);
	if (WorldSubsystem) {
		const auto RefreshCallback = MakeShared<FOnGameFrameworkWorldItemCreated>();
		RefreshCallback->BindWeakLambda(this, [=](TSharedRef<FGameFrameworkWorldItem> InWorldItem, const bool bIsValid, const FString& InError) {
			SetWorldItem(InWorldItem);
		});
		WorldSubsystem->GetItem(Metadata.Id, RefreshCallback);
	}

	UpdateButtons();


	if (LikeCountTextWidget) {
		LikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "WorldLikeCountFormat", "{0}"), FText::AsNumber(Metadata.Likes)));
	}

	if (DislikeCountTextWidget) {
		DislikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "WorldLikeCountFormat", "{0}"), FText::AsNumber(abs(Metadata.Dislikes))));
	}

	if (RatingTextWidget) {
		if (Metadata.TotalLikes + Metadata.Dislikes > 0) {
			const float Value = (static_cast<float>(Metadata.Likes) / (Metadata.Likes + abs(Metadata.Dislikes)) * 5 - 1) / 4 * 5;
			RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "WorldRatingFormat", "{0}"), FText::AsNumber(Value)));
		} else {
			RatingTextWidget->SetText(NSLOCTEXT("VeUI", "WorldRatingNone", "-"));
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

void UUISpaceDetailPageWidget::UpdateButtons() {
	const bool Edit = UserIsOwner;

	EditButtonWidget->SetVisibility(Edit ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	EditButtonWidget->SetIsEnabled(Edit);
}

void UUISpaceDetailPageWidget::DeleteRequest() {

	GET_MODULE_SUBSYSTEM(SpaceSubsystem, Api2, World);
	if (!SpaceSubsystem) {
		return;
	}

	const auto CallbackPtr = MakeShared<FOnWorldRequestCompleted2>();
	CallbackPtr->BindLambda([=](const FApiSpaceMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			if (InResponseCode != EApi2ResponseCode::Ok) {
				VeLogWarningFunc("Failed to delete world: %s", *InError);

				FUINotificationData NotificationData;
				NotificationData.Type = EUINotificationType::Failure;
				NotificationData.Header = LOCTEXT("DeleteNotificationHeader", "Error");
				NotificationData.Message = FText::Format(LOCTEXT("WorldDeleteResponseFailed", "Failed to delete world: {0}"), FText::FromString(InError));
				NotificationSubsystem->AddNotification(NotificationData);
			}

			if (InResponseCode == EApi2ResponseCode::Ok) {
				FUINotificationData NotificationData;
				NotificationData.Type = EUINotificationType::Success;
				NotificationData.Header = LOCTEXT("DeleteNotificationHeader", "Success");
				NotificationData.Message = LOCTEXT("DeleteSpaceNotificationMessage", "The world has been successfully deleted");
				NotificationSubsystem->AddNotification(NotificationData);

				GetPageManager()->ShowPrevPage(true);
			}
		}
	});

	SpaceSubsystem->Delete(Metadata.Id, CallbackPtr);
}

void UUISpaceDetailPageWidget::DeleteButton_OnClicked() {
	GET_MODULE_SUBSYSTEM(DialogSubsystem, UI, Dialog);
	if (DialogSubsystem) {
		FUIDialogData DialogData;
		DialogData.Type = EUIDialogType::OkCancel;
		DialogData.HeaderText = LOCTEXT("ConfirmWorldDeleteDialogHeader", "Confirm delete");
		DialogData.MessageText = LOCTEXT("ConfirmWorldDeleteDialogMessage", "Do you really want to delete this world?");
		DialogData.FirstButtonText = LOCTEXT("ConfirmWorldDeleteDialogButtonOk", "OK");
		DialogData.SecondButtonText = LOCTEXT("ConfirmWorldDeleteDialogButtonCancel", "Cancel");
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

void UUISpaceDetailPageWidget::SetWorldItem(TSharedPtr<FGameFrameworkWorldItem> InWorldItem) {
	if (WorldItem == InWorldItem) {
		return;
	}

	if (WorldItem.IsValid()) {
		auto WorldItemRef = WorldItem.Pin();
		WorldItemRef->GetOnStateChanged().RemoveAll(this);
	}

	WorldItem = InWorldItem;

	if (InWorldItem.IsValid()) {
		InWorldItem->GetOnStateChanged().AddUObject(this, &UUISpaceDetailPageWidget::WorldItem_OnStateChanged);
		WorldItem_OnStateChanged(*InWorldItem);
	}
}

void UUISpaceDetailPageWidget::WorldItem_OnStateChanged(const FGameFrameworkWorldItem& InWorldItem) {
	if (!WorldItem.IsValid()) {
		return;
	}
	auto WorldItemPtr = WorldItem.Pin();

	const auto WorldState = WorldItemPtr->GetState();
	const float StateRatio = WorldItemPtr->GetStateRatio();

	FText OnlineButtonText;
	FText OfflineButtonText;
	bool ButtonEnable = false;

	switch (WorldState) {
	case EVeWorldState::Error_Unavailable: {
		// VeLog(">>> Error_DownloadFail");
		OfflineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Unavailable"); // Enter World (Online) [Unavailable]
		OnlineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Unavailable");  // Enter World (Online) [Unavailable]
		ButtonEnable = false;
		break;
	}

	case EVeWorldState::Error_DownloadFail: {
		// VeLog(">>> Error_DownloadFail");
		OfflineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (offline)"); // Enter World (Online) [Download Fail]
		OnlineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (Online)");   // Enter World (Online) [Download Fail]
		ButtonEnable = true;
		break;
	}

	case EVeWorldState::Error_MountFail: {
		// VeLog(">>> Error_MountFail");
		OfflineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Fail to mount"); // Fail to mount [Fail to mount]
		OnlineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Fail to mount");  // Fail to mount [Fail to mount]
		ButtonEnable = false;
		break;
	}

	case EVeWorldState::PendingDownloadPackage: {
		// VeLog(">>> PendingDownloadPackage");
		OfflineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (offline)"); // Enter World (Online) [PendingDownload]
		OnlineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (Online)");   // Enter World (Online) [PendingDownload]
		ButtonEnable = true;
		break;
	}

	case EVeWorldState::PackageDownloading: {
		// VeLog(">>> PackageDownloading");
		OfflineButtonText = FText::Format(LOCTEXT("TeleportOfflineButtonWidget_Downloading", "Downloading {0}"), FText::AsPercent(StateRatio));
		OnlineButtonText = FText::Format(LOCTEXT("TeleportOfflineButtonWidget_Downloading", "Downloading {0}"), FText::AsPercent(StateRatio));
		ButtonEnable = false;
		break;
	}

	case EVeWorldState::PackageDownloaded: {
		// VeLog(">>> PackageDownloaded");
		OfflineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (Offline) "); // Enter World (Online) [PackageDownloaded]
		OnlineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (Online) ");   // Enter World (Online) [PackageDownloaded]
		ButtonEnable = true;
		break;
	}

	case EVeWorldState::PackageMounted: {
		// VeLog(">>> PackageMounted");
		OfflineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (Offline)"); // Enter World (Online) [PackageMounted]
		OnlineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (Online)");   // Enter World (Online) [PackageMounted]
		ButtonEnable = true;
		break;
	}

	case EVeWorldState::ServerMatching: {
		// VeLog(">>> ServerMatching");
		OfflineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (Offline)"); // Enter World (Online)
		OnlineButtonText = FText::Format(LOCTEXT("TeleportOfflineButtonWidget_Downloading", "Matching Server {0}"), FText::AsPercent(StateRatio));
		ButtonEnable = false;
		break;
	}

	case EVeWorldState::ServerOnline: {
		// VeLog(">>> ServerOnline");
		OfflineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (Offline)"); // Enter World (Online)
		OnlineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Enter World (Online)");   // Enter World (Online) [ServerOnline]
		ButtonEnable = true;
		break;
	}

	case EVeWorldState::Teleporting: {
		// VeLog(">>> Teleporting");
		OfflineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Teleporting..."); // Enter World (Online) [Teleporting...]
		OnlineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Teleporting...");  // Enter World (Online) [Teleporting...]
		ButtonEnable = false;
		break;
	}

	default: {
		// VeLog(">>> Default %d", WorldState);
		OfflineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Error"); // Enter World (Online) [Error]
		OnlineButtonText = LOCTEXT("TeleportOnlineButtonWidget_Default", "Error");  // Enter World (Online) [Error]
		ButtonEnable = false;
	}
	}


	if (TeleportOfflineButtonWidget) {
		TeleportOfflineButtonWidget->SetButtonText(OfflineButtonText);
		TeleportOfflineButtonWidget->SetIsEnabled(ButtonEnable);
	}

	if (TeleportOnlineButtonWidget) {
		TeleportOnlineButtonWidget->SetButtonText(OnlineButtonText);
		TeleportOnlineButtonWidget->SetIsEnabled(ButtonEnable);
	}

	if (DeleteButtonWidget) {
		if (UserIsOwner) {
			DeleteButtonWidget->Show();
		}
	}
}

void UUISpaceDetailPageWidget::OpenWorldOfflineButton_OnClicked() {
	if (!WorldItem.IsValid()) {
		return;
	}
	auto WorldItemPtr = WorldItem.Pin();


	// If not mounted
	const auto WorldState = WorldItemPtr->GetState();
	if (WorldState == EVeWorldState::PackageDownloaded) {
		const auto Callback = MakeShared<FOnPackageState>();
		Callback->BindWeakLambda(this, [=](EVePackageState InState) {
			if (InState != EVePackageState::Mounted) {
				VeLogErrorFunc("Package '%s' (%s) not mounted", *WorldItemPtr->GetMetadata().Name, *WorldItemPtr->GetId().ToString(EGuidFormats::DigitsWithHyphensLower));
				TeleportOfflineButtonWidget->SetIsEnabled(false);
				TeleportOnlineButtonWidget->SetIsEnabled(false);
				return;
			}
			WorldItemPtr->StartTeleportOffline(MakeShared<FOnOpenWorldState>(), GetOwningPlayer());
		});
		WorldItemPtr->MountPackage(Callback);
		return;
	}

	WorldItemPtr->StartTeleportOffline(MakeShared<FOnOpenWorldState>(), GetOwningPlayer());
}

void UUISpaceDetailPageWidget::OpenWorldOnlineButton_OnClicked() {
	if (!WorldItem.IsValid()) {
		return;
	}
	auto WorldItemPtr = WorldItem.Pin();

	// If not mounted
	const auto WorldState = WorldItemPtr->GetState();
	if (WorldState == EVeWorldState::PackageDownloaded) {
		const auto Callback = MakeShared<FOnPackageState>();
		Callback->BindWeakLambda(this, [=](EVePackageState InState) {
			if (InState != EVePackageState::Mounted) {
				VeLogErrorFunc("Package '%s' (%s) not mounted", *WorldItemPtr->GetMetadata().Name, *WorldItemPtr->GetId().ToString(EGuidFormats::DigitsWithHyphensLower));
				TeleportOfflineButtonWidget->SetIsEnabled(false);
				TeleportOnlineButtonWidget->SetIsEnabled(false);
				return;
			}
			WorldItemPtr->StartTeleportOnline(MakeShared<FOnOpenWorldState>(), GetOwningPlayer());
		});
		WorldItemPtr->MountPackage(Callback);
		return;
	}

	WorldItemPtr->StartTeleportOnline(MakeShared<FOnOpenWorldState>(), GetOwningPlayer());
}

void UUISpaceDetailPageWidget::CreateCopyButton_OnClicked() {
	if (IsValid(FormPageClass)) {
		if (auto Page = GetPageManager()->OpenPage<UUISpaceFormPageWidget>(FormPageClass)) {
			Page->SetParentPage(this);

			FVeWorldMetadata NewWorld;
			NewWorld.Map = Metadata.Map;
			NewWorld.Package.Id = Metadata.Package.Id;

			Page->SetMetadata(NewWorld);
		}
	}
}

#undef LOCTEXT_NAMESPACE
