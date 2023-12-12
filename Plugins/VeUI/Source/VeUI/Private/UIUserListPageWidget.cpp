// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIUserListPageWidget.h"

#include "VeApi.h"
#include "VeUI.h"
#include "Components/TileView.h"
#include "ApiUserSubsystem.h"
#include "UIPageManagerWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "Components/UICommonFilterWidget.h"
#include "Components/UILoadingWidget.h"
#include "Subsystems/UINotificationSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIUserListPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PaginationWidget) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
	}
	
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;

	SetIsProcessing(false);
}

void UUIUserListPageWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();
	
	if(!OnContainerItemClickedDelegateHandle.IsValid()) {
		if (IsValid(ContainerWidget)) {
			OnContainerItemClickedDelegateHandle = ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
				if (auto* MetadataObject = Cast<UApiUserMetadataObject>(InObject)) {
					GetPageManager()->OpenPage(DetailPageClass, *MetadataObject->Metadata->Id.ToString());
				}
			});
		}
	}

	if (!OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (IsValid(PaginationWidget)) {
			OnPaginationPageChangeDelegateHandle = PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
				RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
				Refresh();
			});
		}
	}

	if (!OnFilterChangeDelegateHandle.IsValid()) {
		if (IsValid(FilterWidget)) {
			OnFilterChangeDelegateHandle = FilterWidget->OnFilterChanged.AddWeakLambda(this, [this](FString InQuery) {
				RequestMetadata.Query = InQuery;
				RequestMetadata.Offset = 0;
				if (PaginationWidget) {
					PaginationWidget->ResetPagination();
				}
				Refresh();
			});
		}
	}

	if (!OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(RefreshButtonWidget)) {
			OnRefreshButtonClickedDelegateHandle = RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIUserListPageWidget::Refresh);
		}
	}
}

void UUIUserListPageWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();
	
	if (OnContainerItemClickedDelegateHandle.IsValid()) {
		if (IsValid(ContainerWidget)) {
			ContainerWidget->OnItemClicked().Remove(OnContainerItemClickedDelegateHandle);
		}
		OnContainerItemClickedDelegateHandle.Reset();
	}
	
	if (OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (IsValid(PaginationWidget)) {
			PaginationWidget->OnPageChanged.Remove(OnPaginationPageChangeDelegateHandle);
		}
		OnPaginationPageChangeDelegateHandle.Reset();
	}

	if (OnFilterChangeDelegateHandle.IsValid()) {
		if (IsValid(FilterWidget)) {
			FilterWidget->OnFilterChanged.Remove(OnFilterChangeDelegateHandle);
		}
		OnFilterChangeDelegateHandle.Reset();
	}

	if (OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(RefreshButtonWidget)) {
			RefreshButtonWidget->GetOnButtonClicked().Remove(OnRefreshButtonClickedDelegateHandle);
		}
		OnRefreshButtonClickedDelegateHandle.Reset();
	}
}

void UUIUserListPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();
	
	MetadataRequest(RequestMetadata);
}

void UUIUserListPageWidget::MetadataRequest(const IApiBatchQueryRequestMetadata& InRequestMetadata) {
	GET_MODULE_SUBSYSTEM(UserSubsystem, Api, User);
	if (UserSubsystem) {
		// 1. Create a callback.
		TSharedPtr<FOnUserBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnUserBatchRequestCompleted());

		// 2. Bind callback.
		CallbackPtr->BindWeakLambda(this, [=](const FApiUserBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			SetIsProcessing(false);
			OnMetadataResponse(InMetadata, bInSuccessful, InError);

			CallbackPtr.Reset();
		});

		// 3. Make the request using the callback.
		if (UserSubsystem->GetBatch(InRequestMetadata, CallbackPtr)) {
			SetIsProcessing(true);
		} else {
			LogErrorF("failed to request");
		}
	}
}

void UUIUserListPageWidget::OnMetadataResponse(const FApiUserBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(PaginationWidget)) {
			PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
		}
		if (IsValid(ContainerWidget)) {
			ContainerWidget->ScrollToTop();
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UApiUserMetadataObject::BatchToUObjectArray(InMetadata.Entities, this));
		}
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadUserBatchError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a user batch.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIUserListPageWidget::SetIsProcessing(bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}


#undef LOCTEXT_NAMESPACE
