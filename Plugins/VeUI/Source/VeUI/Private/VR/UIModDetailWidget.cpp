// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIModDetailWidget.h"

#include "ApiSpaceMetadata.h"
#include "VeShared.h"
#include "DownloadChunkSubsystem.h"
#include "VeHUD.h"
#include "UINotificationData.h"
// #include "../../../../VeShared/Source/VePak/VePak.h"
#include "Kismet/GameplayStatics.h"
#include "VeDownload.h"
#include "VePak/Public/PakPakSubsystem.h"
#include "VeUI.h"
#include "AnimatedTexture2D.h"


#define LOCTEXT_NAMESPACE "VeUI"

void UUIModDetailWidget::NativeConstruct() {
	bZoomImageWidgetVisible = true;
	LastProgressTime = FDateTime::MinValue();

	HideZoomImageWidget(false, false);

	Super::NativeConstruct();
}

void UUIModDetailWidget::RegisterCallbacks() {
	if (CloseButtonWidget) {
		if (!OnCloseButtonWidgetDelegateHandle.IsValid()) {
			OnCloseButtonWidgetDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosed.IsBound()) {
					OnClosed.Broadcast();
				}
			});
		}
	}

	if (PreviewImageButtonWidget) {
		if (!OnPreviewButtonClickedDelegateHandle.IsValid()) {
			OnPreviewButtonClickedDelegateHandle = PreviewImageButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (ZoomImageWidget) {
					FApiFileMetadata* PreviewImage = Metadata->Files.FindByPredicate([=](const FApiFileMetadata& File) {
						return File.Type == EApiFileType::ImageFull;
					});
					if (PreviewImage) {
						ZoomImageWidget->SetMetadata(*PreviewImage);
						ShowZoomImageWidget();
					}
				}
			});
		}
	}

	if (DeleteButtonWidget) {
		if (!OnDeleteButtonWidgetDelegateHandle.IsValid()) {
			OnDeleteButtonWidgetDelegateHandle = DeleteButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIModDetailWidget::RequestObjectDeleteConfirmation);
		}
	}

	if (InstallButtonWidget) {
		if (!OnInstallButtonWidgetDelegateHandle.IsValid()) {
			OnInstallButtonWidgetDelegateHandle = InstallButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIModDetailWidget::InstallModButtonClicked);
		}
	}

	if (VisitButtonWidget) {
		if (!OnVisitButtonWidgetDelegateHandle.IsValid()) {
			OnVisitButtonWidgetDelegateHandle = VisitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIModDetailWidget::VisitModButtonClicked);
		}
	}
	if (HostButtonWidget) {
		if (!OnHostButtonWidgetDelegateHandle.IsValid()) {
			OnHostButtonWidgetDelegateHandle = HostButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIModDetailWidget::HostModButtonClicked);
		}
	}

	if (JoinButtonWidget) {
		if (!OnJoinButtonWidgetDelegateHandle.IsValid()) {
			OnJoinButtonWidgetDelegateHandle = JoinButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIModDetailWidget::JoinModButtonClicked);
		}
	}

	// if (OpenButtonWidget) {
	// 	OpenButtonWidget->OnButtonClicked.AddUObject(this, &UUIModDetailWidget::OpenModButtonClicked);
	// }

	if (ZoomImageWidget) {
		if (!OnZoomImageWidgetClosedDelegateHandle.IsValid())
			OnZoomImageWidgetClosedDelegateHandle = ZoomImageWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				HideZoomImageWidget();
			});
	}
}

