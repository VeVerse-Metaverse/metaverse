// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIPlaceableBrowserWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VR/UIObjectListWidget.h"
#include "VR/UIOpenSeaAssetListWidget.h"
#include "VeApi.h"
#include "VeDownload.h"
#include "VeUI.h"
#include "FileHeaderRequest.h"
#include "DownloadChunkSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIPlaceableBrowserWidget::OnNavigate() {
	ObjectListWidget->Refresh();
	OpenSeaAssetListWidget->Refresh();

	Super::OnNavigate();
}

void UUIPlaceableBrowserWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (IsValid(TabBoxWidget)) {
		if (!TabBoxWidget->OnTabSelectedEvent.IsAlreadyBound(this, &UUIPlaceableBrowserWidget::OnTabSelectedCallback)) {
			TabBoxWidget->OnTabSelectedEvent.AddDynamic(this, &UUIPlaceableBrowserWidget::OnTabSelectedCallback);
		}
	}

	NavigateToOpenSeaNftListWidget();

	OnNavigate();
}

void UUIPlaceableBrowserWidget::RegisterCallbacks() {
	if (!OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			OnCloseButtonClickedDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
			});
		}
	}

	if (OnObjectSelectedDelegateHandle.IsValid()) {
		OnObjectSelectedDelegateHandle = ObjectListWidget->OnObjectSelected.AddWeakLambda(this, [=](TSharedPtr<FApiObjectMetadata> InMetadata) {
			// Broadcast new Object Id
			//OnObjectSelected(InMetadata->Id);
			if (FVeUIModule* UIModule = FVeUIModule::Get()) {
				if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
					{
						BreadcrumbSubsystem->Pop();
					}
				} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
			} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

			if (OnClosedDelegate.IsBound()) {
				OnClosedDelegate.Broadcast();
			}

			GetOwningPlayer()->SetShowMouseCursor(false);
			GetOwningPlayer()->SetInputMode(FInputModeGameOnly{});
			RemoveFromParent();
		});
	}

	if (!OnOpenSeaAssetSelectedDelegateHandle.IsValid()) {
		if (IsValid(OpenSeaAssetListWidget)) {
			OnOpenSeaAssetSelectedDelegateHandle = OpenSeaAssetListWidget->OnObjectSelected.AddWeakLambda(
				this, [this](const FOpenSeaAssetMetadata& InMetadata) {
					if (FVeUIModule* UIModule = FVeUIModule::Get()) {
						if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
							{
								BreadcrumbSubsystem->Pop();
							}
						} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
					} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

					if (OnClosedDelegate.IsBound()) {
						OnClosedDelegate.Broadcast();
					}


					GetOwningPlayer()->SetShowMouseCursor(false);
					GetOwningPlayer()->SetInputMode(FInputModeGameOnly{});
					RemoveFromParent();

					if (FVeDownloadModule* DownloadModule = FVeDownloadModule::Get()) {
						if (const TSharedPtr<FDownloadChunkSubsystem> ChunkDownloadSubsystem = DownloadModule->GetChunkSubsystem()) {
							const auto CallbackPtr = MakeShared<FOnFileHeaderRequestComplete>();
							CallbackPtr->BindWeakLambda(this, [=](const FFileHeaderResult& InResult, const bool bInSuccessful, const FString& InError) {
								if (bInSuccessful) {
									const auto MimeType = InResult.GetContentMimeType();
									auto Metadata = InMetadata;
									Metadata.SetMimeType(MimeType);

									// if (FVeUIModule* UIModule = FVeUIModule::Get()) {
										// if (const TSharedPtr<FUIPlaceableSubsystem> PlaceableSubSystem = UIModule->GetPlaceableSubsystem()) {
											//PlaceableSubSystem->NotifyOnOpenSeaAssetSelected(InMetadata.GetImageUrl(), Metadata.MediaType, Metadata.MimeType);
										// } else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
									// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }

								} else {
									LogErrorF("failed to load nft asset: %s", *InError);
								}
							});
							ChunkDownloadSubsystem->GetFileHeaders(InMetadata.GetImageUrl(), CallbackPtr);
						} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Download), STRINGIFY(ChunkDownload)); }
					} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Download)); }
				});
		} else {
			LogErrorF("placeable browser widget has no valid open sea nft list widget");
		}
	}
	if (!OnPlaceableUrlWidgetDelegateHandle.IsValid()) {
		if (IsValid(PlaceableUrlWidget)) {
			OnPlaceableUrlWidgetDelegateHandle = PlaceableUrlWidget->OnUrlChanged.AddWeakLambda(
				this, [this](const FString& Url) {

					if (FVeUIModule* UIModule = FVeUIModule::Get()) {
						if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
							{
								BreadcrumbSubsystem->Pop();
							}
						} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
					} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

					if (OnClosedDelegate.IsBound()) {
						OnClosedDelegate.Broadcast();
					}

					GetOwningPlayer()->SetShowMouseCursor(false);
					GetOwningPlayer()->SetInputMode(FInputModeGameOnly{});
					RemoveFromParent();
					if (FVeDownloadModule* DownloadModule = FVeDownloadModule::Get()) {
						if (const TSharedPtr<FDownloadChunkSubsystem> ChunkDownloadSubsystem = DownloadModule->GetChunkSubsystem()) {
							const auto CallbackPtr = MakeShared<FOnFileHeaderRequestComplete>();
							CallbackPtr->BindWeakLambda(this, [=](const FFileHeaderResult& InResult, const bool bInSuccessful, const FString& InError) {
								if (bInSuccessful) {
									const auto MimeType = InResult.GetContentMimeType();
									FString MediaType;
									if (MimeType.StartsWith(TEXT("image/"))) {
										MediaType = "image_2d";
									} else if (MimeType.EndsWith(TEXT("mp4"))) {
										MediaType = "video_2d";
									} else if (MimeType.StartsWith(TEXT("model/")) && MimeType.Contains(TEXT("gltf"))) {
										MediaType = "mesh_3d";
									} else if (MimeType.StartsWith(TEXT("audio/"))) {
										MediaType = "audio";
									} else {
										MediaType = "unknown";
									}
									// if (FVeUIModule* UIModule = FVeUIModule::Get()) {
									// 	if (const TSharedPtr<FUIPlaceableSubsystem> PlaceableSubSystem = UIModule->GetPlaceableSubsystem()) {
									// 		// PlaceableSubSystem->NotifyOnPlaceableUrlSelected(Url, MediaType, MimeType);
									// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
									// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
								} else {
									LogErrorF("failed to load nft asset: %s", *InError);
								}
							});
							ChunkDownloadSubsystem->GetFileHeaders(Url, CallbackPtr);
						} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Download), STRINGIFY(ChunkDownload)); }
					} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Download)); }
				});
		} else {
			LogErrorF("placeable browser widget has no valid open sea nft list widget");
		}
	}

	// if (FVeUIModule* UIModule = FVeUIModule::Get()) {
	// 	if (const TSharedPtr<FUIPlaceableSubsystem> PlaceableSubsystem = UIModule->GetPlaceableSubsystem()) {
	// 		if (!OnShowPlaceableBrowserDelegateHandle.IsValid()) {
	// 			// OnShowPlaceableBrowserDelegateHandle = PlaceableSubsystem->GetOnPlaceableBrowserOpen().AddWeakLambda(this,[=] {
	// 				// NavigateToOpenSeaNftListWidget();
	// 			// });
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(PlaceableSubsystem)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }

}

