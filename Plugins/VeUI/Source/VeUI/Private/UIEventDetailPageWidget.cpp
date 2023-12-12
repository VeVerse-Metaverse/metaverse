// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIEventDetailPageWidget.h"

#include "VeApi.h"
#include "VeUI.h"
#include "VeDownload.h"
#include "VePak/VePak.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "UIPageManagerWidget.h"
#include "UIPreviewImagePageWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIImageButtonWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIEventDetailPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	LastProgressTime = FDateTime::MinValue();

	if (PreviewImageWidget) {
		PreviewImageWidget->OnClicked.AddWeakLambda(this, [this]() {
			if (auto Page = GetPageManager()->OpenPage<UUIPreviewImagePageWidget>(PreviewPageClass, PreviewImageWidget->GetURL())) {
				Page->SetParentPage(this);
			}
		});
	}

	if (SummaryTextWidget) {
		SummaryTextWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (DescriptionTextWidget) {
		DescriptionTextWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IsValid(VisitButtonWidget)) {
		VisitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIEventDetailPageWidget::OnVisitButtonClicked);
	}
}

void UUIEventDetailPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	const FGuid OptionsId = FGuid(GetOptions());
	if (!OptionsId.IsValid()) {
		LogWarningF("Filed options value");
		return;
	}

	MetadataRequest(OptionsId);
}

void UUIEventDetailPageWidget::MetadataRequest(const FGuid& InId) {
	GET_MODULE_SUBSYSTEM(EventSubsystem, Api, Event);
	if (EventSubsystem) {
		// 1. Create a callback.
		TSharedPtr<FOnEventRequestCompleted> CallbackPtr = MakeShareable(new FOnEventRequestCompleted());

		// 2. Bind callback.
		CallbackPtr->BindWeakLambda(this, [=](const FApiEventMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// SetIsProcessing(false);
			OnMetadataRequestCompleted(InMetadata, bInSuccessful, InError);

			CallbackPtr.Reset();
		});

		// 3. Make the request using the callback.
		if (EventSubsystem->GetEvent(InId, CallbackPtr)) {
			// SetIsProcessing(true);
		} else {
			LogErrorF("failed to request");
		}
	}
}