void UUIModDetailWidget::UnregisterCallbacks() {
	if (OnCloseButtonWidgetDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonWidgetDelegateHandle);
			OnCloseButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnPreviewButtonClickedDelegateHandle.IsValid()) {
		if (PreviewImageButtonWidget) {
			PreviewImageButtonWidget->GetOnButtonClicked().Remove(OnPreviewButtonClickedDelegateHandle);
			OnPreviewButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnInstallButtonWidgetDelegateHandle.IsValid()) {
		if (InstallButtonWidget) {
			InstallButtonWidget->GetOnButtonClicked().Remove(OnInstallButtonWidgetDelegateHandle);
			OnInstallButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnDeleteButtonWidgetDelegateHandle.IsValid()) {
		if (DeleteButtonWidget) {
			DeleteButtonWidget->GetOnButtonClicked().Remove(OnDeleteButtonWidgetDelegateHandle);
			OnDeleteButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnVisitButtonWidgetDelegateHandle.IsValid()) {
		if (VisitButtonWidget) {
			VisitButtonWidget->GetOnButtonClicked().Remove(OnVisitButtonWidgetDelegateHandle);
			OnVisitButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnZoomImageWidgetClosedDelegateHandle.IsValid()) {
		if (ZoomImageWidget) {
			ZoomImageWidget->OnClosedDelegate.Remove(OnZoomImageWidgetClosedDelegateHandle);
			OnZoomImageWidgetClosedDelegateHandle.Reset();
		}
	}

	if (BoundDownload.IsValid()) BoundDownload.Reset();
}

void UUIModDetailWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	// LogScreenLogF("Texture Loaded at ObjectDetailWidget %s", *InResult.Url);
	
	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
}

void UUIModDetailWidget::InstallModButtonClicked() {
	if (const FApiFileMetadata* ModFile = GetModPakFileMetadata()) {
		if (!ModFile->Url.IsEmpty()) {
			if (BoundDownload.IsValid()) return;

			if (FVeDownloadModule* DownloadModule = FVeDownloadModule::Get()) {
				if (const TSharedPtr<FDownloadChunkSubsystem> ChunkSubsystem = DownloadModule->GetChunkSubsystem()) {
					BoundDownload = ChunkSubsystem->DownloadFile({ Metadata->Id, ModFile->Url, GetModPakPath() }, 1, false);
					if (BoundDownload.IsValid()) {
						BoundDownload->OnProgress().BindUObject(this, &UUIModDetailWidget::OnModFileDownloadProgress);
						BoundDownload->OnComplete().BindUObject(this, &UUIModDetailWidget::OnModFileDownloadComplete);
					}
						
					bModPakFileExists = false;
				} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Download), STRINGIFY(Chunk)); }
			} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Download)); };
		}
	}

	FUINotificationData NotificationData;
	NotificationData.Lifetime = 5.f;
	NotificationData.Header = LOCTEXT("DownloadVirtualWorldStartedNotificationHeader", "Started");
	NotificationData.Type = EUINotificationType::Info;
	NotificationData.Message = LOCTEXT("DownloadVirtualWorldStartedNotificationMessage", "The download of the virtual world has started");
	
	AddNotification(NotificationData);
	bIsProcessing = true;
	UpdateButtons();
}

void UUIModDetailWidget::DeleteModButtonClicked() {
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (const FApiFileMetadata* ModFile = GetModPakFileMetadata()) {
		auto Path = GetModPakPath();
		if (FPakPakSubsystem::IsPakMounted(Path) && !FPakPakSubsystem::UnmountPakFile(Path)) {
			LogErrorF("Failed to unmount pak", STRINGIFY(Download), STRINGIFY(Chunk));
		}
		if (FVeDownloadModule* DownloadModule = FVeDownloadModule::Get()) {
			if (const TSharedPtr<FDownloadChunkSubsystem> ChunkSubsystem = DownloadModule->GetChunkSubsystem()) {
				ChunkSubsystem->TryRemove({ Metadata->Id,  ModFile->Url, Path });
			}
		}
	}

	UpdateButtons();
}

