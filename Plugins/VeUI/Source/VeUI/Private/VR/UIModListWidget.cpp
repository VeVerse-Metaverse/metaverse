// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIModListWidget.h"

#include "UINotificationData.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Api2PackageSubsystem.h"
#include "ApiPackageMetadataObject.h"


#define LOCTEXT_NAMESPACE "VeUI"

void UUIModListWidget::RegisterCallbacks() {
	// if (!OnModBatchLoadedDelegateHandle.IsValid()) {
	// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 		if (const TSharedPtr<FApiModSubsystem> ModSubsystem = ApiModule->GetModSubsystem()) {
	// 			{
	// 				OnModBatchLoadedDelegateHandle = ModSubsystem->GetOnBatchLoaded().AddLambda(
	// 					[this](const FApiModBatchMetadata& Metadata, const bool bSuccessful, const FString& Error) {
	// 						SetIsProcessing(false);
	// 						if (bSuccessful) {
	// 							if (PaginationWidget) { PaginationWidget->SetPaginationOptions(Metadata.Total, ItemsPerPage); }
	// 							if (ContainerWidget) { ContainerWidget->ScrollToTop(); }
	// 							SetMods(Metadata);
	// 						} else {
	// 							FUINotificationData NotificationData;
	// 							NotificationData.Header = LOCTEXT("LoadSpaceBatchError", "Load space batch error");
	// 							NotificationData.Lifetime = 3.f;
	// 							NotificationData.Type = EUINotificationType::Failure;
	// 							NotificationData.Message = FText::FromString("Failed to load a space batch.");
	// 							GetHUD()->AddNotification(NotificationData);
	// 						}
	// 					});
	// 			}
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Mod)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	// }

	if (!OnMetaverseListItemClickedDelegateHandle.IsValid()) {
		OnMetaverseListItemClickedDelegateHandle = OnMetaverseListItemClicked.AddWeakLambda(this, [this](UApiPackageMetadataObject* InSpace) {
			if (InSpace) {
				if (OnMetaverseSelected.IsBound()) {
					OnMetaverseSelected.Broadcast(InSpace->Metadata);
				}
			} else {
				FUINotificationData NotificationData;
				NotificationData.Header = NSLOCTEXT("VeUI", "OpenVirtualWorldError", "Error");
				NotificationData.Lifetime = 3.f;
				NotificationData.Type = EUINotificationType::Failure;
				NotificationData.Message = FText::FromString("Failed to load the virtual world.");
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
								if (IsValid(PaginationWidget)) {
									IApiBatchQueryRequestMetadata RequestMetadata;
									RequestMetadata.Limit = PaginationWidget->ItemsPerPage;
									RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
									RequestMetadata.Query = CurrentQuery;
									GetMods(RequestMetadata);
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
				GetMods(RequestMetadata);
			});
		}
	}


	if (!OnRefreshButtonClickedDelegateHandle.IsValid()) {
		if (RefreshButtonWidget) {
			OnRefreshButtonClickedDelegateHandle = RefreshButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIModListWidget::Refresh);
		}
	}

	if (!OnModNameFilterChangeDelegateHandle.IsValid()) {
		if (ModNameFilterWidget) {
			OnModNameFilterChangeDelegateHandle = ModNameFilterWidget->OnFilterChanged.AddWeakLambda(this, [this](FString InQuery) {
				IApiBatchQueryRequestMetadata RequestMetadata;
				RequestMetadata.Query = InQuery;
				RequestMetadata.Limit = ItemsPerPage;
				RequestMetadata.Offset = 0;
				CurrentQuery = InQuery;
				if (PaginationWidget) {
					PaginationWidget->ResetPagination();
				}
				GetMods(RequestMetadata);
			});
		}
	}
}

void UUIModListWidget::UnregisterCallbacks() {
	// if (OnModBatchLoadedDelegateHandle.IsValid()) {
	// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 		if (const TSharedPtr<FApiModSubsystem> ModSubsystem = ApiModule->GetModSubsystem()) {
	// 			{
	// 				ModSubsystem->GetOnBatchLoaded().Remove(OnModBatchLoadedDelegateHandle);
	// 				OnModBatchLoadedDelegateHandle.Reset();
	// 			}
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Mod)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	// }

	if (OnMetaverseListItemClickedDelegateHandle.IsValid()) {
		OnMetaverseListItemClicked.Remove(OnMetaverseListItemClickedDelegateHandle);
		OnMetaverseListItemClickedDelegateHandle.Reset();
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

	if (OnModNameFilterChangeDelegateHandle.IsValid()) {
		if (ModNameFilterWidget) {
			ModNameFilterWidget->OnFilterChanged.Remove(OnModNameFilterChangeDelegateHandle);
			OnModNameFilterChangeDelegateHandle.Reset();
		}
	}
}

