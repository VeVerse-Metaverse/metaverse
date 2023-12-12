// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIUserListWidget.h"

#include "ApiUserSubsystem.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Components/UILoadingWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIUserListWidget::NativeConstruct() {
	Super::NativeConstruct();
	SetIsProcessing(false);
}

void UUIUserListWidget::SetIsProcessing(bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

void UUIUserListWidget::Refresh() {
	IApiBatchQueryRequestMetadata RequestMetadata;
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
	RequestMetadata.Query = CurrentQuery;
	RequestUsers(RequestMetadata);
}

void UUIUserListWidget::SetUsers(const FApiUserBatchMetadata& BatchMetadata) {
	if (IsValid(ContainerWidget)) {
		ContainerWidget->ClearListItems();
		ContainerWidget->SetListItems(UApiUserMetadataObject::BatchToUObjectArray(BatchMetadata.Entities, this));
	}

	if (IsValid(PaginationWidget)) {
		PaginationWidget->SetPaginationOptions(BatchMetadata.Total, ItemsPerPage);
	}
}

void UUIUserListWidget::OnUsersLoaded(const FApiUserBatchMetadata& Metadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(PaginationWidget)) {
			PaginationWidget->SetPaginationOptions(Metadata.Total, ItemsPerPage);
		}
		if (IsValid(ContainerWidget)) {
			ContainerWidget->ScrollToTop();
		}
		SetUsers(Metadata);
	} else {
		FUINotificationData NotificationData;
		NotificationData.Header = LOCTEXT("LoadUserBatchError", "Error");
		NotificationData.Lifetime = 3.f;
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = FText::FromString("Failed to load a user batch.");
		AddNotification(NotificationData);
	}
}

void UUIUserListWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();
	
	if(!OnContainerItemClickedDelegateHandle.IsValid()) {
		if (ContainerWidget) {
			OnContainerItemClickedDelegateHandle = ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
				if (OnUserListItemClicked.IsBound()) {
					if (UApiUserMetadataObject* User = Cast<UApiUserMetadataObject>(InObject)) {
						OnUserListItemClicked.Broadcast(User);
					}
				}
			});
		}
	}

	// if (!OnUserBatchLoadedDelegateHandle.IsValid()) {
	// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 		if (const TSharedPtr<FApiUserSubsystem> UserSubsystem = ApiModule->GetUserSubsystem()) {
	// 			{
	// 				OnUserBatchLoadedDelegateHandle = UserSubsystem->GetOnBatchLoaded().AddLambda(
	// 					[this](const FApiUserBatchMetadata& Metadata, const bool bSuccessful, const FString& Error) {
	// 						SetIsProcessing(false);
	// 						if (bSuccessful) {
	// 							if (PaginationWidget) { PaginationWidget->SetPaginationOptions(Metadata.Total, ItemsPerPage); }
	// 							if (ContainerWidget) { ContainerWidget->ScrollToTop(); }
	// 							SetUsers(Metadata);
	// 						} else {
	// 							FUINotificationData NotificationData;
	// 							NotificationData.Header = LOCTEXT("LoadUserBatchError", "Error");
	// 							NotificationData.Lifetime = 3.f;
	// 							NotificationData.Type = EUINotificationType::Failure;
	// 							NotificationData.Message = FText::FromString("Failed to load a user batch.");
	// 							GetHUD()->AddNotification(NotificationData);
	// 						}
	// 					});
	// 			}
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(User)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	// }

	if (!OnUserListItemClickedDelegateHandle.IsValid()) {
		OnUserListItemClickedDelegateHandle = OnUserListItemClicked.AddWeakLambda(this, [this](UApiUserMetadataObject* InUser) {
			if (InUser) {
				if (OnUserSelected.IsBound()) {
					OnUserSelected.Broadcast(InUser->Metadata);
				}
			} else {
				FUINotificationData NotificationData;
				NotificationData.Header = NSLOCTEXT("VeUI", "OpenUserError", "Error");
				NotificationData.Lifetime = 3.f;
				NotificationData.Type = EUINotificationType::Failure;
				NotificationData.Message = FText::FromString("Failed to load the User.");
				AddNotification(NotificationData);
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
								if (PaginationWidget) {
									IApiBatchQueryRequestMetadata RequestMetadata;
									RequestMetadata.Limit = PaginationWidget->ItemsPerPage;
									RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
									RequestMetadata.Query = CurrentQuery;
									RequestUsers(RequestMetadata);
								}
							}
						});
				}
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	}
#endif

	if (!OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (PaginationWidget) {
			OnPaginationPageChangeDelegateHandle = PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
				IApiBatchQueryRequestMetadata RequestMetadata;
				RequestMetadata.Limit = InItemsPerPage;
				RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
				RequestMetadata.Query = CurrentQuery;
				RequestUsers(RequestMetadata);
			});
		}
	}

	if (!OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (RefreshButtonWidget) {
			OnRefreshButtonClickedDelegateHandle = RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIUserListWidget::Refresh);
		}
	}

	if (!OnUserNameFilterChangeDelegateHandle.IsValid()) {
		if (UserNameFilterWidget) {
			OnUserNameFilterChangeDelegateHandle = UserNameFilterWidget->OnFilterChanged.AddWeakLambda(this, [this](FString InQuery) {
				IApiBatchQueryRequestMetadata RequestMetadata;
				RequestMetadata.Query = InQuery;
				RequestMetadata.Limit = ItemsPerPage;
				RequestMetadata.Offset = 0;
				CurrentQuery = InQuery;
				if (PaginationWidget) {
					PaginationWidget->ResetPagination();
				}
				RequestUsers(RequestMetadata);
			});
		}
	}
}

void UUIUserListWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();
	
	if (OnContainerItemClickedDelegateHandle.IsValid()) {
		if (ContainerWidget) {
			ContainerWidget->OnItemClicked().Remove(OnContainerItemClickedDelegateHandle);
			OnContainerItemClickedDelegateHandle.Reset();
		}
	}
	
	// if (OnUserBatchLoadedDelegateHandle.IsValid()) {
	// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 		if (const TSharedPtr<FApiUserSubsystem> UserSubsystem = ApiModule->GetUserSubsystem()) {
	// 			{
	// 				UserSubsystem->GetOnBatchLoaded().Remove(OnUserBatchLoadedDelegateHandle);
	// 				OnUserBatchLoadedDelegateHandle.Reset();
	// 			}
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(User)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
	// }

	if (OnUserListItemClickedDelegateHandle.IsValid()) {
		OnUserListItemClicked.Remove(OnUserListItemClickedDelegateHandle);
		OnUserListItemClickedDelegateHandle.Reset();
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

	if (OnUserNameFilterChangeDelegateHandle.IsValid()) {
		if (UserNameFilterWidget) {
			UserNameFilterWidget->OnFilterChanged.Remove(OnUserNameFilterChangeDelegateHandle);
			OnUserNameFilterChangeDelegateHandle.Reset();
		}
	}
}

void UUIUserListWidget::RequestUsers(const IApiBatchQueryRequestMetadata& RequestMetadata) {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiUserSubsystem> UserSubsystem = ApiModule->GetUserSubsystem()) {
			{
				// 1. Create a callback.
				TSharedPtr<FOnUserBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnUserBatchRequestCompleted());

				// 2. Bind callback.
				CallbackPtr->BindWeakLambda(this, [=](const FApiUserBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
					if (!bInSuccessful) {
						LogErrorF("failed to request: %s", *InError);
					}

					// Finish processing.
					SetIsProcessing(false);

					OnUsersLoaded(InMetadata, bInSuccessful, InError);

					CallbackPtr.Reset();
				});

				// 3. Make the request using the callback.
				if (!UserSubsystem->GetBatch(RequestMetadata, CallbackPtr)) {
					LogErrorF("failed to request");
				} else {
					SetIsProcessing(true);
				}
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(User)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}


#undef LOCTEXT_NAMESPACE
