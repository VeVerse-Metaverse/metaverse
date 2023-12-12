// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIObjectListWidget.h"

#include "ApiObjectSubsystem.h"
#include "UINotificationData.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIObjectListWidget::RegisterCallbacks() {
	if (!OnObjectListItemClickedDelegateHandle.IsValid()) {
		OnObjectListItemClickedDelegateHandle = OnObjectListItemClicked.AddWeakLambda(this, [this](UApiObjectMetadataObject* InObject) {
			if (InObject) {
				if (OnObjectSelected.IsBound()) {
					OnObjectSelected.Broadcast(InObject->Metadata);
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

	// Deprecated
#if 0
	if (!OnAuthSubsystemUserLoginDelegateHandle.IsValid()) {
		if (const auto* ApiModule = FVeApi2Module::Get()) {
			if (const auto AuthSubsystem = ApiModule->GetAuthSubsystem()) {
				{
					OnAuthSubsystemUserLoginDelegateHandle = AuthSubsystem->GetOnUserLogin().AddWeakLambda(
						this, [this](const FApiUserMetadata&, const bool bSuccessful, const FString&) {
							if (bSuccessful) {
								if (IsValid(PaginationWidget)) {
									IApiBatchQueryRequestMetadata RequestMetadata;
									RequestMetadata.Limit = PaginationWidget->ItemsPerPage;
									RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
									RequestMetadata.Query = CurrentQuery;
									GetObjects(RequestMetadata);
								}
							}
						});
				}
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	}
#endif

	if (!OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (IsValid(PaginationWidget)) {
			OnPaginationPageChangeDelegateHandle = PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
				IApiBatchQueryRequestMetadata RequestMetadata;
				RequestMetadata.Limit = InItemsPerPage;
				RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
				RequestMetadata.Query = CurrentQuery;
				GetObjects(RequestMetadata);
			});
		}
	}

	if (!OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(RefreshButtonWidget)) {
			OnRefreshButtonClickedDelegateHandle = RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectListWidget::Refresh);
			// 	IApiBatchQueryRequestMetadata RequestMetadata;
			// 	RequestMetadata.Limit = ItemsPerPage;
			// 	RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
			// 	RequestMetadata.Query = CurrentQuery;
			// 	GetObjects(RequestMetadata);
			// });
		}
	}

	if (!OnObjectQueryFilterChangeDelegateHandle.IsValid()) {
		if (IsValid(ObjectQueryFilterWidget)) {
			OnObjectQueryFilterChangeDelegateHandle = ObjectQueryFilterWidget->OnFilterChanged.AddWeakLambda(this, [this](FString InQuery) {
				IApiBatchQueryRequestMetadata RequestMetadata;
				RequestMetadata.Query = InQuery;
				RequestMetadata.Limit = ItemsPerPage;
				RequestMetadata.Offset = 0;
				CurrentQuery = InQuery;
				if (IsValid(PaginationWidget)) {
					PaginationWidget->ResetPagination();
				}
				GetObjects(RequestMetadata);
			});
		}
	}

	if (!OnObjectCreateButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CreateButtonWidget)) {
			OnObjectCreateButtonClickedDelegateHandle = CreateButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectListWidget::OnObjectFormButtonClicked);
		}
	}
}

void UUIObjectListWidget::UnregisterCallbacks() {
	if (OnObjectListItemClickedDelegateHandle.IsValid()) {
		OnObjectListItemClicked.Remove(OnObjectListItemClickedDelegateHandle);
		OnObjectListItemClickedDelegateHandle.Reset();
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
		if (IsValid(PaginationWidget)) {
			PaginationWidget->OnPageChanged.Remove(OnPaginationPageChangeDelegateHandle);
			OnPaginationPageChangeDelegateHandle.Reset();
		}
	}

	if (OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(RefreshButtonWidget)) {
			RefreshButtonWidget->GetOnButtonClicked().Remove(OnRefreshButtonClickedDelegateHandle);
			OnRefreshButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnObjectQueryFilterChangeDelegateHandle.IsValid()) {
		if (IsValid(ObjectQueryFilterWidget)) {
			ObjectQueryFilterWidget->OnFilterChanged.Remove(OnObjectQueryFilterChangeDelegateHandle);
			OnObjectQueryFilterChangeDelegateHandle.Reset();
		}
	}

	if (OnObjectCreateButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CreateButtonWidget)) {
			CreateButtonWidget->GetOnButtonClicked().Remove(OnObjectCreateButtonClickedDelegateHandle);
			OnObjectCreateButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUIObjectListWidget::GetObjects(const IApiBatchQueryRequestMetadata& RequestMetadata) {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiObjectSubsystem> ObjectSubsystem = ApiModule->GetObjectSubsystem()) {
			{
				const TSharedRef<FOnObjectBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnObjectBatchRequestCompleted());

				CallbackPtr->BindWeakLambda(this, [this](const FApiObjectBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
					if (!bInSuccessful) {
						LogErrorF("failed to request: %s", *InError);
					}

					// Finish processing.
					SetIsProcessing(false);

					OnObjectsLoaded(InMetadata, bInSuccessful, InError);
				});

				if (!ObjectSubsystem->GetBatch(RequestMetadata, CallbackPtr)) {
					LogErrorF("failed to request");
				} else {
					SetIsProcessing(true);
				}
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Object)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUIObjectListWidget::OnObjectFormButtonClicked() const {
	if (OnObjectCreateButtonClicked.IsBound()) {
		OnObjectCreateButtonClicked.Broadcast();
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
}

void UUIObjectListWidget::NativeConstruct() {
	if (IsValid(ContainerWidget)) {
		ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
			if (OnObjectListItemClicked.IsBound()) {
				if (UApiObjectMetadataObject* Object = Cast<UApiObjectMetadataObject>(InObject)) {
					OnObjectListItemClicked.Broadcast(Object);
				}
			}
		});
	}

	SetIsProcessing(false);
	Super::NativeConstruct();
}

void UUIObjectListWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

void UUIObjectListWidget::Refresh() {
	IApiBatchQueryRequestMetadata RequestMetadata;
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
	RequestMetadata.Query = CurrentQuery;
	GetObjects(RequestMetadata);
}

void UUIObjectListWidget::SetObjects(const FApiObjectBatchMetadata& BatchMetadata) {
	if (IsValid(ContainerWidget)) {
		ContainerWidget->ClearListItems();
		ContainerWidget->SetListItems(UApiObjectMetadataObject::BatchToUObjectArray(BatchMetadata.Entities, this));
	}

	if (IsValid(PaginationWidget)) {
		PaginationWidget->SetPaginationOptions(BatchMetadata.Total, ItemsPerPage);
	}
}

void UUIObjectListWidget::OnObjectsLoaded(const FApiObjectBatchMetadata& Metadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(PaginationWidget)) { PaginationWidget->SetPaginationOptions(Metadata.Total, ItemsPerPage); }
		if (IsValid(ContainerWidget)) { ContainerWidget->ScrollToTop(); }
		SetObjects(Metadata);
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

#undef LOCTEXT_NAMESPACE