void UUIPlaceableBrowserWidget::UnregisterCallbacks() {
	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnObjectSelectedDelegateHandle.IsValid()) {
		ObjectListWidget->OnObjectSelected.Remove(OnObjectSelectedDelegateHandle);
		OnObjectSelectedDelegateHandle.Reset();
	}

	if (OnOpenSeaAssetSelectedDelegateHandle.IsValid()) {
		OpenSeaAssetListWidget->OnObjectSelected.Remove(OnOpenSeaAssetSelectedDelegateHandle);
		OnOpenSeaAssetSelectedDelegateHandle.Reset();
	}

	if (OnPlaceableUrlWidgetDelegateHandle.IsValid()) {
		PlaceableUrlWidget->OnUrlChanged.Remove(OnPlaceableUrlWidgetDelegateHandle);
		OnPlaceableUrlWidgetDelegateHandle.Reset();
	}

	if (OnObjectBrowserClosedDelegateHandle.IsValid()) {
		OnClosedDelegate.Remove(OnObjectBrowserClosedDelegateHandle);
		OnObjectBrowserClosedDelegateHandle.Reset();
	}

	if (!OnShowPlaceableBrowserDelegateHandle.IsValid()) {
		// if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		// 	if (const TSharedPtr<FUIPlaceableSubsystem> PlaceableSubSystem = UIModule->GetPlaceableSubsystem()) {
		// 		// PlaceableSubSystem->GetOnPlaceableBrowserOpen().Remove(OnShowPlaceableBrowserDelegateHandle);
		// 		OnShowPlaceableBrowserDelegateHandle.Reset();
		// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
		// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	}
}

