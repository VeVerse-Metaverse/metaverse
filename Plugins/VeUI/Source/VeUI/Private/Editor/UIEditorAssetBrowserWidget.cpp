#include "Editor/UIEditorAssetBrowserWidget.h"

#include "Api2PlaceableMetadataObject.h"
#include "Api2PlaceableSubsystem.h"
#include "ApiPlaceableRequest.h"
#include "UINotificationData.h"
#include "Components/UICommonFilterWidget.h"
#include "VeUI.h"
#include "Components/UILoadingWidget.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Components/ComboBoxString.h"

void UUIEditorAssetBrowserWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PaginationWidget) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
	}

	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;

	SetIsProcessing(false);

	if (PaginationWidget) {
		PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
			RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
			NativeOnRefresh();
		});
	}

	if (FilterWidget) {
		FilterWidget->OnFilterChanged.AddWeakLambda(this, [this](FString InQuery) {
			RequestMetadata.Offset = 0;
			RequestMetadata.Query = InQuery;
			if (PaginationWidget) {
				PaginationWidget->ResetPagination();
			}
			NativeOnRefresh();
		});
	}
}

void UUIEditorAssetBrowserWidget::Refresh() {
	NativeOnRefresh();
}

void UUIEditorAssetBrowserWidget::NativeOnRefresh() {
	MetadataRequest(RequestMetadata);
}

void UUIEditorAssetBrowserWidget::MetadataRequest(const FApi2PlaceableClassBatchQueryRequestMetadata& InRequestMetadata) {
	GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api2, Placeable);
	if (PlaceableSubsystem) {
		TSharedPtr<FOnPlaceableClassBatchRequestCompleted2> CallbackPtr = MakeShareable(new FOnPlaceableClassBatchRequestCompleted2());
		CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApi2PlaceableClassBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			SetIsProcessing(false);
			OnMetadataResponse(InMetadata, bInSuccessful, InError);

			CallbackPtr.Reset();
		});

		PlaceableSubsystem->GetPlaceableClassBatch(InRequestMetadata, CallbackPtr);
	}
}

void UUIEditorAssetBrowserWidget::OnMetadataResponse(const FApi2PlaceableClassBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
	if (bInSuccessful) {
		if (PaginationWidget) {
			PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
		}
		if (ContainerWidget) {
			ContainerWidget->ScrollToTop();
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UApi2PlaceableClassMetadataObject::BatchToUObjectArray(InMetadata.Entities, this));
		}
		if (CategoryDropDown) {
			//CategoryDropDown->AddOption();
		}
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = NSLOCTEXT("VeUI", "LoadAssetBatchError", "Error");
			NotificationData.Message = FText::FromString("Failed to load assets.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIEditorAssetBrowserWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}