void UUIModDetailWidget::VisitModButtonClicked() {
	const FApiFileMetadata* PakMetadata = GetModPakFileMetadata();
	// const FGuid ModPakId = ModPakMetadata->Id;
	// const FString ModFilePath = GetModPakPath(ModPakId);

	// if (FVePakModule* PakModule = FVePakModule::Get()) {
	// 	if (const TSharedPtr<FPakPakSubsystem> PakSubsystem = PakModule->GetPakSubsystem()) {
	// 		{
	// 			if (!PakSubsystem->MountMetaversePakFile(PakMetadata->Id)) {
	// 				LogErrorF("failed to mount virtual world pak file %s", *PakMetadata->Id.ToString());
	// 				return;
	// 			}
	// 			PakSubsystem->RegisterMetaverseMountPoint(Metadata->Name);
	// 			// PakSubsystem->LogFilesInDirectoryRecursively(TEXT("Metaverse"));
	// 			PakSubsystem->LoadPakAssetRegistry(Metadata->Name);
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Pak), STRINGIFY(Pak)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Pak)); };

	UGameplayStatics::OpenLevel(GetOwningPlayer(), FName(Metadata->Map), true, TEXT("")); // ?listen

	// const bool bResult = FPakLoader::Get()->MountPakFile(ModFilePath, INDEX_NONE, FString());
	// if (!bResult) {
	// 	LogErrorF("Failed to mount the mod pak file");
	// 	return;
	// }
	//
	// FPakSubsystemPtr PakSubsystemPtr = FVePakModule::GetPakSubsystem();
	// PakSubsystemPtr->MountPakFile();
	//
	// if (Metadata->Name.IsEmpty()) {
	// 	LogErrorF("Metadata name is empty");
	// }
	//
	// const FString RootPath = FString::Printf(TEXT("/%s/"), *Metadata->Name);
	// // const FString RootPath = FString::Printf(TEXT("/%s/"), TEXT("Game"));
	// const FString ContentPath = FString::Printf(TEXT("../../../Metaverse/Plugins/%s/Content"), *Metadata->Name);
	// FPakLoader::Get()->RegisterMountPoint(RootPath, ContentPath);
	//
	// auto List = FPakLoader::Get()->GetMountedPakFilenames();
	// for (auto F : List) {
	// 	LogScreenLogF("Mounted pak: %s", *F);
	// }

	// UGameplayStatics::OpenLevel(GetWorld(), FName("Example"));
}

void UUIModDetailWidget::HostModButtonClicked() {
	const FApiFileMetadata* PakMetadata = GetModPakFileMetadata();

	// x download 
	// x mount
	// x register mount point
	// x load asset registry
	// x determine the map name from the space
	// xx add the space to the mod metadata
	// x travel to the map with ?listen option

	// if (FVePakModule* PakModule = FVePakModule::Get()) {
	// 	if (const TSharedPtr<FPakPakSubsystem> PakSubsystem = PakModule->GetPakSubsystem()) {
	// 		{
	// 			if (!PakSubsystem->MountMetaversePakFile(PakMetadata->Id)) {
	// 				LogErrorF("failed to mount virtual world pak file %s", *PakMetadata->Id.ToString());
	// 				return;
	// 			}
	// 			// Register mount point.
	// 			PakSubsystem->RegisterMetaverseMountPoint(Metadata->Name);
	// 			// Debug
	// 			PakSubsystem->LogFilesInDirectoryRecursively(TEXT("Metaverse"));
	// 			// Load asset registry from the pak.
	// 			PakSubsystem->LoadPakAssetRegistry(Metadata->Name);
	//
	// 			// if (UUIMenuSubsystem* MenuSubsystem = GetWorld()->GetSubsystem<UUIMenuSubsystem>()) {
	// 			// 	if (Metadata->Space) {
	// 			// 		if (!Metadata->Space->Map.IsEmpty()) {
	// 			// 			if (GetOwningPlayer() && GetOwningPlayer()->IsLocalController()) {
	// 			// 				MenuSubsystem->GetOnHostGame().Broadcast(Metadata->Space->Map);
	// 			// 			}
	// 			// 		}
	// 			// 	}
	// 			// }
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Pak), STRINGIFY(Pak)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Pak)); };

	// const bool bResult = FPakLoader::Get()->MountPakFile(ModFilePath, INDEX_NONE, FString());
	// if (!bResult) {
	// 	LogErrorF("Failed to mount the mod pak file");
	// 	return;
	// }
	//
	// FPakSubsystemPtr PakSubsystemPtr = FVePakModule::GetPakSubsystem();
	// PakSubsystemPtr->MountPakFile();
	//
	// if (Metadata->Name.IsEmpty()) {
	// 	LogErrorF("Metadata name is empty");
	// }
	//
	// const FString RootPath = FString::Printf(TEXT("/%s/"), *Metadata->Name);
	// // const FString RootPath = FString::Printf(TEXT("/%s/"), TEXT("Game"));
	// const FString ContentPath = FString::Printf(TEXT("../../../Metaverse/Plugins/%s/Content"), *Metadata->Name);
	// FPakLoader::Get()->RegisterMountPoint(RootPath, ContentPath);
	//
	// auto List = FPakLoader::Get()->GetMountedPakFilenames();
	// for (auto F : List) {
	// 	LogScreenLogF("Mounted pak: %s", *F);
	// }

	// UGameplayStatics::OpenLevel(GetWorld(), FName("Example"));
}

