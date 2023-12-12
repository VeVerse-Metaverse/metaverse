// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UISpaceBrowserWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VR/UISpaceListWidget.h"
#include "VR/UISpaceDetailWidget.h"
#include "VeApi.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUISpaceBrowserWidget::OnNavigate() {
	SpaceListWidget->Refresh();

	Super::OnNavigate();
}

void UUISpaceBrowserWidget::NativeConstruct() {
	bSpaceDetailWidgetVisible = true;
	bSpaceListWidgetVisible = true;
	bSpaceFormWidgetVisible = true;

	NavigateToSpaceList(false, false);


	Super::NativeConstruct();
}

void UUISpaceBrowserWidget::RegisterCallbacks() {
	if (!OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CloseButtonWidget)) {
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

	if (IsValid(SpaceDetailWidget)) {
		if (!OnSpaceDetailWidgetClosedDelegateHandle.IsValid()) {
			OnSpaceDetailWidgetClosedDelegateHandle = SpaceDetailWidget->OnClosed.AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToSpaceList();
			});
		}
	}

	if (IsValid(SpaceListWidget)) {
		if (!OnSpaceSelectedDelegateHandle.IsValid()) {
			OnSpaceSelectedDelegateHandle = SpaceListWidget->OnSpaceSelected.AddWeakLambda(this, [this](TSharedPtr<FApiSpaceMetadata> InMetadata) {
				if (IsValid(SpaceDetailWidget)) {
					SpaceDetailWidget->SetMetadata(InMetadata);
				}

				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Push(LOCTEXT("SpaceDetails", "Space Details"));
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToSpaceDetail();
			});
		}
	}

	if (IsValid(SpaceListWidget)) {
		if (!OnSpaceCreateButtonClickedDelegateHandle.IsValid()) {
			OnSpaceCreateButtonClickedDelegateHandle = SpaceListWidget->OnSpaceCreateButtonClicked.AddWeakLambda(this, [this] {
				if (IsValid(SpaceFormWidget)) {
					SpaceFormWidget->SetMetadata(nullptr);

					if (FVeUIModule* UIModule = FVeUIModule::Get()) {
						if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
							{
								BreadcrumbSubsystem->Push(LOCTEXT("SpaceCreate", "Create Space"));
							}
						} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
					} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

					NavigateToSpaceForm();
				}
			});
		}
	}

	if (IsValid(SpaceDetailWidget)) {
		if (!OnSpaceEditButtonClickedDelegateHandle.IsValid()) {
			OnSpaceEditButtonClickedDelegateHandle = SpaceDetailWidget->OnSpaceEditButtonClicked.AddWeakLambda(this, [this](TSharedPtr<FApiSpaceMetadata> InMetadata) {
				if (IsValid(SpaceFormWidget)) {
					SpaceFormWidget->SetMetadata(InMetadata);

					if (FVeUIModule* UIModule = FVeUIModule::Get()) {
						if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
							{
								BreadcrumbSubsystem->Push(LOCTEXT("SpaceEdit", "Edit Space"));
							}
						} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
					} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

					NavigateToSpaceForm();
				}
			});
		}
	}

	if (IsValid(SpaceFormWidget)) {
		if (!OnSpaceFormClosedDelegateHandle.IsValid()) {
			OnSpaceFormClosedDelegateHandle = SpaceFormWidget->OnClosed.AddWeakLambda(this, [this](TSharedPtr<FApiSpaceMetadata> InMetadata) {
				if (InMetadata.IsValid() && InMetadata->Id.IsValid()) {
					if (IsValid(SpaceDetailWidget)) {
						SpaceDetailWidget->SetMetadata(InMetadata);
					}
					NavigateToSpaceDetail();
				} else {
					NavigateToSpaceList();
				}

				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
			});
		}
	}
}

