// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIEventListPageWidget.h"

#include "VeApi.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Components/TileView.h"
#include "ApiEventSubsystem.h"
#include "UIPageManagerWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "Components/UICommonFilterWidget.h"
#include "Components/UILoadingWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIEventListPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PaginationWidget) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
	}
	
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;

	SetIsProcessing(false);
}

void UUIEventListPageWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();
	
	if(!OnContainerItemClickedDelegateHandle.IsValid()) {
		if (IsValid(ContainerWidget)) {
			OnContainerItemClickedDelegateHandle = ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
				if (auto* MetadataObject = Cast<UApiEventMetadataObject>(InObject)) {
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
			OnRefreshButtonClickedDelegateHandle = RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIEventListPageWidget::Refresh);
		}
	}
}

void UUIEventListPageWidget::UnregisterCallbacks() {
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

void UUIEventListPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();
	
	MetadataRequest(RequestMetadata);
}

void UUIEventListPageWidget::MetadataRequest(const IApiBatchQueryRequestMetadata& InRequestMetadata) {
	GET_MODULE_SUBSYSTEM(EventSubsystem, Api, Event);
	if (EventSubsystem) {
		// 1. Create a callback.
		const auto Callback = MakeShared<FOnEventBatchRequestCompleted>();

		// 2. Bind callback.
		Callback->BindWeakLambda(this, [=](const FApiEventBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError)  {
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			SetIsProcessing(false);
			OnMetadataResponse(InMetadata, bInSuccessful, InError);
		});

		// 3. Make the request using the callback.
		if (EventSubsystem->GetBatch(InRequestMetadata, Callback)) {
			SetIsProcessing(true);
			return;
		}
		LogErrorF("failed to request");
	}
	
	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Header = LOCTEXT("Error", "Error");
		NotificationData.Message = LOCTEXT("FailedToLoadVirtualWorld", "Failed to load an event data");
		NotificationSubsystem->AddNotification(NotificationData);
	}
}

void UUIEventListPageWidget::OnMetadataResponse(const FApiEventBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(PaginationWidget)) {
			PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
		}
		if (IsValid(ContainerWidget)) {
			ContainerWidget->ScrollToTop();
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UApiEventMetadataObject::BatchToUObjectArray(InMetadata.Entities, this));
		}
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadEventBatchError", "Error");
			NotificationData.Message = FText::FromString("Failed to load an event batch.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIEventListPageWidget::SetIsProcessing(bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}


#undef LOCTEXT_NAMESPACE
