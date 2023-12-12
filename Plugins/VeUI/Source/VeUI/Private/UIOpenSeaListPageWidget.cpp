// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIOpenSeaListPageWidget.h"

#include "ApiOpenSeaMetadataObject.h"
#include "UIOpenSeaDetailPageWidget.h"
#include "VeUI.h"
#include "VeShared.h"
#include "VeOpenSea/VeOpenSeaModule.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "UIPageManagerWidget.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2OpenSeaSubsystem.h"
#include "Components/UICommonPaginationWidget.h"
#include "Components/UILoadingWidget.h"
#include "Components/TileView.h"
#include "Components/UICommonFilterWidget.h"


#define LOCTEXT_NAMESPACE "VeUI"


void UUIOpenSeaListPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(PaginationWidget)) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
		PaginationWidget->SetUnlimited(true);
	}

	RequestMetadata.Owner = TEXT("");
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		CurrentUserEthAddress = AuthSubsystem->GetUserMetadata().EthAddress;
	}

	if (FilterWidget) {
		FilterWidget->OnFilterChanged.AddWeakLambda(this, [this](FString InQuery) {
			
			RequestMetadata.Query = InQuery;
			RequestMetadata.Offset = 0;
			if (PaginationWidget) {
				PaginationWidget->ResetPagination();
			}
			NativeOnRefresh();
		});
	}

	SetIsProcessing(false);
}

void UUIOpenSeaListPageWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

#pragma region Item selection callback

	if (!OnContainerItemClickDelegateHandle.IsValid()) {
		if (ContainerWidget) {
			OnContainerItemClickDelegateHandle = ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
				if (InObject) {
					if (const auto* MetadataObject = Cast<UApiOpenSeaMetadataObject>(InObject)) {

						// Check if we should override the page opening with object selected callback.
						if (OnObjectSelected.IsBound()) {
							OnObjectSelected.Broadcast(MetadataObject->Metadata);
						} else {
							auto* Page = GetPageManager()->OpenPage<UUIOpenSeaDetailPageWidget>(DetailPageClass, MetadataObject->Metadata.Id.ToString());
							Page->SetMetadata(MetadataObject->Metadata);
						}
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
	}

#pragma endregion Item selection callback

#pragma region Navigation and filter callbacks

	// if (FilterCheckBoxWidget) {
	// 	FilterCheckBoxWidget->OnCheckStateChanged.AddUniqueDynamic(this, &UUIOpenSeaListPageWidget::OnFilterCheckStateChangedCallback);
	// }

	if (!OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (IsValid(PaginationWidget)) {
			OnPaginationPageChangeDelegateHandle = PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
				RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
				Refresh();
			});
		}
	}

	if (!OnRefreshButtonClickDelegateHandle.IsValid()) {
		if (IsValid(RefreshButtonWidget)) {
			OnRefreshButtonClickDelegateHandle = RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIOpenSeaListPageWidget::Refresh);
		}
	}

#pragma endregion Navigation callbacks

}

void UUIOpenSeaListPageWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	if (OnContainerItemClickDelegateHandle.IsValid()) {
		if (IsValid(ContainerWidget)) {
			ContainerWidget->OnItemClicked().Remove(OnContainerItemClickDelegateHandle);
		}
		OnContainerItemClickDelegateHandle.Reset();
	}

	if (OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (IsValid(PaginationWidget)) {
			PaginationWidget->OnPageChanged.Remove(OnPaginationPageChangeDelegateHandle);
		}
		OnPaginationPageChangeDelegateHandle.Reset();
	}

	if (OnRefreshButtonClickDelegateHandle.IsValid()) {
		if (IsValid(RefreshButtonWidget)) {
			RefreshButtonWidget->GetOnButtonClicked().Remove(OnRefreshButtonClickDelegateHandle);
		}
		OnRefreshButtonClickDelegateHandle.Reset();
	}
}

void UUIOpenSeaListPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	RequestOpenSeaAssets(RequestMetadata);
}

void UUIOpenSeaListPageWidget::RequestOpenSeaAssets(const FOpenSeaAssetBatchRequestMetadata& InRequestMetadata) {
	GET_MODULE_SUBSYSTEM(OpenSeaSubsystem, Api2, OpenSea);

	if (OpenSeaSubsystem) {
		// 1. Callback
		TSharedRef<FOnOpenSeaBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnOpenSeaBatchRequestCompleted{});

		// 2. Lambda
		CallbackPtr->BindWeakLambda(this, [=](const FApiOpenSeaAssetBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
			if (!bInSuccessful) {
				LogErrorF("failed to request opensea asset batch: %s", *InError);
			}

			SetIsProcessing(false);

			OnOpenSeaAssetsReceived(InMetadata, bInSuccessful, InError);

		});

		// 3. Request
		OpenSeaSubsystem->GetAssets(InRequestMetadata, CallbackPtr);
	}
}

void UUIOpenSeaListPageWidget::OnOpenSeaAssetsReceived(const FApiOpenSeaAssetBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(ContainerWidget)) {
			ContainerWidget->ScrollToTop();
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UApiOpenSeaMetadataObject::BatchToUObjectArray(InMetadata.Assets, this));
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

void UUIOpenSeaListPageWidget::SetIsProcessing(const bool bInIsProcessing) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsProcessing) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

// void UUIOpenSeaListPageWidget::OnFilterCheckStateChangedCallback(bool bIsChecked) {
// 	RequestMetadata.Owner = bIsChecked ? CurrentUserEthAddress : TEXT("");
// 	RequestMetadata.Offset = 0;
// 	Refresh();
// }

#undef LOCTEXT_NAMESPACE
