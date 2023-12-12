// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIMetaverseDetailPageWidget.h"

#include "VeApi2.h"
#include "VeUI.h"
#include "VeDownload.h"
#include "VePak/VePak.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Subsystems/UIDialogSubsystem.h"
#include "Components/Image.h"
#include "UIPageManagerWidget.h"
#include "UIPreviewImagePageWidget.h"
#include "Components/TextBlock.h"
#include "Components/UIButtonWidget.h"
#include "AnimatedTexture2D.h"
#include "Api2PackageSubsystem.h"
#include "Components/UIImageWidget.h"


#define LOCTEXT_NAMESPACE "VeUI"


void UUIMetaverseDetailPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	LastProgressTime = FDateTime::MinValue();
}

void UUIMetaverseDetailPageWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (!OnPreviewButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(PreviewImageButtonWidget)) {
			OnPreviewButtonClickedDelegateHandle = PreviewImageButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				FApiFileMetadata* PreviewImage = Metadata.Files.FindByPredicate([=](const FApiFileMetadata& File) {
					return File.Type == EApiFileType::ImageFull;
				});
				if (PreviewImage && !PreviewImage->Url.IsEmpty()) {
					if (auto Page = GetPageManager()->OpenPage<UUIPreviewImagePageWidget>(PreviewPageClass, PreviewImage->Url)) {
						Page->SetParentPage(this);
					}
				}
			});
		}
	}

	if (!OnDeleteButtonWidgetDelegateHandle.IsValid()) {
		if (IsValid(DeleteButtonWidget)) {
			OnDeleteButtonWidgetDelegateHandle = DeleteButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIMetaverseDetailPageWidget::OnDeleteButtonClicked);
		}
	}

	if (!OnInstallButtonWidgetDelegateHandle.IsValid()) {
		if (IsValid(InstallButtonWidget)) {
			OnInstallButtonWidgetDelegateHandle = InstallButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIMetaverseDetailPageWidget::OnInstallButtonClicked);
		}
	}
}

void UUIMetaverseDetailPageWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	if (OnPreviewButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(PreviewImageButtonWidget)) {
			PreviewImageButtonWidget->GetOnButtonClicked().Remove(OnPreviewButtonClickedDelegateHandle);
		}
		OnPreviewButtonClickedDelegateHandle.Reset();
	}

	if (OnInstallButtonWidgetDelegateHandle.IsValid()) {
		if (IsValid(InstallButtonWidget)) {
			InstallButtonWidget->GetOnButtonClicked().Remove(OnInstallButtonWidgetDelegateHandle);
		}
		OnInstallButtonWidgetDelegateHandle.Reset();
	}

	if (OnDeleteButtonWidgetDelegateHandle.IsValid()) {
		if (IsValid(DeleteButtonWidget)) {
			DeleteButtonWidget->GetOnButtonClicked().Remove(OnDeleteButtonWidgetDelegateHandle);
		}
		OnDeleteButtonWidgetDelegateHandle.Reset();
	}

	if (BoundDownload.IsValid()) {
		BoundDownload.Reset();
	}
}

void UUIMetaverseDetailPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	if (PreviewImageWidget) {
		const TArray<EApiFileType> Types = {
			EApiFileType::ImagePreview,
			EApiFileType::ImageFull,
		};
	
		if (const auto* FileMetadata = FindFileMetadata(Metadata.Files, Types)) {
			PreviewImageWidget->ShowImage(FileMetadata->Url);
		}
	}

	const FGuid OptionsId = FGuid(GetOptions());
	if (!OptionsId.IsValid()) {
		LogWarningF("Filed options value");
		return;
	}

	UpdateButtons();
	MetadataRequest(OptionsId);
}

void UUIMetaverseDetailPageWidget::MetadataRequest(const FGuid& InId) {
	GET_MODULE_SUBSYSTEM(PackageSubsystem, Api2, Package);
	if (PackageSubsystem) {
		// 1. Create a callback.
		const auto CallbackPtr = MakeShared<FOnPackageRequestCompleted2>();

		// 2. Bind callback.
		CallbackPtr->BindWeakLambda(this, [=](const FApiPackageMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// SetIsProcessing(false);
			OnMetadataRequestCompleted(InMetadata, bInSuccessful, InError);
		});

		// 3. Make the request using the callback.
		PackageSubsystem->Get(InId, CallbackPtr);
	}
}

