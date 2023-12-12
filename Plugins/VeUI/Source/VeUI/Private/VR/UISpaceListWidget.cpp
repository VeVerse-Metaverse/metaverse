// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UISpaceListWidget.h"

#include "ApiSpaceMetadataObject.h"
#include "ApiSpaceSubsystem.h"
#include "UINotificationData.h"
// #include "VeApi.h"
#include "VeApi2.h"


void UUISpaceListWidget::NativeConstruct() {
	if (ContainerWidget) {
		ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
			if (OnSpaceListItemClicked.IsBound()) {
				if (UApiSpaceMetadataObject* Space = Cast<UApiSpaceMetadataObject>(InObject)) {
					OnSpaceListItemClicked.Broadcast(Space);
				}
			}
		});
	}

	SetIsProcessing(false);

	Super::NativeConstruct();
}

void UUISpaceListWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (LoadingWidget) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

void UUISpaceListWidget::Refresh() {
	FApiWorldIndexRequestMetadata RequestMetadata;
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
	RequestMetadata.Query = CurrentQuery;
	// RequestMetadata.Type = SpaceType;
	GetSpaces(RequestMetadata);
}

void UUISpaceListWidget::GetSpaces(const FApiWorldIndexRequestMetadata& RequestMetadata) {
	GET_MODULE_SUBSYSTEM(SpaceSubsystem, Api2, World);
	if (SpaceSubsystem) {
		const auto CallbackPtr = MakeShared<FOnWorldBatchRequestCompleted2>();
		CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApi2WorldBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
			if (!bSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// Finish processing.
			SetIsProcessing(false);

			OnSpacesLoaded(InMetadata, bSuccessful, InError);
		});

		SetIsProcessing(true);
		SpaceSubsystem->Index(RequestMetadata, CallbackPtr);
	}
}

void UUISpaceListWidget::SetSpaces(const FApi2WorldBatchMetadata& BatchMetadata) {
	if (ContainerWidget) {
		ContainerWidget->ClearListItems();
		ContainerWidget->SetListItems(UApiSpaceMetadataObject::BatchToUObjectArray(BatchMetadata.Entities, this));
	}

	if (PaginationWidget) {
		PaginationWidget->SetPaginationOptions(BatchMetadata.Total, ItemsPerPage);
	}
}

void UUISpaceListWidget::OnUserLogin(const FApiUserMetadata&, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (PaginationWidget) {
			FApiWorldIndexRequestMetadata RequestMetadata;
			RequestMetadata.Limit = PaginationWidget->ItemsPerPage;
			RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
			RequestMetadata.Query = CurrentQuery;
			// RequestMetadata.Type = SpaceType;
			GetSpaces(RequestMetadata);
		}
	}
}

void UUISpaceListWidget::OnSpaceItemClicked(UApiSpaceMetadataObject* InSpace) {
	if (InSpace) {
		if (OnSpaceSelected.IsBound()) {
			OnSpaceSelected.Broadcast(InSpace->Metadata);
		}
	} else {
		FUINotificationData NotificationData;
		NotificationData.Header = NSLOCTEXT("VeUI", "OpenSpaceError", "Error");
		NotificationData.Lifetime = 3.f;
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = FText::FromString("Failed to load the Space.");
		AddNotification(NotificationData);
	}
}

void UUISpaceListWidget::OnSpaceFormButtonClicked() {
	if (OnSpaceCreateButtonClicked.IsBound()) {
		OnSpaceCreateButtonClicked.Broadcast();
	} else {
		FUINotificationData NotificationData;
		NotificationData.Header = NSLOCTEXT("VeUI", "OpenSpaceError", "Error");
		NotificationData.Lifetime = 3.f;
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = FText::FromString("Failed to load the Space.");
		AddNotification(NotificationData);
	}
}

void UUISpaceListWidget::OnSpacesLoaded(const FApi2WorldBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
	if (bInSuccessful) {
		if (PaginationWidget) {
			PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
		}

		if (ContainerWidget) {
			ContainerWidget->ScrollToTop();
		}

		SetSpaces(InMetadata);
	} else {
		FUINotificationData NotificationData;
		NotificationData.Header = NSLOCTEXT("VeUI", "LoadSpaceBatchError", "Error");
		NotificationData.Lifetime = 3.f;
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = FText::FromString("Failed to load a space batch.");
		AddNotification(NotificationData);
	}
}

void UUISpaceListWidget::OnPageChanged(const int32 InCurrentPage, const int32 InItemsPerPage) {
	FApiWorldIndexRequestMetadata RequestMetadata;
	RequestMetadata.Limit = InItemsPerPage;
	RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
	RequestMetadata.Query = CurrentQuery;
	// RequestMetadata.Type = "artheon";
	GetSpaces(RequestMetadata);
}

void UUISpaceListWidget::OnFilterChanged(const FString& InQuery) {
	FApiWorldIndexRequestMetadata RequestMetadata;
	RequestMetadata.Query = InQuery;
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;
	// RequestMetadata.Type = "artheon";
	CurrentQuery = InQuery;
	if (PaginationWidget) {
		PaginationWidget->ResetPagination();
	}
	GetSpaces(RequestMetadata);
}