void UUIModListWidget::GetMods(const IApiBatchQueryRequestMetadata& RequestMetadata) {
	GET_MODULE_SUBSYSTEM(PackageSubsystem, Api2, Package);
	if (PackageSubsystem) {
		const auto CallbackPtr = MakeShared<FOnPackageBatchRequestCompleted2>();

		CallbackPtr->BindWeakLambda(this, [this](const FApiPackageBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
			if (!bSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// Finish processing.
			SetIsProcessing(false);

			OnModBatchLoaded(InMetadata, bSuccessful, InError);
		});

		// todo: REFACTOR CODE
		FApiPackageIndexRequestMetadata RequestMetadata1;
		RequestMetadata1.Query = RequestMetadata.Query;
		RequestMetadata1.Limit = RequestMetadata.Limit;
		RequestMetadata1.Offset = RequestMetadata.Offset;
		if (IsRunningDedicatedServer()) {
			RequestMetadata1.Deployment = "Server";
		} else {
			RequestMetadata1.Deployment = "Client";
		}
#if PLATFORM_WINDOWS
		RequestMetadata1.Platform = "Windows";
#elif PLATFORM_MAC
		RequestMetadata1.Platform = "Mac";
#elif PLATFORM_LINUX
		RequestMetadata1.Platform = "Linux";
#endif
		// :end REFACTOR CODE

		PackageSubsystem->Index(RequestMetadata1, CallbackPtr);
	}

	// todo: REMOVE DEPRECATED CODE
	// if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 	if (const TSharedPtr<FApiMetaverseSubsystem> ModSubsystem = ApiModule->GetMetaverseSubsystem()) {
	// 		{
	// 			const TSharedRef<FOnMetaverseBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnMetaverseBatchRequestCompleted());
	//
	// 			CallbackPtr->BindWeakLambda(this, [this](const FApiMetaverseBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
	// 				if (!bInSuccessful) {
	// 					LogErrorF("failed to request: %s", *InError);
	// 				}
	//
	// 				// Finish processing.
	// 				SetIsProcessing(false);
	//
	// 				OnModBatchLoaded(InMetadata, bInSuccessful, InError);
	// 			});
	//
	// 			if (!ModSubsystem->GetBatch(RequestMetadata, CallbackPtr)) {
	// 				LogErrorF("failed to request");
	// 				FUINotificationData NotificationData;
	// 				NotificationData.Header = LOCTEXT("Error", "Error");
	// 				NotificationData.Message = LOCTEXT("FailedToLoadVirtualWorld", "Failed to load virtual world");
	// 				NotificationData.Type = EUINotificationType::Failure;
	// 				AddNotification(NotificationData);
	// 			} else {
	// 				SetIsProcessing(true);
	// 			}
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Mod)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUIModListWidget::NativeConstruct() {
	if (IsValid(ContainerWidget)) {
		ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
			if (OnMetaverseListItemClicked.IsBound()) {
				if (UApiPackageMetadataObject* Mod = Cast<UApiPackageMetadataObject>(InObject)) {
					OnMetaverseListItemClicked.Broadcast(Mod);
				}
			}
		});
	}

	SetIsProcessing(false);

	Super::NativeConstruct();
}

void UUIModListWidget::SetIsProcessing(bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

void UUIModListWidget::Refresh() {
	IApiBatchQueryRequestMetadata RequestMetadata;
	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
	RequestMetadata.Query = CurrentQuery;
	GetMods(RequestMetadata);
}

void UUIModListWidget::SetMetaverses(const FApiPackageBatchMetadata& BatchMetadata) {
	if (IsValid(ContainerWidget)) {
		ContainerWidget->ClearListItems();
		ContainerWidget->SetListItems(UApiPackageMetadataObject::BatchToUObjectArray(BatchMetadata.Entities, this));
	}

	if (IsValid(PaginationWidget)) {
		PaginationWidget->SetPaginationOptions(BatchMetadata.Total, ItemsPerPage);
	}
}

void UUIModListWidget::OnModBatchLoaded(const FApiPackageBatchMetadata& Metadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(PaginationWidget)) { PaginationWidget->SetPaginationOptions(Metadata.Total, ItemsPerPage); }
		if (IsValid(ContainerWidget)) { ContainerWidget->ScrollToTop(); }
		SetMetaverses(Metadata);
	} else {
		FUINotificationData NotificationData;
		NotificationData.Header = LOCTEXT("Error", "Error");
		NotificationData.Lifetime = 3.f;
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = LOCTEXT("FailedToLoadvirtualworld", "Failed to load virtual worlds");
		AddNotification(NotificationData);
	}
}

#undef LOCTEXT_NAMESPACE