void UUIPlaceableBrowserWidget::OnTabSelectedCallback(int Index, FString TabName) {
	switch (Index) {
	case 0:
		NavigateToOpenSeaNftListWidget();
		break;
	case 1:
		NavigateToObjectList();
		break;
	case 2:
		NavigateToPlaceableUrlWidget();
		break;
	default:
		NavigateToOpenSeaNftListWidget();
		break;
	}
}

void UUIPlaceableBrowserWidget::ShowObjectListWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bObjectListWidgetVisible) {
		bObjectListWidgetVisible = true;
		if (ObjectListWidget) {
			// ObjectListWidget->ResetState();
			// ObjectListWidget->ResetAnimationState();
			ObjectListWidget->Show();
		}
	}
}

void UUIPlaceableBrowserWidget::ShowOpenSeaNftListWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bOpenSeaNftListWidgetVisible) {
		bOpenSeaNftListWidgetVisible = true;
		if (OpenSeaAssetListWidget) {
			OpenSeaAssetListWidget->Refresh();
			// OpenSeaAssetListWidget->ResetState();
			// OpenSeaAssetListWidget->ResetAnimationState();
			OpenSeaAssetListWidget->Show();
		}
	}
}

void UUIPlaceableBrowserWidget::ShowPlaceableUrlWidget(bool bAnimate, bool bPlaySound) {
	if (!bPlaceableUrlWidgetVisible) {
		bPlaceableUrlWidgetVisible = true;
		if (PlaceableUrlWidget) {
			// PlaceableUrlWidget->ResetState();
			// PlaceableUrlWidget->ResetAnimationState();
			PlaceableUrlWidget->Show();
		}
	}
}

void UUIPlaceableBrowserWidget::HideObjectListWidget(const bool bAnimate, const bool bPlaySound) {
	if (bObjectListWidgetVisible) {
		bObjectListWidgetVisible = false;
		if (ObjectListWidget) {
			ObjectListWidget->Hide();
		}
	}
}

void UUIPlaceableBrowserWidget::HideOpenSeaNftListWidget(const bool bAnimate, const bool bPlaySound) {
	if (bOpenSeaNftListWidgetVisible) {
		bOpenSeaNftListWidgetVisible = false;
		if (OpenSeaAssetListWidget) {
			OpenSeaAssetListWidget->Hide();
		}
	}
}

void UUIPlaceableBrowserWidget::HidePlaceableUrlWidget(bool bAnimate, bool bPlaySound) {
	if (bPlaceableUrlWidgetVisible) {
		bPlaceableUrlWidgetVisible = false;
		if (PlaceableUrlWidget) {
			PlaceableUrlWidget->Hide();
		}
	}
}

void UUIPlaceableBrowserWidget::NavigateToObjectList(const bool bAnimate, const bool bPlaySound) {
	ShowObjectListWidget();
	HideOpenSeaNftListWidget();
	HidePlaceableUrlWidget();
}

void UUIPlaceableBrowserWidget::NavigateToOpenSeaNftListWidget(const bool bAnimate, const bool bPlaySound) {
	HideObjectListWidget();
	ShowOpenSeaNftListWidget();
	HidePlaceableUrlWidget();

}

void UUIPlaceableBrowserWidget::NavigateToPlaceableUrlWidget(bool bAnimate, bool bPlaySound) {
	HideObjectListWidget();
	HideOpenSeaNftListWidget();
	ShowPlaceableUrlWidget();
}


#undef LOCTEXT_NAMESPACE