void UUIEventDetailPageWidget::OnMetadataRequestCompleted(const FApiEventMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		SetMetadata(InMetadata);
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadEventError", "Error");
			NotificationData.Message = FText::FromString("Failed to load an event data.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIEventDetailPageWidget::SetMetadata(const FApiEventMetadata& InMetadata) {
	Metadata = InMetadata;

	FText EventTitle = (InMetadata.Title.IsEmpty()) ? NSLOCTEXT("VeUI", "EventNameUnknown", "Unnamed") : FText::FromString(InMetadata.Title);
	SetTitle(EventTitle);
	if (TitleTextWidget) {
		TitleTextWidget->SetText(EventTitle);
	}

	// PreviewImage
	if (PreviewImageWidget) {
		PreviewImageWidget->ShowImage(Metadata.Files, EApiFileType::ImagePreview);
	}

	if (SummaryTextWidget) {
		if (Metadata.Summary.IsEmpty()) {
			SummaryTextWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			SummaryTextWidget->SetText(FText::FromString(Metadata.Summary));
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
		if (Metadata.Owner.Id.IsValid() && !Metadata.Owner.Name.IsEmpty()) {
			OwnerTextWidget->SetText(FText::FromString(Metadata.Owner.Name));
		} else {
			OwnerTextWidget->SetText(NSLOCTEXT("VeUI", "Events_OwnerUnknown", "Unknown"));
		}
	}

	if (StartsDateTextWidget) {
		if (Metadata.StartsAt == 0) {
			StartsDateTextWidget->SetText(NSLOCTEXT("VeUI", "Events_StartsDateUnknown", "No starts date"));
		} else {
			// StartsDateTextWidget->SetText(FText::FromString(Metadata.StartsAt.ToString()));
			StartsDateTextWidget->SetText(FText::AsDateTime(Metadata.StartsAt));
		}
	}

	if (EndsDateTextWidget) {
		if (Metadata.EndsAt == 0) {
			EndsDateTextWidget->SetText(NSLOCTEXT("VeUI", "Events_EndsDateUnknown", "No ends date"));
		} else {
			// EndsDateTextWidget->SetText(FText::FromString(Metadata.StartsAt.ToString()));
			EndsDateTextWidget->SetText(FText::AsDateTime(Metadata.EndsAt));
		}
	}

	if (ViewCountTextWidget) {
		ViewCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldViewCountFormat", "{0}"), FText::AsNumber(Metadata.Views)));
	}

	// if (LikeCountTextWidget) {
		// LikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldLikeCountFormat", "{0}"), FText::AsNumber(Metadata.Likes)));
	// }

	// if (DislikeCountTextWidget) {
		// DislikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldLikeCountFormat", "{0}"), FText::AsNumber(Metadata.Dislikes)));
	// }

	// if (RatingTextWidget) {
	// 	if (Metadata.Likes + Metadata.Dislikes > 0) {
	// 		const float Value = (static_cast<float>(Metadata.Likes) / (Metadata.Likes + Metadata.Dislikes) * 5 - 1) / 4 * 5;
	// 		RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldRatingFormat", "{0}"), FText::AsNumber(Value)));
	// 	} else {
	// 		RatingTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldRatingNone", "Not rated"));
	// 	}
	// }
}

void UUIEventDetailPageWidget::OnVisitButtonClicked() {
	// Check if pak is mounted
// 	if (FPakPakSubsystem::IsEventPakMounted(Metadata, false)) {
// 		// Pak file is mounted, request the matching server.
// 		UGameplayStatics::OpenLevel(GetOwningPlayer(), FName(Metadata.Map), true, TEXT("")); // ?listen
// 	} else if (FPakPakSubsystem::DoesEventPakFileExist(Metadata, false)) {
// #if UE_EDITOR
// 		LogWarningF("can not mount virtual world in editor");
// 		UGameplayStatics::OpenLevel(GetOwningPlayer(), FName(Metadata.Map), true, TEXT("")); // ?listen
//
// #else
// 		// Pak file is not mounted, but exists on the disk, mount it.
// 		const FGuid PakId = FPakPakSubsystem::GetEventPakFileId(Metadata, false);
// 		const FString PakPath = FPakPakSubsystem::GetEventPakPath(PakId);
// 		if (FPakPakSubsystem::MountEventPakFile(PakId)) {
// 			FPakPakSubsystem::RegisterEventMountPoint(Metadata.Name);
// 			FPakPakSubsystem::LoadPakAssetRegistry(Metadata.Name);
// 			UGameplayStatics::OpenLevel(GetOwningPlayer(), FName(Metadata.Map), true, TEXT("")); // ?listen
// 		} else {
// 			LogErrorF("failed to mount virtual world file: %s", *PakPath);
// 		}
// #endif
// 	}

	// const FApiFileMetadata* PakMetadata = GetModPakFileMetadata();
	// // const FGuid ModPakId = ModPakMetadata.Id;
	// // const FString ModFilePath = GetModPakPath(ModPakId);
	//
	// if (FVePakModule* PakModule = FVePakModule::Get()) {
	// 	if (const TSharedPtr<FPakPakSubsystem> PakSubsystem = PakModule->GetPakSubsystem()) {
	// 		{
	// 			if (!PakSubsystem->MountModPakFile(PakMetadata->EntityId)) {
	// 				LogErrorF("failed to mount virtual world pak file %s", *PakMetadata->EntityId.ToString());
	// 				return;
	// 			}
	// 			PakSubsystem->RegisterModMountPoint(Metadata.Name);
	// 			// PakSubsystem->LogFilesInDirectoryRecursively(TEXT("LE7EL"));
	// 			PakSubsystem->LoadPakAssetRegistry(Metadata.Name);
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Pak), STRINGIFY(Pak)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Pak)); };

	//UGameplayStatics::OpenLevel(GetOwningPlayer(), FName(Metadata.Map), true, TEXT("")); // ?listen

	// const bool bResult = FPakLoader::Get()->MountPakFile(ModFilePath, INDEX_NONE, FString());
	// if (!bResult) {
	// 	LogErrorF("Failed to mount the mod pak file");
	// 	return;
	// }
	//
	// FPakSubsystemPtr PakSubsystemPtr = FVePakModule::GetPakSubsystem();
	// PakSubsystemPtr->MountPakFile();
	//
	// if (Metadata.Name.IsEmpty()) {
	// 	LogErrorF("Metadata name is empty");
	// }
	//
	// const FString RootPath = FString::Printf(TEXT("/%s/"), *Metadata.Name);
	// // const FString RootPath = FString::Printf(TEXT("/%s/"), TEXT("Game"));
	// const FString ContentPath = FString::Printf(TEXT("../../../Metaverse/Plugins/%s/Content"), *Metadata.Name);
	// FPakLoader::Get()->RegisterMountPoint(RootPath, ContentPath);
	//
	// auto List = FPakLoader::Get()->GetMountedPakFilenames();
	// for (auto F : List) {
	// 	LogScreenLogF("Mounted pak: %s", *F);
	// }

	// UGameplayStatics::OpenLevel(GetWorld(), FName("Example"));
}

