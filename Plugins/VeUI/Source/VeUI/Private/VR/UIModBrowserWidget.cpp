// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIModBrowserWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VR/UIModDetailWidget.h"
#include "VR/UIModListWidget.h"
#include "Subsystems/UINavigationSubsystem.h"
#include "VeApi.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIModBrowserWidget::NativeConstruct() {
	bModDetailWidgetVisible = true;
	bModListWidgetVisible = true;

	NavigateToModList(false, false);

	Super::NativeConstruct();
}

void UUIModBrowserWidget::RegisterCallbacks() {
	// if (!OnModBatchLoadedDelegateHandle.IsValid()) {
	// 	TSharedPtr<FApiModSubsystem> ModSubsystem = FVeApiModule::Get().GetModSubsystem();
	// 	if (ModSubsystem.IsValid()) {
	// 		OnModBatchLoadedDelegateHandle = ModSubsystem->GetOnBatchLoaded().AddLambda([this](const FApiModBatchMetadata& Metadata, const bool bSuccessful, const FString& Error) {
	// 			if (bSuccessful) {
	// 				ModListWidget->SetMods(Metadata);
	// 			} else {
	// 				FUINotificationData NotificationData;
	// 				NotificationData.Header = NSLOCTEXT("VeUI", "LoadSpaceBatchError", "Error");
	// 				NotificationData.Lifetime = 3.f;
	// 				NotificationData.Type = EUINotificationType::Failure;
	// 				NotificationData.Message = FText::FromString("Failed to load a space batch.");
	// 				GetHUD()->AddNotification(NotificationData);
	// 			}
	// 		});
	// 	}
	// }
	//
	// if (ModListWidget) {
	// 	if (!OnModListItemClickedDelegateHandle.IsValid()) {
	// 		OnModListItemClickedDelegateHandle = ModListWidget->OnModListItemClicked.AddWeakLambda(this, [this](UApiModMetadataObject* InMetadataObject) {
	// 			if (InMetadataObject) {
	// 				NavigateToModDetail();
	// 				ModDetailWidget->SetMetadata(InMetadataObject->Metadata);
	// 			} else {
	// 				FUINotificationData NotificationData;
	// 				NotificationData.Header = NSLOCTEXT("VeUI", "OpenModError", "Error");
	// 				NotificationData.Lifetime = 3.f;
	// 				NotificationData.Type = EUINotificationType::Failure;
	// 				NotificationData.Message = FText::FromString("Failed to load the mod.");
	// 				GetHUD()->AddNotification(NotificationData);
	// 			}
	// 		});
	// 	}
	// }

	if (ModDetailWidget) {
		if (!OnModDetailWidgetClosedDelegateHandle.IsValid()) {
			OnModDetailWidgetClosedDelegateHandle = ModDetailWidget->OnClosed.AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToModList();
			});
		}
	}

	if (ModListWidget) {
		if (!OnModSelectedDelegateHandle.IsValid()) {
			OnModSelectedDelegateHandle = ModListWidget->OnMetaverseSelected.AddWeakLambda(this, [this](TSharedPtr<FApiPackageMetadata> InMetadata) {
				if (ModDetailWidget) {
					ModDetailWidget->SetMetadata(InMetadata);
				}

				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Push(LOCTEXT("VirtualWorldDetails", "Virtual World Details"));
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToModDetail();
			});
		}
	}

	if (!OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			OnCloseButtonClickedDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
			});
		}
	}

	// if (!OnAuthSubsystemUserLoginDelegateHandle.IsValid()) {
	// 	TSharedPtr<FApiAuthSubsystem> AuthenticationSubsystem = FVeApiModule::Get().GetAuthSubsystem();
	// 	if (AuthenticationSubsystem.IsValid()) {
	// 		OnAuthSubsystemUserLoginDelegateHandle = AuthenticationSubsystem->GetOnUserLogin().AddWeakLambda(
	// 			this, [this](const FApiUserMetadata&, const bool bSuccessful, const FString&) {
	// 				if (bSuccessful) {
	// 					IApiBatchQueryRequestMetadata RequestMetadata;
	// 					RequestMetadata.Offset = 0;
	// 					RequestMetadata.Limit = 40;
	// 					GetMods(RequestMetadata);
	// 				}
	// 			});
	// 	}
	// }
}

