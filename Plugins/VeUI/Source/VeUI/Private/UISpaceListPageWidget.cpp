// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISpaceListPageWidget.h"

#include "VeApi.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Components/TileView.h"
#include "ApiSpaceMetadataObject.h"
#include "Components/UICommonPaginationWidget.h"
#include "Components/UICommonFilterWidget.h"
#include "UIPageManagerWidget.h"
#include "Components/UILoadingWidget.h"
#include "UISpaceFormPageWidget.h"
#include "VeApi2.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUISpaceListPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PaginationWidget) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
	}

	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;

	if (IsValid(ContainerWidget)) {
		ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
			if (auto* MetadataObject = Cast<UApiSpaceMetadataObject>(InObject)) {
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
			RequestMetadata.Search = InQuery;
			RequestMetadata.Offset = 0;
			if (PaginationWidget) {
				PaginationWidget->ResetPagination();
			}
			Refresh();
		});
	}

	if (IsValid(RefreshButtonWidget)) {
		RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceListPageWidget::Refresh);
	}

	if (IsValid(SortContentButtonWidget)) {
		SortContentButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			// if (bSortContent) {
			// 	bSortContent = false;
			// } else {
			// 	bSortContent = true;
			// }
			SortValidation(ESortType::Content);
		});
	}

	if (IsValid(SortViewButtonWidget)) {
		SortViewButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			// if (bSortView) {
			// 	bSortView = false;
			// } else {
			// 	bSortView = true;
			// }
			SortValidation(ESortType::View);
		});
	}

	if (IsValid(SortLikeButtonWidget)) {
		SortLikeButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			// if (bSortLike) {
			// 	bSortLike = false;
			// } else {
			// 	bSortLike = true;
			// }
			SortValidation(ESortType::Like);
		});
	}

	if (IsValid(CreateButtonWidget)) {
		CreateButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			if (IsValid(FormPageClass)) {
				if (auto Page = GetPageManager()->OpenPage<UUISpaceFormPageWidget>(FormPageClass)) {
					Page->SetParentPage(this);
					Page->SetMetadata(FVeWorldMetadata());
				}
			}
		});
	}

	SetIsProcessing(false);
}

void UUISpaceListPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	MetadataRequest(RequestMetadata);
}

void UUISpaceListPageWidget::MetadataRequest(const FApi2BatchSortRequestMetadata& InRequestMetadata) {
	GET_MODULE_SUBSYSTEM(SpaceSubsystem, Api2, World);
	if (SpaceSubsystem) {
		const auto Callback = MakeShared<FOnWorldBatchRequestCompleted2>();
		Callback->BindWeakLambda(this, [this, Callback](const FApi2WorldBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
			if (!bSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			SetIsProcessing(false);
			OnMetadataResponse(InMetadata, bSuccessful, InError);
		});

		SetIsProcessing(true);
		SpaceSubsystem->IndexV2(InRequestMetadata, Callback);
	}
}

void UUISpaceListPageWidget::OnMetadataResponse(const FApi2WorldBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
	if (bInSuccessful) {
		if (PaginationWidget) {
			PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
		}
		if (ContainerWidget) {
			ContainerWidget->ScrollToTop();
			ContainerWidget->ClearListItems();
			ContainerWidget->SetListItems(UApiSpaceMetadataObject::BatchToUObjectArray(InMetadata.Entities, this));
		}
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = NSLOCTEXT("VeUI", "LoadSpaceBatchError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a world batch.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUISpaceListPageWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

void UUISpaceListPageWidget::SortValidation(const ESortType SortType) {
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;
	RequestMetadata.SortOptions.Empty();

	// if (bSortContent) {
	// 	SortContentButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
	// 	RequestMetadata.SortOptions.Add(EApiEntitySorts::PakFile);
	// } else {
	// 	SortContentButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
	// }
	//
	// if (bSortView) {
	// 	SortViewButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
	// 	RequestMetadata.SortOptions.Add(EApiEntitySorts::Views);
	// } else {
	// 	SortViewButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
	// }
	//
	// if (bSortLike) {
	// 	SortLikeButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
	// 	RequestMetadata.SortOptions.Add(EApiEntitySorts::Likes);
	// } else {
	// 	SortLikeButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
	// }


	if (SortContentButtonWidget) {
		SortContentButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
	}
	
	if (SortViewButtonWidget) {
		SortViewButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
	}
	
	if (SortLikeButtonWidget) {
		SortLikeButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
	}

	switch (SortType) {
	case ESortType::Content:
		if (SortContentButtonWidget) {
			SortContentButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
			RequestMetadata.SortOptions.Add(EApiEntitySorts::PakFile);
		}
		break;
	case ESortType::View:
		if (SortViewButtonWidget) {
			SortViewButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
			RequestMetadata.SortOptions.Add(EApiEntitySorts::Views);
		}
		break;
	case ESortType::Like:
		if (SortLikeButtonWidget) {
			SortLikeButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
			RequestMetadata.SortOptions.Add(EApiEntitySorts::Likes);
		}
		break;
	default:
		break;
	}
	
	Refresh();
}


#undef LOCTEXT_NAMESPACE
