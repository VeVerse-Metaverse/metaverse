// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIEditorOpenSeaAssetsPropertiesWidget.h"

#include "Api2OpenSeaSubsystem.h"
#include "ApiOpenSeaMetadataObject.h"
#include "OpenSeaAssetMetadataObject.h"
#include "VeApi.h"
#include "VeShared.h"
#include "VeApi2.h"
#include "Components/UICommonPaginationWidget.h"
#include "Components/CheckBox.h"
#include "Components/TileView.h"
#include "Components/UICommonFilterWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIEditorOpenSeaAssetsPropertiesWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(PaginationWidget)) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
		PaginationWidget->SetUnlimited(true);
	}

	RequestMetadata.Owner = TEXT("");
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;

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
		RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIEditorOpenSeaAssetsPropertiesWidget::NativeOnRefresh);
	}

	NativeOnRefresh();
}

void UUIEditorOpenSeaAssetsPropertiesWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UUIEditorOpenSeaAssetsPropertiesWidget::NativeOnRefresh() {
	MetadataRequest(RequestMetadata);
}

void UUIEditorOpenSeaAssetsPropertiesWidget::MetadataRequest(const FOpenSeaAssetBatchRequestMetadata& InRequestMetadata) {
	GET_MODULE_SUBSYSTEM(OpenSeaSubsystem, Api2, OpenSea);

	if (OpenSeaSubsystem) {
		// 1. Callback
		TSharedRef<FOnOpenSeaBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnOpenSeaBatchRequestCompleted{});

		// 2. Lambda
		CallbackPtr->BindWeakLambda(this, [=](const FApiOpenSeaAssetBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
			if (!bInSuccessful) {
				LogErrorF("failed to request opensea asset batch: %s", *InError);
			}

			OnMetadataResponse(InMetadata, bInSuccessful, InError);

		});

		// 3. Request
		OpenSeaSubsystem->GetAssets(InRequestMetadata, CallbackPtr);
	}
}

void UUIEditorOpenSeaAssetsPropertiesWidget::OnMetadataResponse(const FApiOpenSeaAssetBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (IsValid(PaginationWidget)) {
		PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
	}
	if (IsValid(ContainerWidget)) {
		ContainerWidget->ScrollToTop();
		ContainerWidget->ClearListItems();
		ContainerWidget->SetListItems(UApiOpenSeaMetadataObject::BatchToUObjectArray(InMetadata.Assets, this));
		UpdateSelection();
	}
}

void UUIEditorOpenSeaAssetsPropertiesWidget::SetEntityId(const FGuid& InEntityId) {
	SelectedItem = InEntityId;
	UpdateSelection();
}

FGuid UUIEditorOpenSeaAssetsPropertiesWidget::GetEntityId() const {
	if (ContainerWidget) {
		if (auto* MetadataObject = ContainerWidget->GetSelectedItem<UApiOpenSeaMetadataObject>()) {
			return MetadataObject->Metadata.Id;
		}
	}
	return FGuid();
}

FApiOpenSeaAssetMetadata UUIEditorOpenSeaAssetsPropertiesWidget::GetMetadata() const {
	if (ContainerWidget) {
		if (auto* MetadataObject = ContainerWidget->GetSelectedItem<UApiOpenSeaMetadataObject>()) {
			return MetadataObject->Metadata;
		}
	}
	return FApiOpenSeaAssetMetadata();
}

void UUIEditorOpenSeaAssetsPropertiesWidget::UpdateSelection() {
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

#undef LOCTEXT_NAMESPACE