void UUIModDetailWidget::JoinModButtonClicked() {
	const FApiFileMetadata* PakMetadata = GetModPakFileMetadata();
	// const FGuid ModPakId = ModPakMetadata->Id;
	// const FString ModFilePath = GetModPakPath(ModPakId);

	// if (FVePakModule* PakModule = FVePakModule::Get()) {
	// 	if (const TSharedPtr<FPakPakSubsystem> PakSubsystem = PakModule->GetPakSubsystem()) {
	// 		{
	// 			if (!PakSubsystem->MountMetaversePakFile(PakMetadata->Id)) {
	// 				LogErrorF("failed to mount virtual world pak file %s", *PakMetadata->Id.ToString());
	// 				return;
	// 			}
	// 			PakSubsystem->RegisterMetaverseMountPoint(Metadata->Name);
	// 			PakSubsystem->LogFilesInDirectoryRecursively(TEXT("Metaverse"));
	// 			PakSubsystem->LoadPakAssetRegistry(Metadata->Name);
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Pak), STRINGIFY(Pak)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Pak)); };

	// const bool bResult = FPakLoader::Get()->MountPakFile(ModFilePath, INDEX_NONE, FString());
	// if (!bResult) {
	// 	LogErrorF("Failed to mount the mod pak file");
	// 	return;
	// }
	//
	// FPakSubsystemPtr PakSubsystemPtr = FVePakModule::GetPakSubsystem();
	// PakSubsystemPtr->MountPakFile();
	//
	// if (Metadata->Name.IsEmpty()) {
	// 	LogErrorF("Metadata name is empty");
	// }
	//
	// const FString RootPath = FString::Printf(TEXT("/%s/"), *Metadata->Name);
	// // const FString RootPath = FString::Printf(TEXT("/%s/"), TEXT("Game"));
	// const FString ContentPath = FString::Printf(TEXT("../../../Metaverse/Plugins/%s/Content"), *Metadata->Name);
	// FPakLoader::Get()->RegisterMountPoint(RootPath, ContentPath);
	//
	// auto List = FPakLoader::Get()->GetMountedPakFilenames();
	// for (auto F : List) {
	// 	LogScreenLogF("Mounted pak: %s", *F);
	// }

	// UGameplayStatics::OpenLevel(GetWorld(), FName("Example"));
}

void UUIModDetailWidget::UpdateButtons() {
	if (InstallButtonWidget) {
		if (GetPakFileExists()) {
			InstallButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			InstallButtonWidget->SetIsEnabled(true);
		} else if (GetPakFileExists() && bModPakFileExists) {
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
		} else if (!GetPakFileExists() && !bModPakFileExists) {
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

	if (VisitButtonWidget) {
		if (!GetPakFileExists()) {
			VisitButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			VisitButtonWidget->SetIsEnabled(true);
		} else if (!GetPakFileExists() && !bModPakFileExists) {
			VisitButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			VisitButtonWidget->SetIsEnabled(true);
		} else if (bIsProcessing) {
			VisitButtonWidget->SetVisibility(ESlateVisibility::Visible);
			VisitButtonWidget->SetIsEnabled(false);
		} else {
			VisitButtonWidget->SetVisibility(ESlateVisibility::Visible);
			VisitButtonWidget->SetIsEnabled(true);
		}
	}
}

void UUIModDetailWidget::RequestObjectDeleteConfirmation() {
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
			DeleteModButtonClicked();
		}
	});

	const auto _ = GetHUD()->ShowDialog(DialogData, OnDialogButtonClicked);
}

