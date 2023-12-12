// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIPlaceableArtObjectPropertiesWidget.h"

#include "Api2ObjectSubsystem.h"
#include "VeShared.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "VeGameModule.h"
#include "Components/TileView.h"
#include "Components/UICommonFilterWidget.h"
#include "Components/UICommonPaginationWidget.h"


void UUIPlaceableArtObjectPropertiesWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PaginationWidget) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
	}

	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;

	// SetIsProcessing(false);

	if (PaginationWidget) {
		PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
			RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
			NativeOnRefresh();
		});
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

	if (RefreshButtonWidget) {
		RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIPlaceableArtObjectPropertiesWidget::NativeOnRefresh);
	}

	NativeOnRefresh();
}

void UUIPlaceableArtObjectPropertiesWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UUIPlaceableArtObjectPropertiesWidget::NativeOnRefresh() {
	MetadataRequest(RequestMetadata);
}

void UUIPlaceableArtObjectPropertiesWidget::MetadataRequest(const IApiBatchQueryRequestMetadata& InRequestMetadata) {
	GET_MODULE_SUBSYSTEM(ObjectSubsystem, Api2, Object);
	if (ObjectSubsystem) {
		const auto CallbackPtr = MakeShared<FOnObjectBatchRequestCompleted2>();
		CallbackPtr->BindWeakLambda(this, [this](const FApiObjectBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			// SetIsProcessing(false);

			if (InResponseCode != EApi2ResponseCode::Ok) {
				LogErrorF("failed to request: %s", *InError);
				return;
			}

			OnMetadataResponse(InMetadata, InResponseCode == EApi2ResponseCode::Ok, InError);
		});

		ObjectSubsystem->Index(InRequestMetadata, CallbackPtr);		
	}
}

void UUIPlaceableArtObjectPropertiesWidget::OnMetadataResponse(const FApiObjectBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (IsValid(PaginationWidget)) {
		PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
	}
	if (IsValid(ContainerWidget)) {
		ContainerWidget->ScrollToTop();
		ContainerWidget->ClearListItems();
		ContainerWidget->SetListItems(UApiObjectMetadataObject::BatchToUObjectArray(InMetadata.Entities, this));
		UpdateSelection();
	}
}

void UUIPlaceableArtObjectPropertiesWidget::SetEntityId(const FGuid& InEntityId) {
	SelectedItem = InEntityId;
	UpdateSelection();
}

FGuid UUIPlaceableArtObjectPropertiesWidget::GetEntityId() const {
	if (ContainerWidget) {
		if (auto* MetadataObject = ContainerWidget->GetSelectedItem<UApiObjectMetadataObject>()) {
			return MetadataObject->Metadata->Id;
		}
	}
	return FGuid();
}

FApiObjectMetadata UUIPlaceableArtObjectPropertiesWidget::GetMetadata() const {
	if (ContainerWidget) {
		if (auto* MetadataObject = ContainerWidget->GetSelectedItem<UApiObjectMetadataObject>()) {
			return *MetadataObject->Metadata;
		}
	}
	return FApiObjectMetadata();
}

void UUIPlaceableArtObjectPropertiesWidget::UpdateSelection() {
	if (!ContainerWidget) {
		return;
	}

	for (auto* Item : ContainerWidget->GetListItems()) {
		if (auto* MetadataObject = Cast<UApiObjectMetadataObject>(Item)) {
			if (MetadataObject->Metadata->Id == SelectedItem) {
				ContainerWidget->SetItemSelection(Item, true);
				break;
			}
		}
	}
}
