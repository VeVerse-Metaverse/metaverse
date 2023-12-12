// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIUserBrowserWidget.h"

#include "VR/UIUserDetailWidget.h"
#include "VR/UIUserListWidget.h"
#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VeApi.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIUserBrowserWidget::NativeConstruct() {
	bUserDetailWidgetVisible = true;
	bUserListWidgetVisible = true;

	NavigateToUserList(false, false);

	
	Super::NativeConstruct();
}

void UUIUserBrowserWidget::RegisterCallbacks() {

	if (UserDetailWidget) {
		if (!OnUserDetailWidgetClosedDelegateHandle.IsValid()) {
			OnUserDetailWidgetClosedDelegateHandle = UserDetailWidget->OnClosed.AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToUserList();
			});
		}
	}

	if (UserListWidget) {
		if (!OnUserSelectedDelegateHandle.IsValid()) {
			OnUserSelectedDelegateHandle = UserListWidget->OnUserSelected.AddWeakLambda(this, [this](const TSharedPtr<FApiUserMetadata> InMetadata) {
				if (UserDetailWidget) {
					UserDetailWidget->SetMetadata(InMetadata);
				}

				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Push(LOCTEXT("PeopleDetails", "Person Details"));
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToUserDetail();
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
}

void UUIUserBrowserWidget::UnregisterCallbacks() {
	if (OnUserDetailWidgetClosedDelegateHandle.IsValid()) {
		if (UserDetailWidget) {
			UserDetailWidget->OnClosed.Remove(OnUserDetailWidgetClosedDelegateHandle);
			OnUserDetailWidgetClosedDelegateHandle.Reset();
		}
	}

	if (OnUserSelectedDelegateHandle.IsValid()) {
		if (UserListWidget) {
			UserListWidget->OnUserSelected.Remove(OnUserSelectedDelegateHandle);
			OnUserSelectedDelegateHandle.Reset();
		}
	}

	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUIUserBrowserWidget::OnNavigate() {
	UserListWidget->Refresh();

	Super::OnNavigate();
}


void UUIUserBrowserWidget::ShowUserListWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bUserListWidgetVisible) {
		bUserListWidgetVisible = true;
		if (UserListWidget) {
			UserListWidget->ResetState();
			UserListWidget->ResetAnimationState();
			UserListWidget->Show(bAnimate, bPlaySound);
		}
	}
}

void UUIUserBrowserWidget::ShowUserDetailWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bUserDetailWidgetVisible) {
		bUserDetailWidgetVisible = true;
		if (UserDetailWidget) {
			UserDetailWidget->ResetState();
			UserDetailWidget->ResetAnimationState();
			UserDetailWidget->Show(bAnimate, bPlaySound);
		}
	}
}

void UUIUserBrowserWidget::HideUserListWidget(const bool bAnimate, const bool bPlaySound) {
	if (bUserListWidgetVisible) {
		bUserListWidgetVisible = false;
		if (UserListWidget) {
			UserListWidget->Hide(bAnimate, bPlaySound);
		}
	}
}

void UUIUserBrowserWidget::HideUserDetailWidget(const bool bAnimate, const bool bPlaySound) {
	if (bUserDetailWidgetVisible) {
		bUserDetailWidgetVisible = false;
		if (UserDetailWidget) {
			UserDetailWidget->Hide(bAnimate, bPlaySound);
		}
	}
}

void UUIUserBrowserWidget::NavigateToUserList(const bool bAnimate, const bool bPlaySound) {
	ShowUserListWidget();
	HideUserDetailWidget();
}

void UUIUserBrowserWidget::NavigateToUserDetail(const bool bAnimate, const bool bPlaySound) {
	HideUserListWidget();
	ShowUserDetailWidget();
}

#undef LOCTEXT_NAMESPACE