// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIOpenSeaAssetListWidget.h"

#include "UINotificationData.h"
#include "VeApi.h"
#include "VeOpenSea/VeOpenSeaModule.h"
#include "Components/UICommonPaginationWidget.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"


#define LOCTEXT_NAMESPACE "VeUI"

void UUIOpenSeaAssetListWidget::RegisterCallbacks() {
	if (IsValid(PaginationWidget)) {
		PaginationWidget->SetUnlimited(true);
	}

	if (!OnObjectListItemClickedDelegateHandle.IsValid()) {
		OnObjectListItemClickedDelegateHandle = OnObjectListItemClicked.AddWeakLambda(this, [this](UOpenSeaAssetMetadataObject* InObject) {
			if (InObject) {
				if (OnObjectSelected.IsBound()) {
					OnObjectSelected.Broadcast(InObject->Metadata);
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

	if (!OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (IsValid(PaginationWidget)) {
			OnPaginationPageChangeDelegateHandle = PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
				if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
					if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
						if (AuthSubsystem->GetUserEthAddress().IsEmpty()) {
							LogWarningF("user did not bind wallet");
						} else {
							FOpenSeaAssetBatchRequestMetadata RequestMetadata;
							RequestMetadata.Limit = InItemsPerPage;
							RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
							RequestMetadata.Owner = AuthSubsystem->GetUserEthAddress();

							RequestOpenSeaAssets(RequestMetadata);
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
			});
		}
	}

	if (!OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(RefreshButtonWidget)) {
			OnRefreshButtonClickedDelegateHandle = RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIOpenSeaAssetListWidget::Refresh);
			// 	IApiBatchQueryRequestMetadata RequestMetadata;
			// 	RequestMetadata.Limit = ItemsPerPage;
			// 	RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
			// 	RequestMetadata.Query = CurrentQuery;
			// 	GetObjects(RequestMetadata);
			// });
		}
	}

	if (!OnObjectCreateButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CreateButtonWidget)) {
			OnObjectCreateButtonClickedDelegateHandle = CreateButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIOpenSeaAssetListWidget::OnObjectFormButtonClicked);
		}
	}
}

void UUIOpenSeaAssetListWidget::UnregisterCallbacks() {
	if (OnObjectListItemClickedDelegateHandle.IsValid()) {
		OnObjectListItemClicked.Remove(OnObjectListItemClickedDelegateHandle);
		OnObjectListItemClickedDelegateHandle.Reset();
	}

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

	if (OnObjectQueryFilterChangeDelegateHandle.IsValid()) {
		if (IsValid(ObjectQueryFilterWidget)) {
			ObjectQueryFilterWidget->OnFilterChanged.Remove(OnObjectQueryFilterChangeDelegateHandle);
			OnObjectQueryFilterChangeDelegateHandle.Reset();
		}
	}

	if (OnObjectCreateButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CreateButtonWidget)) {
			CreateButtonWidget->GetOnButtonClicked().Remove(OnObjectCreateButtonClickedDelegateHandle);
			OnObjectCreateButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUIOpenSeaAssetListWidget::RequestOpenSeaAssets(const FOpenSeaAssetBatchRequestMetadata& InRequestMetadata) {
	if (FVeOpenSeaModule* OpenSeaModule = FVeOpenSeaModule::Get()) {
		if (const TSharedPtr<FOpenSeaAssetSubsystem> AssetSubsystem = OpenSeaModule->GetAssetSubsystem()) {
			// 1. Callback
			TSharedPtr<FOnOpenSeaAssetBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnOpenSeaAssetBatchRequestCompleted{});

			// 2. Lambda
			CallbackPtr->BindWeakLambda(this, [=](const FOpenSeaAssetBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
				if (!bInSuccessful) {
					LogErrorF("failed to request opensea asset batch: %s", *InError);
				}

				OnOpenSeaAssetsRequestComplete(InMetadata, bInSuccessful, InError);

				CallbackPtr.Reset();
			});

			// 3. Request
			if (!AssetSubsystem->GetBatch(InRequestMetadata, CallbackPtr)) {
				LogErrorF("failed to request");
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(OpenSea), STRINGIFY(Asset)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(OpenSea)); }
}

void UUIOpenSeaAssetListWidget::OnOpenSeaAssetsRequestComplete(const FOpenSeaAssetBatchMetadata& InMetadata, bool bInSuccessful, const FString& InError) {
	if (bInSuccessful) {
		if (IsValid(ContainerWidget)) {
			ContainerWidget->ScrollToTop();
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UOpenSeaAssetMetadataObject::BatchToUObjectArray(InMetadata.Assets, this));
		}
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadOpenSeaAssetBatchError", "Error");
			NotificationData.Message = FText::FromString("Failed to load OpenSea assets.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIOpenSeaAssetListWidget::OnObjectFormButtonClicked() const {
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

void UUIOpenSeaAssetListWidget::NativeConstruct() {
	if (IsValid(ContainerWidget)) {
		ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
			if (OnObjectListItemClicked.IsBound()) {
				if (UOpenSeaAssetMetadataObject* Object = Cast<UOpenSeaAssetMetadataObject>(InObject)) {
					OnObjectListItemClicked.Broadcast(Object);
				}
			}
		});
	}

	SetIsProcessing(false);
	Super::NativeConstruct();
}

void UUIOpenSeaAssetListWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

void UUIOpenSeaAssetListWidget::Refresh() {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
			if (AuthSubsystem->GetUserEthAddress().IsEmpty()) {
				LogWarningF("user did not bind wallet");
				if (OnUserEthAddressEmpty.IsBound()) {
					OnUserEthAddressEmpty.Broadcast();
				}
			} else {
				FOpenSeaAssetBatchRequestMetadata RequestMetadata;
				RequestMetadata.Limit = ItemsPerPage;
				RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
				RequestMetadata.Owner = AuthSubsystem->GetUserEthAddress();

				RequestOpenSeaAssets(RequestMetadata);
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
}

void UUIOpenSeaAssetListWidget::SetObjects(const FApiObjectBatchMetadata& BatchMetadata) {
	if (IsValid(ContainerWidget)) {
		ContainerWidget->ClearListItems();
		ContainerWidget->SetListItems(UApiObjectMetadataObject::BatchToUObjectArray(BatchMetadata.Entities, this));
	}

	if (IsValid(PaginationWidget)) {
		PaginationWidget->SetPaginationOptions(BatchMetadata.Total, ItemsPerPage);
	}
}

void UUIOpenSeaAssetListWidget::OnObjectsLoaded(const FApiObjectBatchMetadata& Metadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(PaginationWidget)) { PaginationWidget->SetPaginationOptions(Metadata.Total, ItemsPerPage); }
		if (IsValid(ContainerWidget)) { ContainerWidget->ScrollToTop(); }
		SetObjects(Metadata);
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

#undef LOCTEXT_NAMESPACE
