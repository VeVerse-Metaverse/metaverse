// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIMetaverseListPageWidget.h"

#include "VeApi2.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Components/TileView.h"
#include "UIPageManagerWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "Components/UICommonFilterWidget.h"
#include "Components/UILoadingWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIMetaverseListPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PaginationWidget) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
	}
	
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;

	SetIsProcessing(false);

	if (IsValid(ContainerWidget)) {
		ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
			if (auto* MetadataObject = Cast<UApiPackageMetadataObject>(InObject)) {
				GetPageManager()->OpenPage(DetailPageClass, *MetadataObject->Metadata->Id.ToString());
			}
		});
	}

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
		RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIMetaverseListPageWidget::Refresh);
	}
}

void UUIMetaverseListPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();
	
	MetadataRequest(RequestMetadata);
}

void UUIMetaverseListPageWidget::MetadataRequest(const FApiPackageIndexRequestMetadata& InRequestMetadata) {
	GET_MODULE_SUBSYSTEM(PackageSubsystem, Api2, Package);
	if (PackageSubsystem) {
		const auto Callback = MakeShared<FOnPackageBatchRequestCompleted2>();
		Callback->BindWeakLambda(this, [=](const FApiPackageBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)  {
			const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
			if (!bSuccessful) {
				VeLogErrorFunc("Failed to request: %s", *InError);
			}

			SetIsProcessing(false);
			OnMetadataResponse(InMetadata, bSuccessful, InError);
		});

		SetIsProcessing(true);
		PackageSubsystem->Index(InRequestMetadata, Callback);
	}
}

void UUIMetaverseListPageWidget::OnMetadataResponse(const FApiPackageBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(PaginationWidget)) {
			PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
		}
		if (IsValid(ContainerWidget)) {
			ContainerWidget->ScrollToTop();
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UApiPackageMetadataObject::BatchToUObjectArray(InMetadata.Entities, this));
		}
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadMetaverseBatchError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a package batch.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIMetaverseListPageWidget::SetIsProcessing(bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}


#undef LOCTEXT_NAMESPACE
