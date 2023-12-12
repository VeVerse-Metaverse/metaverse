// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#include "UIFileBrowserWidget.h"
#include "PlatformFileInfo.h"
#include "VeShared.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIFileBrowserWidget::NativeConstruct() {
	Super::NativeConstruct();

	// if (BreadcrumbTrailWidget) {
	// 	BreadcrumbTrailWidget->Reset();
	// 	BreadcrumbTrailWidget->PushCrumb(LOCTEXT("Objects", "Objects"));
	// 	BreadcrumbTrailWidget->PushCrumb(LOCTEXT("ObjectDetails", "Object Details"));
	// 	BreadcrumbTrailWidget->PushCrumb(LOCTEXT("ObjectForm", "Edit Object"));
	// 	BreadcrumbTrailWidget->PushCrumb(LOCTEXT("FileBrowser", "File Browser"));
	// }
}

void UUIFileBrowserWidget::RegisterCallbacks() {
	if (CloseButtonWidget) {
		if (!OnCloseButtonWidgetDelegateHandle.IsValid()) {
			OnCloseButtonWidgetDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIFileBrowserWidget::OnCloseButtonClicked);
		}
	}

	if (CancelButtonWidget) {
		if (!OnCancelButtonWidgetDelegateHandle.IsValid()) {
			OnCancelButtonWidgetDelegateHandle = CancelButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIFileBrowserWidget::OnCancelButtonClicked);
		}
	}

	if (!OnFileListItemClickedDelegateHandle.IsValid()) {
		OnFileListItemClickedDelegateHandle = OnBrowseListItemClicked.AddUObject(this, &UUIFileBrowserWidget::OnFileItemClicked);
	}

	if (OpenButtonWidget) {
		if (!OnOpenButtonWidgetDelegateHandle.IsValid()) {
			OnOpenButtonWidgetDelegateHandle = OpenButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIFileBrowserWidget::OnOpenButtonClicked);
		}
	}

	if (ContainerWidget) {
		// Set the selection mode to single to allow the container widget to select a single file. 
		ContainerWidget->SetSelectionMode(ESelectionMode::Single);
		ContainerWidget->OnItemClicked().AddUObject(this, &UUIFileBrowserWidget::OnListItemClicked);
		ContainerWidget->OnItemIsHoveredChanged().AddUObject(this, &UUIFileBrowserWidget::OnListItemIsHoveredChanged);
		ContainerWidget->OnItemDoubleClicked().AddUObject(this, &UUIFileBrowserWidget::OnListItemDoubleClicked);
	}
}

void UUIFileBrowserWidget::UnregisterCallbacks() {
	if (OnCloseButtonWidgetDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonWidgetDelegateHandle);
			OnCloseButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnCancelButtonWidgetDelegateHandle.IsValid()) {
		if (CancelButtonWidget) {
			CancelButtonWidget->GetOnButtonClicked().Remove(OnCancelButtonWidgetDelegateHandle);
			OnCancelButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnFileListItemClickedDelegateHandle.IsValid()) {
		OnBrowseListItemClicked.Remove(OnFileListItemClickedDelegateHandle);
		OnFileListItemClickedDelegateHandle.Reset();
	}

	if (OnOpenButtonWidgetDelegateHandle.IsValid()) {
		if (OpenButtonWidget) {
			OpenButtonWidget->GetOnButtonClicked().Remove(OnOpenButtonWidgetDelegateHandle);
			OnOpenButtonWidgetDelegateHandle.Reset();
		}
	}
}

void UUIFileBrowserWidget::OnListItemIsHoveredChanged(UObject* Item, bool bIsHovered) {
	if (bIsHovered) {
		PlaySound(ListItemHoverInSound);
	} else {
		PlaySound(ListItemHoverOutSound);
	}
}

void UUIFileBrowserWidget::OnListItemClicked(UObject* InClickedItem) {
	PlaySound(ListItemClickSound);
}

