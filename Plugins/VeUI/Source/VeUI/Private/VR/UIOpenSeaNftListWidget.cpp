// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIOpenSeaNftListWidget.h"

#include "OpenSeaAssetMetadataObject.h"
#include "UINotificationData.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "VeOpenSea/VeOpenSeaModule.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIOpenSeaNftListWidget::RegisterCallbacks() {

	if (!OnOpenSeaNftListItemClickedDelegateHandle.IsValid()) {
		OnOpenSeaNftSelected.AddWeakLambda(this, [&](TSharedPtr<FApiObjectMetadata> Metadata) { OnOpenSeaNftClicked(Metadata); });
		OnOpenSeaNftListItemClickedDelegateHandle = OnOpenSeaNftListItemClicked.AddWeakLambda(this, [this](UOpenSeaAssetMetadataObject* InObject) {
			if (InObject) {
				if (OnOpenSeaNftSelected.IsBound()) {
					TSharedPtr<FApiObjectMetadata> Metadata = MakeShareable(new FApiObjectMetadata);
					Metadata->Owner.EthAddress = InObject->Metadata.Owner.Address;
					Metadata->SourceUrl = InObject->Metadata.Permalink;
					Metadata->Source = InObject->Metadata.ImageUrl;
					Metadata->Name = InObject->Metadata.Name;
					FApiFileMetadata File;
					File.Url = InObject->Metadata.ImageUrl;
					Metadata->Files.Emplace(File);

					OnOpenSeaNftSelected.Broadcast(Metadata);
				}
			} else {
				GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
				if (NotificationSubsystem) {
					FUINotificationData NotificationData;
					NotificationData.Type = EUINotificationType::Failure;
					NotificationData.Header = NSLOCTEXT("VeUI", "OpenObjectError", "Error");
					NotificationData.Message = FText::FromString("Failed to load the Object.");
					NotificationSubsystem->AddNotification(NotificationData);
				}
			}
		});
	}

	// Deprecated
#if 0
	if (!OnAuthSubsystemUserLoginDelegateHandle.IsValid()) {
		if (const auto* ApiModule = FVeApi2Module::Get()) {
			if (const auto AuthSubsystem = ApiModule->GetAuthSubsystem()) {
				OnAuthSubsystemUserLoginDelegateHandle = AuthSubsystem->GetOnUserLogin().AddWeakLambda(
					this, [this](const FApiUserMetadata&, const bool bSuccessful, const FString&) {
						if (bSuccessful) {
							if (IsValid(PaginationWidget)) {
								FOpenSeaAssetBatchRequestMetadata RequestMetadata;
								RequestMetadata.Limit = PaginationWidget->ItemsPerPage;
								RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
								GetOpenSeaAssets(RequestMetadata);
							}
						}
					});
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	}
#endif

	if (!OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (IsValid(PaginationWidget)) {
			OnPaginationPageChangeDelegateHandle = PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
				FOpenSeaAssetBatchRequestMetadata RequestMetadata;
				RequestMetadata.Limit = InItemsPerPage;
				RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
				GetOpenSeaAssets(RequestMetadata);
			});
		}
	}

	if (!OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(RefreshButtonWidget)) {
			OnRefreshButtonClickedDelegateHandle = RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIOpenSeaNftListWidget::Refresh);
			// 	IApiBatchQueryRequestMetadata RequestMetadata;
			// 	RequestMetadata.Limit = ItemsPerPage;
			// 	RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
			// 	RequestMetadata.Query = CurrentQuery;
			// 	GetObjects(RequestMetadata);
			// });
		}
	}

	if (!OnOpenSeaNftQueryFilterChangeDelegateHandle.IsValid()) {
		if (IsValid(OpenSeaNftFilterWidget)) {
			OnOpenSeaNftQueryFilterChangeDelegateHandle = OpenSeaNftFilterWidget->OnFilterChanged.AddWeakLambda(this, [this](FString InQuery) {
				FOpenSeaAssetBatchRequestMetadata RequestMetadata;
				//RequestMetadata.Query = InQuery;
				RequestMetadata.Limit = ItemsPerPage;
				RequestMetadata.Offset = 0;

				if (IsValid(PaginationWidget)) {
					PaginationWidget->ResetPagination();
				}
				GetOpenSeaAssets(RequestMetadata);
			});
		}
	}

	if (!OnOpenSeaNftCreateButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CreateButtonWidget)) {
			OnOpenSeaNftCreateButtonClickedDelegateHandle = CreateButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIOpenSeaNftListWidget::OnObjectFormButtonClicked);
		}
	}
}

