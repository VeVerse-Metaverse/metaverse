// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIObjectBrowserWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VR/UIObjectListWidget.h"
#include "VR/UIObjectDetailWidget.h"
#include "VeApi.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIObjectBrowserWidget::OnNavigate() {
	ObjectListWidget->Refresh();

	Super::OnNavigate();
}

void UUIObjectBrowserWidget::NativeConstruct() {
	bObjectDetailWidgetVisible = true;
	bObjectListWidgetVisible = true;
	bObjectFormWidgetVisible = true;

	NavigateToObjectList(false, false);

	
	Super::NativeConstruct();
}

void UUIObjectBrowserWidget::RegisterCallbacks() {
	if (ObjectListWidget) {
		if (!OnObjectSelectedDelegateHandle.IsValid()) {
			OnObjectSelectedDelegateHandle = ObjectListWidget->OnObjectSelected.AddWeakLambda(this, [this](TSharedPtr<FApiObjectMetadata> InMetadata) {
				if (ObjectDetailWidget) {
					ObjectDetailWidget->SetMetadata(InMetadata);
				}

				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Push(LOCTEXT("ObjectDetails", "Object details"));
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToObjectDetail();
			});
		}

		if (!OnObjectCreateButtonClickedDelegateHandle.IsValid()) {
			OnObjectCreateButtonClickedDelegateHandle = ObjectListWidget->OnObjectCreateButtonClicked.AddWeakLambda(this, [this] {
				if (ObjectFormWidget) {
					ObjectFormWidget->SetMetadata(nullptr);

					if (FVeUIModule* UIModule = FVeUIModule::Get()) {
						if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
							{
								BreadcrumbSubsystem->Push(
									LOCTEXT("CreateObject", "Create Object"));
							}
						} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
					} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

					NavigateToObjectForm();
				}
			});
		}
	}

	if (ObjectDetailWidget) {
		if (!OnObjectDetailWidgetClosedDelegateHandle.IsValid()) {
			OnObjectDetailWidgetClosedDelegateHandle = ObjectDetailWidget->OnClosed.AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToObjectList();
			});
		}

		if (!OnObjectEditButtonClickedDelegateHandle.IsValid()) {
			OnObjectEditButtonClickedDelegateHandle = ObjectDetailWidget->OnEditButtonClicked.AddWeakLambda(this, [this](TSharedPtr<FApiObjectMetadata> InMetadata) {
				if (ObjectFormWidget) {
					ObjectFormWidget->SetMetadata(InMetadata);

					if (FVeUIModule* UIModule = FVeUIModule::Get()) {
						if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
							{
								BreadcrumbSubsystem->Push(LOCTEXT("ObjectEdit", "Edit Object"));
							}
						} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
					} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

					NavigateToObjectForm();
				}
			});
		}

		if (!OnObjectDeleteButtonClickedDelegateHandle.IsValid()) {
			OnObjectDeleteButtonClickedDelegateHandle = ObjectDetailWidget->OnDeleteButtonClicked.AddWeakLambda(this, [this]() {
				NavigateToObjectList();

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

	if (ObjectFormWidget) {
		if (!OnObjectFormClosedDelegateHandle.IsValid()) {
			OnObjectFormClosedDelegateHandle = ObjectFormWidget->OnClosedDelegate.AddWeakLambda(this, [this](const TSharedPtr<FApiObjectMetadata> InMetadata) {
				if (InMetadata.IsValid() && InMetadata->Id.IsValid()) {
					if (ObjectDetailWidget) {
						ObjectDetailWidget->SetMetadata(InMetadata);
					}
					NavigateToObjectDetail();
				} else {
					NavigateToObjectList();
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

void UUIObjectBrowserWidget::UnregisterCallbacks() {
	if (ObjectListWidget) {
		if (OnObjectSelectedDelegateHandle.IsValid()) {
			ObjectListWidget->OnObjectSelected.Remove(OnObjectSelectedDelegateHandle);
			OnObjectSelectedDelegateHandle.Reset();
		}

		if (OnObjectCreateButtonClickedDelegateHandle.IsValid()) {
			ObjectListWidget->OnObjectCreateButtonClicked.Remove(OnObjectCreateButtonClickedDelegateHandle);
			OnObjectCreateButtonClickedDelegateHandle.Reset();
		}
	}

	if (ObjectDetailWidget) {
		if (OnObjectDetailWidgetClosedDelegateHandle.IsValid()) {
			ObjectDetailWidget->OnClosed.Remove(OnObjectDetailWidgetClosedDelegateHandle);
			OnObjectDetailWidgetClosedDelegateHandle.Reset();
		}

		if (OnObjectEditButtonClickedDelegateHandle.IsValid()) {
			ObjectDetailWidget->OnEditButtonClicked.Remove(OnObjectEditButtonClickedDelegateHandle);
			OnObjectEditButtonClickedDelegateHandle.Reset();
		}

		if (OnObjectDeleteButtonClickedDelegateHandle.IsValid()) {
			ObjectDetailWidget->OnDeleteButtonClicked.Remove(OnObjectDeleteButtonClickedDelegateHandle);
			OnObjectDeleteButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnObjectFormClosedDelegateHandle.IsValid()) {
		if (ObjectFormWidget) {
			ObjectFormWidget->OnClosedDelegate.Remove(OnObjectFormClosedDelegateHandle);
			OnObjectFormClosedDelegateHandle.Reset();
		}
	}

	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUIObjectBrowserWidget::ShowObjectListWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bObjectListWidgetVisible) {
		bObjectListWidgetVisible = true;
		if (ObjectListWidget) {
			// ObjectListWidget->ResetState();
			// ObjectListWidget->ResetAnimationState();
			ObjectListWidget->Show();
		}
	}
}

void UUIObjectBrowserWidget::ShowObjectDetailWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bObjectDetailWidgetVisible) {
		bObjectDetailWidgetVisible = true;
		if (ObjectDetailWidget) {
			// ObjectDetailWidget->ResetState();
			// ObjectDetailWidget->ResetAnimationState();
			ObjectDetailWidget->Show();
		}
	}
}

void UUIObjectBrowserWidget::ShowObjectFormWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bObjectFormWidgetVisible) {
		bObjectFormWidgetVisible = true;
		if (ObjectFormWidget) {
			// ObjectFormWidget->ResetState();
			// ObjectFormWidget->ResetAnimationState();
			ObjectFormWidget->Show();
		}
	}
}

void UUIObjectBrowserWidget::HideObjectListWidget(const bool bAnimate, const bool bPlaySound) {
	if (bObjectListWidgetVisible) {
		bObjectListWidgetVisible = false;
		if (ObjectListWidget) {
			ObjectListWidget->Hide();
		}
	}
}

void UUIObjectBrowserWidget::HideObjectDetailWidget(const bool bAnimate, const bool bPlaySound) {
	if (bObjectDetailWidgetVisible) {
		bObjectDetailWidgetVisible = false;
		if (ObjectDetailWidget) {
			ObjectDetailWidget->Hide();
		}
	}
}

void UUIObjectBrowserWidget::HideObjectFormWidget(const bool bAnimate, const bool bPlaySound) {
	if (bObjectFormWidgetVisible) {
		bObjectFormWidgetVisible = false;
		if (ObjectFormWidget) {
			ObjectFormWidget->Hide();
		}
	}
}

void UUIObjectBrowserWidget::NavigateToObjectList(const bool bAnimate, const bool bPlaySound) {
	ShowObjectListWidget();
	HideObjectDetailWidget();
	HideObjectFormWidget();
}

void UUIObjectBrowserWidget::NavigateToObjectDetail(const bool bAnimate, const bool bPlaySound) {
	HideObjectListWidget();
	ShowObjectDetailWidget();
	HideObjectFormWidget();
}

void UUIObjectBrowserWidget::NavigateToObjectForm(const bool bAnimate, const bool bPlaySound) {
	HideObjectListWidget();
	HideObjectDetailWidget();
	ShowObjectFormWidget();
}

#undef LOCTEXT_NAMESPACE