void UUISpaceBrowserWidget::UnregisterCallbacks() {
	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CloseButtonWidget)) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnSpaceDetailWidgetClosedDelegateHandle.IsValid()) {
		if (IsValid(SpaceDetailWidget)) {
			SpaceDetailWidget->OnClosed.Remove(OnSpaceDetailWidgetClosedDelegateHandle);
			OnSpaceDetailWidgetClosedDelegateHandle.Reset();
		}
	}

	if (OnSpaceSelectedDelegateHandle.IsValid()) {
		if (IsValid(SpaceListWidget)) {
			SpaceListWidget->OnSpaceSelected.Remove(OnSpaceSelectedDelegateHandle);
			OnSpaceSelectedDelegateHandle.Reset();
		}
	}

	if (OnSpaceCreateButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(SpaceListWidget)) {
			SpaceListWidget->OnSpaceCreateButtonClicked.Remove(OnSpaceCreateButtonClickedDelegateHandle);
			OnSpaceCreateButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnSpaceFormClosedDelegateHandle.IsValid()) {
		if (IsValid(SpaceFormWidget)) {
			SpaceFormWidget->OnClosed.Remove(OnSpaceFormClosedDelegateHandle);
			OnSpaceFormClosedDelegateHandle.Reset();
		}
	}

}

void UUISpaceBrowserWidget::ShowSpaceListWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bSpaceListWidgetVisible) {
		bSpaceListWidgetVisible = true;
		if (SpaceListWidget) {
			SpaceListWidget->ResetState();
			SpaceListWidget->ResetAnimationState();
			SpaceListWidget->Show(bAnimate, bPlaySound);
		}
	}
}

void UUISpaceBrowserWidget::ShowSpaceDetailWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bSpaceDetailWidgetVisible) {
		bSpaceDetailWidgetVisible = true;
		if (SpaceDetailWidget) {
			SpaceDetailWidget->ResetState();
			SpaceDetailWidget->ResetAnimationState();
			SpaceDetailWidget->Show(bAnimate, bPlaySound);
		}
	}
}

void UUISpaceBrowserWidget::ShowSpaceFormWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bSpaceFormWidgetVisible) {
		bSpaceFormWidgetVisible = true;
		if (SpaceFormWidget) {
			SpaceFormWidget->ResetState();
			SpaceFormWidget->ResetAnimationState();
			SpaceFormWidget->Show(bAnimate, bPlaySound);
		}
	}
}

void UUISpaceBrowserWidget::HideSpaceListWidget(const bool bAnimate, const bool bPlaySound) {
	if (bSpaceListWidgetVisible) {
		bSpaceListWidgetVisible = false;
		if (SpaceListWidget) {
			SpaceListWidget->Hide(bAnimate, bPlaySound);
		}
	}
}

void UUISpaceBrowserWidget::HideSpaceDetailWidget(const bool bAnimate, const bool bPlaySound) {
	if (bSpaceDetailWidgetVisible) {
		bSpaceDetailWidgetVisible = false;
		if (SpaceDetailWidget) {
			SpaceDetailWidget->Hide(bAnimate, bPlaySound);
		}
	}
}

void UUISpaceBrowserWidget::HideSpaceFormWidget(const bool bAnimate, const bool bPlaySound) {
	if (bSpaceFormWidgetVisible) {
		bSpaceFormWidgetVisible = false;
		if (SpaceFormWidget) {
			SpaceFormWidget->Hide(bAnimate, bPlaySound);
		}
	}
}

void UUISpaceBrowserWidget::NavigateToSpaceList(const bool bAnimate, const bool bPlaySound) {
	ShowSpaceListWidget();
	HideSpaceDetailWidget();
	HideSpaceFormWidget();
}

void UUISpaceBrowserWidget::NavigateToSpaceDetail(const bool bAnimate, const bool bPlaySound) {
	HideSpaceListWidget();
	ShowSpaceDetailWidget();
	HideSpaceFormWidget();
}

void UUISpaceBrowserWidget::NavigateToSpaceForm(const bool bAnimate, const bool bPlaySound) {
	HideSpaceListWidget();
	HideSpaceDetailWidget();
	ShowSpaceFormWidget();
}

#undef LOCTEXT_NAMESPACE