FApiFileMetadata* UUIEventDetailPageWidget::GetEventPakFileMetadata() {
	return Metadata.Files.FindByPredicate([=](const FApiFileMetadata& File) {
		return File.Type == EApiFileType::Pak && File.Platform == FVeSharedModule::GetPlatformString() && File.DeploymentType == EApiFileDeploymentType::Client;
	});
}

bool UUIEventDetailPageWidget::GetPakFileExists() {
	const FApiFileMetadata* PakMetadata = GetEventPakFileMetadata();

	if (PakMetadata) {
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		return PlatformFile.FileExists(*GetEventPakPath());
	}

	return false;
}

bool UUIEventDetailPageWidget::GetPakMetadataExists() {
	return !!GetEventPakFileMetadata();
}

FString UUIEventDetailPageWidget::GetEventPakPath() const {
	// const FGuid ModFileId = FPakPakSubsystem::GetEventPakFileId(Metadata, false);
	// const FString ModFilePath = FPakPakSubsystem::GetEventPakPath(ModFileId);
	// return ModFilePath;
	return "";
}

void UUIEventDetailPageWidget::OnEventPakDownloadProgress(const FChunkDownloadProgress& Result) {
	auto t = FDateTime::UtcNow();
	// [FIXME] make time interval configurable
	if ((t - LastProgressTime).GetTotalSeconds() < 1) return;
	LastProgressTime = t;

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;
		NotificationData.Type = EUINotificationType::Success;
		NotificationData.Header = LOCTEXT("DownloadEventSuccessNotificationHeader", "Downloading");
		NotificationData.Message = FText::Format(
			LOCTEXT("DownloadEventSuccessNotificationMessage", "Downloaded {0} of {1} ({2})."), FText::AsMemory((int64)(Result.iTotalSize * Result.fProgress)),
			FText::AsMemory(Result.iTotalSize), FText::AsPercent(Result.fProgress));
		NotificationSubsystem->AddNotification(NotificationData);
	}
}

void UUIEventDetailPageWidget::OnEventPakDownloadComplete(const FFileDownloadResult& Result) {
	//LogScreenLogF("OnComplete %d %s %s", Result.bSuccessful, *Result.Path, *Result.Url);
	// if (BoundDownload.IsValid()) {
	// 	BoundDownload.Reset();
	// }

	// UpdateButtons();

	bEventPakExists = true;
	bIsProcessing = false;

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;

		if (Result.bSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("DownloadEventSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("DownloadEventSuccessNotificationMessage", "Successfully downloaded the virtual world.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("DownloadEventErrorNotificationHeader", "Error");
			NotificationData.Message = FText::Format(
				LOCTEXT("DownloadEventErrorNotificationMessage", "Failed to load the event data: {0}"), FText::FromString(Result.Error));
		}

		NotificationSubsystem->AddNotification(NotificationData);
	}
}


#undef LOCTEXT_NAMESPACE