void UUIMetaverseDetailPageWidget::OnMetadataRequestCompleted(const FApiPackageMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		SetMetadata(InMetadata);
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadMetaverseError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a metaverse data.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIMetaverseDetailPageWidget::SetMetadata(const FApiPackageMetadata& InMetadata) {
	Metadata = InMetadata;

	FText MetaverseTitle = (InMetadata.Title.IsEmpty()) ? NSLOCTEXT("VeUI", "MetaverseNameUnknown", "Unnamed") : FText::FromString(InMetadata.Title);
	SetTitle(MetaverseTitle);
	if (TitleTextWidget) {
		TitleTextWidget->SetText(MetaverseTitle);
	}

	// PreviewImage
	if (PreviewImageWidget) {
		const TArray<EApiFileType> Types = {
			EApiFileType::ImagePreview,
			EApiFileType::ImageFull,
		};
	
		if (const auto* FileMetadata = FindFileMetadata(Metadata.Files, Types)) {
			PreviewImageWidget->ShowImage(FileMetadata->Url);
		}
	}

	if (PriceTextWidget) {
		if (Metadata.Price > 0) {
			PriceTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldPriceFormat", "{0}"), FText::AsCurrency(Metadata.Price)));
		} else {
			PriceTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldPriceFree", "Free"));
		}
	}

	if (DescriptionTextWidget) {
		if (Metadata.Description.IsEmpty()) {
			DescriptionTextWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata.Description));
		}
	}

	if (AuthorTextWidget) {
		if (Metadata.Owner.IsValid() && !Metadata.Owner->Name.IsEmpty()) {
			AuthorTextWidget->SetText(FText::FromString(Metadata.Owner->Name));
		} else {
			AuthorTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldAuthorUnknown", "Unknown"));
		}
	}

	if (DescriptionTextWidget) {
		if (Metadata.Summary.IsEmpty()) {
			DescriptionTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldSummaryUnknown", "No description"));
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata.Summary));
		}
	}

	if (VersionTextWidget) {
		if (Metadata.Description.IsEmpty()) {
			VersionTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldVersionUnknown", "No version"));
		} else {
			VersionTextWidget->SetText(FText::FromString(Metadata.Version));
		}
	}

	if (ReleaseDateTextWidget) {
		if (Metadata.ReleasedAt == 0) {
			ReleaseDateTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldReleaseDateUnknown", "No release date"));
		} else {
			ReleaseDateTextWidget->SetText(FText::FromString(Metadata.Version));
		}
	}

	if (ViewCountTextWidget) {
		ViewCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldViewCountFormat", "{0}"), FText::AsNumber(Metadata.Views)));
	}

	if (DownloadCountTextWidget) {
		DownloadCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldDownloadCountFormat", "{0}"), FText::AsNumber(Metadata.Downloads)));
	}

	if (LikeCountTextWidget) {
		LikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldLikeCountFormat", "{0}"), FText::AsNumber(Metadata.Likes)));
	}

	if (DislikeCountTextWidget) {
		DislikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldLikeCountFormat", "{0}"), FText::AsNumber(Metadata.Dislikes)));
	}

	if (RatingTextWidget) {
		if (Metadata.Likes + Metadata.Dislikes > 0) {
			const float Value = (static_cast<float>(Metadata.Likes) / (Metadata.Likes + Metadata.Dislikes) * 5 - 1) / 4 * 5;
			RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldRatingFormat", "{0}"), FText::AsNumber(Value)));
		} else {
			RatingTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldRatingNone", "Not rated"));
		}
	}

	if (DownloadSizeTextWidget) {
		if (Metadata.DownloadSize > 0) {
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.SetMaximumFractionalDigits(2);
			DownloadSizeTextWidget->SetText(
				FText::Format(NSLOCTEXT("VeUI", "VirtualWorldDownloadSizeFormat", "{0}"), FText::AsMemory(Metadata.DownloadSize, &NumberFormattingOptions, nullptr, SI)));
		} else {
			DownloadSizeTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldDownloadSizeUnknown", "?"));
		}
	}

	UpdateButtons();
}

// void UUIMetaverseDetailPageWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
// 	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
// 		return;
// 	}
//
// 	// SetTexture(InResult.Texture);
// 	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
//
// 	// // todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
// 	// RUN_DELAYED(0.1f, this, [this](){
// 	// 			SetIsProcessing(false);
// 	// 			});
// }
//
// void UUIMetaverseDetailPageWidget::SetTexture(UTexture2D* Texture) {
// 	PreviewImageWidget->SetBrushFromTexture(Texture);
//
// 	const FVector2D Size = {
// 		static_cast<float>(Texture->GetSizeX()),
// 		static_cast<float>(Texture->GetSizeY())
// 	};
//
// 	PreviewImageWidget->SetDesiredSizeOverride(Size);
// }