FApiFileMetadata* UUIModDetailWidget::GetModPakFileMetadata() {
	return Metadata->Files.FindByPredicate([=](const FApiFileMetadata& File) {
		return File.Type == EApiFileType::Pak && File.Platform == FVeSharedModule::GetPlatformString() && File.DeploymentType == EApiFileDeploymentType::Client;
	});
}

bool UUIModDetailWidget::GetPakFileExists() {
	const FApiFileMetadata* ModFile = GetModPakFileMetadata();

	if (ModFile) {
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		return PlatformFile.FileExists(*GetModPakPath());
	}

	return false;
}

bool UUIModDetailWidget::GetPakMetadataExists() {
	return !!GetModPakFileMetadata();
}

FString UUIModDetailWidget::GetModPakPath() {
	return FPakPakSubsystem::GetMetaversePakPath(Metadata->Id);
}


void UUIModDetailWidget::OnModFileDownloadProgress(const FChunkDownloadProgress& Result) {
	FUINotificationData NotificationData;

	auto t = FDateTime::UtcNow();
	// [FIXME] make time interval configurable
	if ((t - LastProgressTime).GetTotalSeconds() < 1) return;
	LastProgressTime = t;
	NotificationData.Lifetime = 1.f;

	NotificationData.Header = LOCTEXT("DownloadModSuccessNotificationHeader", "Downloading");
	NotificationData.Type = EUINotificationType::Success;
	NotificationData.Message = FText::Format(LOCTEXT("DownloadModSuccessNotificationMessage", "Downloaded {0} of {1} ({2})."), FText::AsMemory((int64)(Result.iTotalSize * Result.fProgress)), FText::AsMemory(Result.iTotalSize), FText::AsPercent(Result.fProgress));
	
	AddNotification(NotificationData);
}

void UUIModDetailWidget::OnModFileDownloadComplete(const FFileDownloadResult& Result) {
	//LogScreenLogF("OnComplete %d %s %s", Result.bSuccessful, *Result.Path, *Result.Url);
	FUINotificationData NotificationData;
	NotificationData.Lifetime = 5.f;

	if (BoundDownload.IsValid()) {
		BoundDownload.Reset();
	}

	if (Result.bSuccessful) {
		NotificationData.Header = LOCTEXT("DownloadModSuccessNotificationHeader", "Success");
		NotificationData.Type = EUINotificationType::Success;
		NotificationData.Message = LOCTEXT("DownloadModSuccessNotificationMessage", "Successfully downloaded the virtual world.");
	} else {
		NotificationData.Header = LOCTEXT("DownloadModErrorNotificationHeader", "Error");
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = FText::Format(LOCTEXT("DeleteObjectErrorNotificationMessage", "Failed to download the virtual world: {0}"), FText::FromString(Result.Error));
	}
	bModPakFileExists = true;
	bIsProcessing = false;
	AddNotification(NotificationData);
}