void UUISpaceListWidget::RegisterCallbacks() {
	// if (!OnSpaceBatchLoadedDelegateHandle.IsValid()) {
	// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 		if (const TSharedPtr<FApiSpaceSubsystem> SpaceSubsystem = ApiModule->GetSpaceSubsystem()) {
	// 			{
	// 				OnSpaceBatchLoadedDelegateHandle = SpaceSubsystem->GetOnBatchLoaded().AddUObject(this, &UUISpaceListWidget::OnBatchLoaded);
	// 			}
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Space)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	// }

	if (!OnSpaceListItemClickedDelegateHandle.IsValid()) {
		OnSpaceListItemClickedDelegateHandle = OnSpaceListItemClicked.AddUObject(this, &UUISpaceListWidget::OnSpaceItemClicked);
	}

	// Deprecated
#if 0
	if (!OnAuthSubsystemUserLoginDelegateHandle.IsValid()) {
		if (const auto* ApiModule = FVeApi2Module::Get()) {
			if (const auto AuthSubsystem = ApiModule->GetAuthSubsystem()) {
				{
					OnAuthSubsystemUserLoginDelegateHandle = AuthSubsystem->GetOnUserLogin().AddUObject(this, &UUISpaceListWidget::OnUserLogin);
				}
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	}
#endif

	if (!OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (PaginationWidget) {
			OnPaginationPageChangeDelegateHandle = PaginationWidget->OnPageChanged.AddUObject(this, &UUISpaceListWidget::OnPageChanged);
		}
	}

	if (!OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (RefreshButtonWidget) {
			OnRefreshButtonClickedDelegateHandle = RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceListWidget::Refresh);
		}
	}
	
	if (!OnSpaceNameFilterChangeDelegateHandle.IsValid()) {
		if (SpaceNameFilterWidget) {
			OnSpaceNameFilterChangeDelegateHandle = SpaceNameFilterWidget->OnFilterChanged.AddUObject(this, &UUISpaceListWidget::OnFilterChanged);
		}
	}

	if (!OnSpaceCreateButtonClickedDelegateHandle.IsValid()) {
		if (CreateButtonClickedWidget) {
			OnSpaceCreateButtonClickedDelegateHandle = CreateButtonClickedWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceListWidget::OnSpaceFormButtonClicked);
		}
	}
}

void UUISpaceListWidget::UnregisterCallbacks() {
	// if (OnSpaceBatchLoadedDelegateHandle.IsValid()) {
	// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 		if (const TSharedPtr<FApiSpaceSubsystem> SpaceSubsystem = ApiModule->GetSpaceSubsystem()) {
	// 			{
	// 				SpaceSubsystem->GetOnBatchLoaded().Remove(OnSpaceBatchLoadedDelegateHandle);
	// 				OnSpaceBatchLoadedDelegateHandle.Reset();
	// 			}
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Space)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	// }

	if (OnSpaceListItemClickedDelegateHandle.IsValid()) {
		OnSpaceListItemClicked.Remove(OnSpaceListItemClickedDelegateHandle);
		OnSpaceListItemClickedDelegateHandle.Reset();
	}

	// Deprecated
#if 0
	if (OnAuthSubsystemUserLoginDelegateHandle.IsValid()) {
		if (const auto* ApiModule = FVeApi2Module::Get()) {
			if (const auto AuthSubsystem = ApiModule->GetAuthSubsystem()) {
				{
					AuthSubsystem->GetOnUserLogin().Remove(OnAuthSubsystemUserLoginDelegateHandle);
					OnAuthSubsystemUserLoginDelegateHandle.Reset();
				}
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	}
#endif

	if (OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (PaginationWidget) {
			PaginationWidget->OnPageChanged.Remove(OnPaginationPageChangeDelegateHandle);
			OnPaginationPageChangeDelegateHandle.Reset();
		}
	}

	if (OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (RefreshButtonWidget) {
			RefreshButtonWidget->GetOnButtonClicked().Remove(OnRefreshButtonClickedDelegateHandle);
			OnRefreshButtonClickedDelegateHandle.Reset();
		}
	}
	
	if (OnSpaceNameFilterChangeDelegateHandle.IsValid()) {
		if (SpaceNameFilterWidget) {
			SpaceNameFilterWidget->OnFilterChanged.Remove(OnSpaceNameFilterChangeDelegateHandle);
			OnSpaceNameFilterChangeDelegateHandle.Reset();
		}
	}

	if (OnSpaceCreateButtonClickedDelegateHandle.IsValid()) {
		if (CreateButtonClickedWidget) {
			CreateButtonClickedWidget->GetOnButtonClicked().Remove(OnSpaceCreateButtonClickedDelegateHandle);
			OnSpaceCreateButtonClickedDelegateHandle.Reset();
		}
	}
}