void UUIFileBrowserWidget::OnListItemDoubleClicked(UObject* InClickedItem) {
	PlaySound(ListItemDoubleClickSound);

	if (const UPlatformFileInfo* Item = Cast<UPlatformFileInfo>(InClickedItem)) {
		const FPlatformFileInfo File = Item->Metadata;

		if (File.bIsDirectory) {
			UpdateFileList(File.Path, AllowedExtensions);
		} else {
			if (OnFileSelected.IsBound()) {
				OnFileSelected.Broadcast(File);
			}

			if (OnClosed.IsBound()) {
				OnClosed.Broadcast();
			}
		}
	}
}

void UUIFileBrowserWidget::OnCloseButtonClicked() const {
	if (OnClosed.IsBound()) {
		OnClosed.Broadcast();
	}
}

void UUIFileBrowserWidget::OnOpenButtonClicked() {

	if (ContainerWidget) {
		if (const UPlatformFileInfo* Item = ContainerWidget->GetSelectedItem<UPlatformFileInfo>()) {
			const FPlatformFileInfo File = Item->Metadata;

			if (File.bIsDirectory) {
				UpdateFileList(File.Path, AllowedExtensions);
			} else {
				if (OnFileSelected.IsBound()) {
					OnFileSelected.Broadcast(File);
				}

				if (OnClosed.IsBound()) {
					OnClosed.Broadcast();
				}
			}
		}
	}
}

void UUIFileBrowserWidget::OnFileItemClicked(UPlatformFileInfo* InFile) {
	if (InFile) {
		if (OnFileSelected.IsBound()) {
			OnFileSelected.Broadcast(InFile->Metadata);
		}
		UpdateFileList(FPaths::ProjectContentDir(), AllowedExtensions);

	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = NSLOCTEXT("VeUI", "OpenFileError", "Error");
			NotificationData.Message = FText::FromString("Failed to load the File.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIFileBrowserWidget::UpdateFileList(const FString& InPath, const TArray<FString>& InAllowedExtensions) {
	const FString Path = InPath.IsEmpty() ? FPaths::ProjectContentDir() : InPath;

	TArray<FPlatformFileInfo> Files;

	if (InAllowedExtensions.Num() > 0) {
		FPlatformFileFilter Filter;
		Filter.BindWeakLambda(this, [InAllowedExtensions](const FPlatformFileInfo& File) -> bool {
			if (InAllowedExtensions.Contains(File.Extension) || File.bIsDirectory) {
				return true;
			}
			return false;
		});
		Files = GetFilesInDirectory(Path, &Filter, true);
		Filter.Unbind();
	} else {
		Files = GetFilesInDirectory(Path, nullptr, true);
	}

	// Get the parent directory path. 
	FString LocalPath = Path;
	FPaths::NormalizeDirectoryName(LocalPath);
	FString ParentDir = FPaths::GetPath(LocalPath);
	FPlatformFileInfo ParentDirectoryInfo;
	ParentDirectoryInfo.bIsDirectory = true;
	ParentDirectoryInfo.Directory = FPaths::GetPath(ParentDir);
	ParentDirectoryInfo.Name = TEXT("..");
	ParentDirectoryInfo.Path = ParentDir;
	ParentDirectoryInfo.Size = -1;
	ParentDirectoryInfo.UpdatedAt = 0;

	if (!ParentDirectoryInfo.Path.IsEmpty()) {
		Files.Insert(ParentDirectoryInfo, 0);
	}

	if (Files.Num() > 0) {
		if (ContainerWidget) {
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UPlatformFileInfo::BatchToUObjectArray(Files, this));
		}
	}
}

void UUIFileBrowserWidget::OnCancelButtonClicked() const {
	if (OnClosed.IsBound()) {
		OnClosed.Broadcast();
	}
}

#undef LOCTEXT_NAMESPACE