void UUIModDetailWidget::SetMetadata(const TSharedPtr<FApiPackageMetadata> InMod) {
	Metadata = InMod;

	if (!Metadata.IsValid()) return;

	const auto PreviewImage = Metadata->Files.FindByPredicate([=](const FApiFileMetadata& File) {
		return File.Type == EApiFileType::ImagePreview;
	});

	if (!PreviewImage->Url.IsEmpty()) {
		RequestTexture(this, PreviewImage->Url, TEXTUREGROUP_UI);
	}

	if (NameTextWidget) {
		if (Metadata->Name.IsEmpty()) {
			NameTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldNameUnknown", "Unnamed"));
		} else {
			NameTextWidget->SetText(FText::FromString(Metadata->Name));
		}
	}

	if (PriceTextWidget) {
		if (Metadata->Price > 0) {
			PriceTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldPriceFormat", "{0}"), FText::AsCurrency(Metadata->Price)));
		} else {
			PriceTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldPriceFree", "Free"));
		}
	}

	if (DescriptionTextWidget) {
		if (Metadata->Description.IsEmpty()) {
			DescriptionTextWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata->Description));
		}
	}

	if (AuthorTextWidget) {
		if (Metadata->Owner.IsValid() && !Metadata->Owner->Name.IsEmpty()) {
			AuthorTextWidget->SetText(FText::FromString(Metadata->Owner->Name));
		} else {
			AuthorTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldAuthorUnknown", "Unknown"));
		}
	}

	if (DescriptionTextWidget) {
		if (Metadata->Summary.IsEmpty()) {
			DescriptionTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldSummaryUnknown", "No description"));
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata->Summary));
		}
	}

	if (VersionTextWidget) {
		if (Metadata->Description.IsEmpty()) {
			VersionTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldVersionUnknown", "No version"));
		} else {
			VersionTextWidget->SetText(FText::FromString(Metadata->Version));
		}
	}

	if (ReleaseDateTextWidget) {
		if (Metadata->ReleasedAt == 0) {
			ReleaseDateTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldReleaseDateUnknown", "No release date"));
		} else {
			ReleaseDateTextWidget->SetText(FText::FromString(Metadata->Version));
		}
	}

	if (ViewCountTextWidget) {
		ViewCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldViewCountFormat", "{0}"), FText::AsNumber(Metadata->Views)));
	}

	if (DownloadCountTextWidget) {
		DownloadCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldDownloadCountFormat", "{0}"), FText::AsNumber(Metadata->Downloads)));
	}

	if (LikeCountTextWidget) {
		LikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldLikeCountFormat", "{0}"), FText::AsNumber(Metadata->Likes)));
	}

	if (DislikeCountTextWidget) {
		DislikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldLikeCountFormat", "{0}"), FText::AsNumber(Metadata->Dislikes)));
	}

	if (RatingTextWidget) {
		if (Metadata->Likes + Metadata->Dislikes > 0) {
			const float Value = (static_cast<float>(Metadata->Likes) / (Metadata->Likes + Metadata->Dislikes) * 5 - 1) / 4 * 5;
			RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "VirtualWorldRatingFormat", "{0}"), FText::AsNumber(Value)));
		} else {
			RatingTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldRatingNone", "Not rated"));
		}
	}

	if (DownloadSizeTextWidget) {
		if (Metadata->DownloadSize > 0) {
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.SetMaximumFractionalDigits(2);
			DownloadSizeTextWidget->SetText(
				FText::Format(NSLOCTEXT("VeUI", "VirtualWorldDownloadSizeFormat", "{0}"), FText::AsMemory(Metadata->DownloadSize, &NumberFormattingOptions, nullptr, SI)));
		} else {
			DownloadSizeTextWidget->SetText(NSLOCTEXT("VeUI", "VirtualWorldDownloadSizeUnknown", "?"));
		}
	}

	UpdateButtons();
}

void UUIModDetailWidget::HideZoomImageWidget(bool bAnimate, bool bPlaySound) {
	if (bZoomImageWidgetVisible) {
		bZoomImageWidgetVisible = false;
		if (ZoomImageWidget) {
			ZoomImageWidget->Hide();
		}
	}
}

void UUIModDetailWidget::ShowZoomImageWidget(bool bAnimate, bool bPlaySound) {

	if (!bZoomImageWidgetVisible) {
		bZoomImageWidgetVisible = true;
		if (ZoomImageWidget) {
			// ZoomImageWidget->ResetState();
			// ZoomImageWidget->ResetAnimationState();
			ZoomImageWidget->Show();
		}
	}
}

#undef LOCTEXT_NAMESPACE