void UUIOpenSeaNftListWidget::UnregisterCallbacks() {
	if (OnOpenSeaNftListItemClickedDelegateHandle.IsValid()) {
		OnOpenSeaNftListItemClicked.Remove(OnOpenSeaNftListItemClickedDelegateHandle);
		OnOpenSeaNftListItemClickedDelegateHandle.Reset();
	}

	// Deprecated
#if 0
	if (OnAuthSubsystemUserLoginDelegateHandle.IsValid()) {
		if (const auto* ApiModule = FVeApi2Module::Get()) {
			if (const auto AuthSubsystem = ApiModule->GetAuthSubsystem()) {
				{
					AuthSubsystem->GetOnUserLogin().Remove(OnAuthSubsystemUserLoginDelegateHandle);
					OnAuthSubsystemUserLoginDelegateHandle.Reset();
				}
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	}
#endif

	if (OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (IsValid(PaginationWidget)) {
			PaginationWidget->OnPageChanged.Remove(OnPaginationPageChangeDelegateHandle);
			OnPaginationPageChangeDelegateHandle.Reset();
		}
	}

	if (OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(RefreshButtonWidget)) {
			RefreshButtonWidget->GetOnButtonClicked().Remove(OnRefreshButtonClickedDelegateHandle);
			OnRefreshButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnOpenSeaNftQueryFilterChangeDelegateHandle.IsValid()) {
		if (IsValid(OpenSeaNftFilterWidget)) {
			OpenSeaNftFilterWidget->OnFilterChanged.Remove(OnOpenSeaNftQueryFilterChangeDelegateHandle);
			OnOpenSeaNftQueryFilterChangeDelegateHandle.Reset();
		}
	}

	if (OnOpenSeaNftCreateButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CreateButtonWidget)) {
			CreateButtonWidget->GetOnButtonClicked().Remove(OnOpenSeaNftCreateButtonClickedDelegateHandle);
			OnOpenSeaNftCreateButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUIOpenSeaNftListWidget::OnAssetsLoaded(const FOpenSeaAssetBatchMetadata& InMetadata, bool bInSuccessful, const FString& String) {
	if (bInSuccessful) {
		//if (IsValid(PaginationWidget)) { PaginationWidget->SetPaginationOptions(Metadata.Total, ItemsPerPage); }
		if (IsValid(ContainerWidget)) { ContainerWidget->ScrollToTop(); }
		if (IsValid(ContainerWidget)) {
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UOpenSeaAssetMetadataObject::BatchToUObjectArray(InMetadata.Assets, this));
		}
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadObjectBatchError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a object batch.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIOpenSeaNftListWidget::GetOpenSeaAssets(const FOpenSeaAssetBatchRequestMetadata& InRequestMetadata) {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
			if (AuthSubsystem->GetUserEthAddress().IsEmpty()) {
				LogWarningF("user has no eth address");
				//return OnGetEthAddressFailed.Broadcast();
			}

			if (FVeOpenSeaModule* OpenSeaModule = FVeOpenSeaModule::Get()) {
				if (const TSharedPtr<FOpenSeaAssetSubsystem> AssetSubsystem = OpenSeaModule->GetAssetSubsystem()) {
					FOpenSeaAssetBatchRequestMetadata RequestMetadata{};
					// RequestMetadata.Owner = "0x92aA6ab29370215a0C85B757d27CC728584c86F1"; //AuthSubsystem->GetUserEthAddress();
					RequestMetadata.Owner = AuthSubsystem->GetUserEthAddress();
					// 1. Create a callback.
					TSharedPtr<FOnOpenSeaAssetBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnOpenSeaAssetBatchRequestCompleted());

					// 2. Bind the callback.
					CallbackPtr->BindWeakLambda(this, [=](const FOpenSeaAssetBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
						if (!bInSuccessful) {
							LogErrorF("request failed: %s", *InError);
						}

						OnAssetsLoaded(InMetadata, bInSuccessful, InError);

						CallbackPtr.Reset();
					});

					// 3. Request with callback.
					if (!AssetSubsystem->GetBatch(RequestMetadata, CallbackPtr)) {
						LogErrorF("failed to request");
					}
				} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(OpenSea), STRINGIFY(Assets)); }
			} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(OpenSea)); }
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
}

void UUIOpenSeaNftListWidget::OnObjectFormButtonClicked() const {
	if (OnObjectCreateButtonClicked.IsBound()) {
		OnObjectCreateButtonClicked.Broadcast();
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = NSLOCTEXT("VeUI", "OpenObjectError", "Error");
			NotificationData.Message = FText::FromString("Failed to load the Object.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIOpenSeaNftListWidget::OnOpenSeaNftClicked(TSharedPtr<FApiObjectMetadata> Metadata) {
	if (DetailWidget) {
		DetailWidget->SetMetadata(Metadata);
		ContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
		DetailWidget->SetVisibility(ESlateVisibility::Visible);
		DetailWidget->Show();
		DetailWidget->OnClosed.AddWeakLambda(this, [&]() {
			DetailWidget->Hide();
			DetailWidget->SetVisibility(ESlateVisibility::Collapsed);
			DetailWidget->OnClosed.RemoveAll(this);
			ContainerWidget->SetVisibility(ESlateVisibility::Visible);
		});
	}
}

void UUIOpenSeaNftListWidget::NativeConstruct() {
	if (IsValid(ContainerWidget)) {
		ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
			if (OnOpenSeaNftListItemClicked.IsBound()) {
				if (UOpenSeaAssetMetadataObject* Asset = Cast<UOpenSeaAssetMetadataObject>(InObject)) {
					OnOpenSeaNftListItemClicked.Broadcast(Asset);
				}
			}
		});
	}

	SetIsProcessing(false);
	Super::NativeConstruct();
}

void UUIOpenSeaNftListWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

void UUIOpenSeaNftListWidget::Refresh() {
	FOpenSeaAssetBatchRequestMetadata RequestMetadata;
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
	//RequestMetadata.Query = CurrentQuery;
	GetOpenSeaAssets(RequestMetadata);
}

#undef LOCTEXT_NAMESPACE