void UUIMetaverseDetailPageWidget::OnInstallButtonClicked() {
	// if (const FApiFileMetadata* ModFile = GetModPakFileMetadata()) {
	if (BoundDownload.IsValid()) return;
	if (FVeDownloadModule* DownloadModule = FVeDownloadModule::Get()) {
		if (const TSharedPtr<FDownloadChunkSubsystem> ChunkSubsystem = DownloadModule->GetChunkSubsystem()) {
			const FString PakUrl = FPakPakSubsystem::GetMetaversePakFileUrl(Metadata, false);
			if (PakUrl.IsEmpty()) {
				LogErrorF("no mod file url");
				return;
			}
			const FGuid PakId = FPakPakSubsystem::GetMetaversePakFileId(Metadata, false);
			const FString PakPath = FPakPakSubsystem::GetMetaversePakPath(PakId);

			BoundDownload = ChunkSubsystem->DownloadFile({PakId, PakUrl, PakPath}, 1, false);
			if (BoundDownload.IsValid()) {
				BoundDownload->OnProgress().BindUObject(this, &UUIMetaverseDetailPageWidget::OnMetaversePakDownloadProgress);
				BoundDownload->OnComplete().BindUObject(this, &UUIMetaverseDetailPageWidget::OnMetaversePakDownloadComplete);
			}

			bMetaversePakExists = false;
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Download), STRINGIFY(Chunk)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Download)); };
	// }

	//
	// if (const FApiFileMetadata* ModFile = GetModPakFileMetadata()) {
	// 	if (!ModFile->Url.IsEmpty()) {
	// 		if (BoundDownload.IsValid()) return;
	//
	// 		if (FVeDownloadModule* DownloadModule = FVeDownloadModule::Get()) {
	// 			if (const TSharedPtr<FDownloadChunkSubsystem> ChunkSubsystem = DownloadModule->GetChunkSubsystem()) {
	// 				BoundDownload = ChunkSubsystem->DownloadFile({ Metadata.Id, ModFile->Url, GetModPakPath() }, 1, false);
	// 				if (BoundDownload.IsValid()) {
	// 					BoundDownload->OnProgress().BindUObject(this, &UUIMetaverseDetailPageWidget::OnModFileDownloadProgress);
	// 					BoundDownload->OnComplete().BindUObject(this, &UUIMetaverseDetailPageWidget::OnModFileDownloadComplete);
	// 				}
	// 					
	// 				bModPakFileExists = false;
	// 			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Download), STRINGIFY(Chunk)); }
	// 		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Download)); };
	// 	}
	// }

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;
		NotificationData.Type = EUINotificationType::Info;
		NotificationData.Header = LOCTEXT("DownloadVirtualWorldStartedNotificationHeader", "Started");
		NotificationData.Message = LOCTEXT("DownloadVirtualWorldStartedNotificationMessage", "Downloading the package...");
		NotificationSubsystem->AddNotification(NotificationData);
	}

	bIsProcessing = true;
	UpdateButtons();
}

void UUIMetaverseDetailPageWidget::OnDeleteButtonClicked() {
	GET_MODULE_SUBSYSTEM(DialogSubsystem, UI, Dialog);
	if (DialogSubsystem) {
		FUIDialogData DialogData;

		DialogData.Type = EUIDialogType::OkCancel;
		DialogData.HeaderText = LOCTEXT("ConfirmObjectDeleteDialogHeader", "Confirm delete");
		DialogData.MessageText = LOCTEXT("ConfirmObjectDeleteDialogMessage", "Do you really want to delete this virtual world?");
		DialogData.FirstButtonText = LOCTEXT("ConfirmObjectDeleteDialogButtonOk", "OK");
		DialogData.SecondButtonText = LOCTEXT("ConfirmObjectDeleteDialogButtonCancel", "Cancel");
		DialogData.bCloseOnButtonClick = true;

		FUIDialogButtonClicked OnDialogButtonClicked;
		OnDialogButtonClicked.BindWeakLambda(this, [this](UUIDialogWidget*, const uint8& InButtonIndex) {
			if (InButtonIndex == 0) {
				DeleteMetaverse();
			}
		});

		DialogSubsystem->ShowDialog(DialogData, OnDialogButtonClicked);
	}
}

void UUIMetaverseDetailPageWidget::DeleteMetaverse() {
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// if (const FApiFileMetadata* ModFile = GetMetaversePakFileMetadata()) {
	const FString PakUrl = FPakPakSubsystem::GetMetaversePakFileUrl(Metadata, false);
	if (PakUrl.IsEmpty()) {
		LogErrorF("no metaverse pak url");
		return;
	}
	const FGuid PakId = FPakPakSubsystem::GetMetaversePakFileId(Metadata, false);
	const FString PakPath = FPakPakSubsystem::GetMetaversePakPath(PakId);
	if (FPakPakSubsystem::IsPakMounted(PakPath) && !FPakPakSubsystem::UnmountPakFile(PakPath)) {
		LogErrorF("Failed to unmount pak", STRINGIFY(Download), STRINGIFY(Chunk));
	}
	if (FVeDownloadModule* DownloadModule = FVeDownloadModule::Get()) {
		if (const TSharedPtr<FDownloadChunkSubsystem> ChunkSubsystem = DownloadModule->GetChunkSubsystem()) {
			ChunkSubsystem->TryRemove({PakId, PakUrl, PakPath});
		}
	}
	// }

	UpdateButtons();
}

