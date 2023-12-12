// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISharedAccessListPageWidget.h"

#include "Api2SharedAccessMetadataObject.h"
#include "VeApi.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Components/TileView.h"
#include "Components/UICommonPaginationWidget.h"
#include "Components/UICommonFilterWidget.h"
#include "Components/UILoadingWidget.h"
#include "VeApi2.h"

#define LOCTEXT_NAMESharedAccess "VeUI"


void UUISharedAccessListPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PaginationWidget) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
	}

	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;
	// RequestMetadata.Type = SharedAccessType;

	// if (IsValid(ContainerWidget)) {
	// 	ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
	// 		if (auto* MetadataObject = Cast<UApiSharedAccessMetadataObject>(InObject)) {
	// 			GetPageManager()->OpenPage(DetailPageClass, *MetadataObject->Metadata->EntityId.ToString());
	// 		}
	// 	});
	// }

	if (IsValid(PaginationWidget)) {
		PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
			RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
			Refresh();
		});
	}

	if (IsValid(FilterWidget)) {
		FilterWidget->OnFilterChanged.AddWeakLambda(this, [this](FString InQuery) {
			RequestMetadata.Query = InQuery;
			RequestMetadata.Offset = 0;
			if (PaginationWidget) {
				PaginationWidget->ResetPagination();
			}
			Refresh();
		});
	}

	if (IsValid(RefreshButtonWidget)) {
		RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISharedAccessListPageWidget::Refresh);
	}
	

	SetIsProcessing(false);
}

void UUISharedAccessListPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	MetadataRequest(RequestMetadata);
}

void UUISharedAccessListPageWidget::MetadataRequest(const FApi2SharedAccessBatchRequestMetadata& InRequestMetadata) {

	GET_MODULE_SUBSYSTEM(SharedAccessSubsystem, Api2, SharedAccess);
	if (SharedAccessSubsystem) {
		const auto Callback = MakeShared<FOnSharedAccessBatchRequestCompleted2>();
		Callback->BindWeakLambda(this, [this, Callback](const FApi2SharedAccessBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
			if (!bSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			SetIsProcessing(false);
			OnMetadataResponse(InMetadata, bSuccessful, InError);
		});
		
		RequestMetadata = InRequestMetadata;
		RequestMetadata.EntityId = FGuid(GetOptions());

		SetIsProcessing(true);
		SharedAccessSubsystem->Index(RequestMetadata, Callback);
	}
}

void UUISharedAccessListPageWidget::OnMetadataResponse(const FApi2SharedAccessBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
	if (bInSuccessful) {
		if (PaginationWidget) {
			PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
		}
		if (ContainerWidget) {
			ContainerWidget->ScrollToTop();
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UApi2SharedAccessMetadataObject::BatchToUObjectArray(InMetadata.Entities, this));
		}
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = NSLOCTEXT("VeUI", "LoadSharedAccessBatchError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a world batch.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUISharedAccessListPageWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}


#undef LOCTEXT_NAMESharedAccess
