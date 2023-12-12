// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIObjectListPageWidget.h"

#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Components/TileView.h"
#include "Components/UICommonFilterWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "UIPageManagerWidget.h"
#include "ApiObjectSubsystem.h"
#include "Components/UILoadingWidget.h"
#include "UIObjectFormPageWidget.h"
#include "VeApi2.h"
#include "VeShared.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIObjectListPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PaginationWidget) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
	}

	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;

	SetIsProcessing(false);

	if (IsValid(ContainerWidget)) {
		ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
			if (auto* MetadataObject = Cast<UApiObjectMetadataObject>(InObject)) {
				//GetPageManager()->OpenPage(DetailPageClass, *MetadataObject->Metadata->Id.ToString());

				// Check if we should override the page opening with object selected callback.
				if (OnObjectSelected.IsBound()) {
					OnObjectSelected.Broadcast(MetadataObject->Metadata);
				} else {
					GetPageManager()->OpenPage(DetailPageClass, *MetadataObject->Metadata->Id.ToString());
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
		RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectListPageWidget::Refresh);
	}

	if (IsValid(CreateButtonWidget)) {
		CreateButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(FormPageClass)) {
				if (auto Page = GetPageManager()->OpenPage<UUIObjectFormPageWidget>(FormPageClass)) {
					Page->SetParentPage(this);
					Page->SetMetadata(FApiObjectMetadata());
				}
			}
		});
	}
}

void UUIObjectListPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	MetadataRequest(RequestMetadata);
}

void UUIObjectListPageWidget::MetadataRequest(const IApiBatchQueryRequestMetadata& InRequestMetadata) {
	GET_MODULE_SUBSYSTEM(ObjectSubsystem, Api2, Object);
	if (ObjectSubsystem) {
		const auto Callback = MakeShared<FOnObjectBatchRequestCompleted2>();
		Callback->BindWeakLambda(this, [this](const FApiObjectBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			if (InResponseCode != EApi2ResponseCode::Ok) {
				VeLogErrorFunc("Failed to request: %s", *InError);
			}

			SetIsProcessing(false);
			OnMetadataResponse(InMetadata, InResponseCode, InError);
		});

		SetIsProcessing(true);
		ObjectSubsystem->Index(InRequestMetadata, Callback);
	}
}

void UUIObjectListPageWidget::OnMetadataResponse(const FApiObjectBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& Error) {
	if (InResponseCode == EApi2ResponseCode::Ok) {
		if (IsValid(PaginationWidget)) {
			PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
		}
		if (IsValid(ContainerWidget)) {
			ContainerWidget->ScrollToTop();
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UApiObjectMetadataObject::BatchToUObjectArray(InMetadata.Entities, this));
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

void UUIObjectListPageWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}


#undef LOCTEXT_NAMESPACE