void UUIMetaverseDetailPageWidget::UpdateButtons() {
	if (InstallButtonWidget) {
		if (GetPakFileExists()) {
			InstallButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			InstallButtonWidget->SetIsEnabled(true);
		} else if (GetPakFileExists() && bMetaversePakExists) {
			InstallButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			InstallButtonWidget->SetIsEnabled(true);
		} else if (bIsProcessing) {
			InstallButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			InstallButtonWidget->SetIsEnabled(true);
		} else if (!GetPakMetadataExists()) {
			InstallButtonWidget->SetVisibility(ESlateVisibility::Visible);
			InstallButtonWidget->SetIsEnabled(false);
		} else {
			InstallButtonWidget->SetVisibility(ESlateVisibility::Visible);
			InstallButtonWidget->SetIsEnabled(true);
		}
	}

	if (DeleteButtonWidget) {
		if (!GetPakFileExists()) {
			DeleteButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			DeleteButtonWidget->SetIsEnabled(true);
		} else if (!GetPakFileExists() && !bMetaversePakExists) {
			DeleteButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			DeleteButtonWidget->SetIsEnabled(true);
		} else if (bIsProcessing) {
			DeleteButtonWidget->SetVisibility(ESlateVisibility::Visible);
			DeleteButtonWidget->SetIsEnabled(false);
		} else {
			DeleteButtonWidget->SetVisibility(ESlateVisibility::Visible);
			DeleteButtonWidget->SetIsEnabled(true);
		}
	}
}


FApiFileMetadata* UUIMetaverseDetailPageWidget::GetMetaversePakFileMetadata() {
	return Metadata.Files.FindByPredicate([=](const FApiFileMetadata& File) {
		return File.Type == EApiFileType::Pak && File.Platform == FVeSharedModule::GetPlatformString() && File.DeploymentType == EApiFileDeploymentType::Client;
	});
}

bool UUIMetaverseDetailPageWidget::GetPakFileExists() {
	const FApiFileMetadata* PakMetadata = GetMetaversePakFileMetadata();

	if (PakMetadata) {
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		return PlatformFile.FileExists(*GetMetaversePakPath());
	}

	return false;
}

bool UUIMetaverseDetailPageWidget::GetPakMetadataExists() {
	return !!GetMetaversePakFileMetadata();
}

FString UUIMetaverseDetailPageWidget::GetMetaversePakPath() const {
	const FGuid ModFileId = FPakPakSubsystem::GetMetaversePakFileId(Metadata, false);
	const FString ModFilePath = FPakPakSubsystem::GetMetaversePakPath(ModFileId);
	return ModFilePath;
}

void UUIMetaverseDetailPageWidget::OnMetaversePakDownloadProgress(const FChunkDownloadProgress& Result) {
	auto t = FDateTime::UtcNow();
	// [FIXME] make time interval configurable
	if ((t - LastProgressTime).GetTotalSeconds() < 1) return;
	LastProgressTime = t;

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;
		NotificationData.Type = EUINotificationType::Success;
		NotificationData.Header = LOCTEXT("DownloadMetaverseSuccessNotificationHeader", "Downloading");
		NotificationData.Message = FText::Format(
			LOCTEXT("DownloadMetaverseSuccessNotificationMessage", "Downloaded {0} of {1} ({2})."), FText::AsMemory((int64)(Result.iTotalSize * Result.fProgress)),
			FText::AsMemory(Result.iTotalSize), FText::AsPercent(Result.fProgress));
		NotificationSubsystem->AddNotification(NotificationData);
	}
}

void UUIMetaverseDetailPageWidget::OnMetaversePakDownloadComplete(const FFileDownloadResult& Result) {
	//LogScreenLogF("OnComplete %d %s %s", Result.bSuccessful, *Result.Path, *Result.Url);
	if (BoundDownload.IsValid()) {
		BoundDownload.Reset();
	}

	UpdateButtons();

	bMetaversePakExists = true;
	bIsProcessing = false;

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;

		if (Result.bSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("DownloadMetaverseSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("DownloadMetaverseSuccessNotificationMessage", "Successfully downloaded the virtual world.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("DownloadMetaverseErrorNotificationHeader", "Error");
			NotificationData.Message = FText::Format(
				LOCTEXT("DownloadMetaverseErrorNotificationMessage", "Failed to download the package: {0}"), FText::FromString(Result.Error));
		}

		NotificationSubsystem->AddNotification(NotificationData);
	}
}


#undef LOCTEXT_NAMESPACE