void UUIModBrowserWidget::UnregisterCallbacks() {
	// if (OnModBatchLoadedDelegateHandle.IsValid()) {
	// 	TSharedPtr<FApiModSubsystem> ModSubsystem = FVeApiModule::Get().GetModSubsystem();
	// 	if (ModSubsystem.IsValid()) {
	// 		ModSubsystem->GetOnBatchLoaded().Remove(OnModBatchLoadedDelegateHandle);
	// 		OnModBatchLoadedDelegateHandle.Reset();
	// 	}
	// }
	//
	// if (OnModListItemClickedDelegateHandle.IsValid()) {
	// 	if (ModListWidget) {
	// 		ModListWidget->OnModListItemClicked.Remove(OnModListItemClickedDelegateHandle);
	// 		OnModListItemClickedDelegateHandle.Reset();
	// 	}
	// }

	if (OnModDetailWidgetClosedDelegateHandle.IsValid()) {
		if (ModDetailWidget) {
			ModDetailWidget->OnClosed.Remove(OnModDetailWidgetClosedDelegateHandle);
			OnModDetailWidgetClosedDelegateHandle.Reset();
		}
	}

	if (OnModSelectedDelegateHandle.IsValid()) {
		if (ModListWidget) {
			ModListWidget->OnMetaverseSelected.Remove(OnModSelectedDelegateHandle);
			OnModSelectedDelegateHandle.Reset();
		}
	}

	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}

	// if (OnAuthSubsystemUserLoginDelegateHandle.IsValid()) {
	// 	TSharedPtr<FApiAuthSubsystem> AuthenticationSubsystem = FVeApiModule::Get().GetAuthSubsystem();
	// 	if (AuthenticationSubsystem.IsValid()) {
	// 		AuthenticationSubsystem->GetOnUserLogin().Remove(OnAuthSubsystemUserLoginDelegateHandle);
	// 		OnAuthSubsystemUserLoginDelegateHandle.Reset();
	// 	}
	// }
}

void UUIModBrowserWidget::OnNavigate() {
	ModListWidget->Refresh();

	Super::OnNavigate();
}

// void UUIModBrowserWidget::GetMods(const IApiBatchQueryRequestMetadata& RequestMetadata) const {
// 	TSharedPtr<FApiModSubsystem> ModSubsystem = FVeApiModule::Get().GetModSubsystem();
// 	if (ModSubsystem.IsValid()) {
// 		ModSubsystem->GetBatch((RequestMetadata));
// 	}
// }

void UUIModBrowserWidget::ShowModListWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bModListWidgetVisible) {
		bModListWidgetVisible = true;
		if (ModListWidget) {
			ModListWidget->ResetState();
			ModListWidget->ResetAnimationState();
			ModListWidget->Show(bAnimate, bPlaySound);
		}
	}
}

void UUIModBrowserWidget::ShowModDetailWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bModDetailWidgetVisible) {
		bModDetailWidgetVisible = true;
		if (ModDetailWidget) {
			ModDetailWidget->ResetState();
			ModDetailWidget->ResetAnimationState();
			ModDetailWidget->Show(bAnimate, bPlaySound);
		}
	}
}

void UUIModBrowserWidget::HideModListWidget(const bool bAnimate, const bool bPlaySound) {
	if (bModListWidgetVisible) {
		bModListWidgetVisible = false;
		if (ModListWidget) {
			ModListWidget->Hide(bAnimate, bPlaySound);
		}
	}
}

void UUIModBrowserWidget::HideModDetailWidget(const bool bAnimate, const bool bPlaySound) {
	if (bModDetailWidgetVisible) {
		bModDetailWidgetVisible = false;
		if (ModDetailWidget) {
			ModDetailWidget->Hide(bAnimate, bPlaySound);
		}
	}
}

void UUIModBrowserWidget::NavigateToModList(const bool bAnimate, const bool bPlaySound) {
	ShowModListWidget();
	HideModDetailWidget();
}

void UUIModBrowserWidget::NavigateToModDetail(const bool bAnimate, const bool bPlaySound) {
	HideModListWidget();
	ShowModDetailWidget();
}

void UUIModBrowserWidget::Navigate(const FString& Options) {
	// todo: implement
}

#undef LOCTEXT_NAMESPACE
