// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIOpenSeaAssetBrowserWidget.h"

#include "VR/UIOpenSeaAssetDetailWidget.h"
#include "VR/UIOpenSeaAssetListWidget.h"
#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VeApi.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIOpenSeaAssetBrowserWidget::OnNavigate() {
	AssetListWidget->Refresh();

	Super::OnNavigate();
}

void UUIOpenSeaAssetBrowserWidget::NativeConstruct() {
	bAssetDetailWidgetVisible = true;
	bAssetListWidgetVisible = true;
	bOpenSeaQrCodeWidgetVisible = true;

	NavigateToAssetList(false, false);

	
	Super::NativeConstruct();
}

void UUIOpenSeaAssetBrowserWidget::RegisterCallbacks() {
	if (AssetListWidget) {
		if (!OnObjectSelectedDelegateHandle.IsValid()) {
			OnObjectSelectedDelegateHandle = AssetListWidget->OnObjectSelected.AddWeakLambda(this, [this](FOpenSeaAssetMetadata InMetadata) {
				if (AssetDetailWidget) {
					AssetDetailWidget->SetMetadata(InMetadata);
				}

				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Push(LOCTEXT("AssetsDetails", "Assets details"));
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToAssetDetail();
			});
		}
	}
	
	if (AssetListWidget) {
		if (!OnUserEthAddressEmptyDelegateHandle.IsValid()) {
			OnUserEthAddressEmptyDelegateHandle = AssetListWidget->OnUserEthAddressEmpty.AddWeakLambda(this, [this]() {
	
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Push(LOCTEXT("QRCode", "QR Code"));
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };
	
				NavigateToOpenSeaQrCode();
			});
		}
	}

	if (OpenSeaQrCodeWidget) {
		if (!OnQrCodeButtonCLickedDelegateHandle.IsValid()) {
			OnQrCodeButtonCLickedDelegateHandle = OpenSeaQrCodeWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToAssetList();
			});
		}
	}
	
	if (AssetDetailWidget) {
		if (!OnObjectDetailWidgetClosedDelegateHandle.IsValid()) {
			OnObjectDetailWidgetClosedDelegateHandle = AssetDetailWidget->OnClosed.AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				NavigateToAssetList();
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

void UUIOpenSeaAssetBrowserWidget::UnregisterCallbacks() {
	if (AssetListWidget) {
		if (OnObjectSelectedDelegateHandle.IsValid()) {
			AssetListWidget->OnObjectSelected.Remove(OnObjectSelectedDelegateHandle);
			OnObjectSelectedDelegateHandle.Reset();
		}

		if (OnObjectCreateButtonClickedDelegateHandle.IsValid()) {
			AssetListWidget->OnObjectCreateButtonClicked.Remove(OnObjectCreateButtonClickedDelegateHandle);
			OnObjectCreateButtonClickedDelegateHandle.Reset();
		}
		
		if (OnUserEthAddressEmptyDelegateHandle.IsValid()) {
			AssetListWidget->OnUserEthAddressEmpty.Remove(OnUserEthAddressEmptyDelegateHandle);
			OnUserEthAddressEmptyDelegateHandle.Reset();
		}
	}

	if (AssetDetailWidget) {
		if (OnObjectDetailWidgetClosedDelegateHandle.IsValid()) {
			AssetDetailWidget->OnClosed.Remove(OnObjectDetailWidgetClosedDelegateHandle);
			OnObjectDetailWidgetClosedDelegateHandle.Reset();
		}
	}

	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}
	
	if (OnQrCodeButtonCLickedDelegateHandle.IsValid()) {
		if (OpenSeaQrCodeWidget) {
			OpenSeaQrCodeWidget->OnClosedDelegate.Remove(OnQrCodeButtonCLickedDelegateHandle);
			OnQrCodeButtonCLickedDelegateHandle.Reset();
		}
	}
}

void UUIOpenSeaAssetBrowserWidget::ShowAssetListWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bAssetListWidgetVisible) {
		bAssetListWidgetVisible = true;
		if (AssetListWidget) {
			// AssetListWidget->ResetState();
			// AssetListWidget->ResetAnimationState();
			AssetListWidget->Show();
		}
	}
}

void UUIOpenSeaAssetBrowserWidget::ShowAssetDetailWidget(const bool bAnimate, const bool bPlaySound) {
	if (!bAssetDetailWidgetVisible) {
		bAssetDetailWidgetVisible = true;
		if (AssetDetailWidget) {
			// AssetDetailWidget->ResetState();
			// AssetDetailWidget->ResetAnimationState();
			AssetDetailWidget->Show();
		}
	}
}

void UUIOpenSeaAssetBrowserWidget::ShowOpenSeaQrCodeWidget(bool bAnimate, bool bPlaySound) {
	if (!bOpenSeaQrCodeWidgetVisible) {
		bOpenSeaQrCodeWidgetVisible = true;
		if (OpenSeaQrCodeWidget) {
			// OpenSeaQrCodeWidget->ResetState();
			// OpenSeaQrCodeWidget->ResetAnimationState();
			OpenSeaQrCodeWidget->Show();
		}
	}
}

void UUIOpenSeaAssetBrowserWidget::HideAssetListWidget(const bool bAnimate, const bool bPlaySound) {
	if (bAssetListWidgetVisible) {
		bAssetListWidgetVisible = false;
		if (AssetListWidget) {
			AssetListWidget->Hide();
		}
	}
}

void UUIOpenSeaAssetBrowserWidget::HideOpenSeaQrCodeWidget(bool bAnimate, bool bPlaySound) {
	if (bOpenSeaQrCodeWidgetVisible) {
		bOpenSeaQrCodeWidgetVisible = false;
		if (OpenSeaQrCodeWidget) {
			OpenSeaQrCodeWidget->Hide();
		}
	}
}

void UUIOpenSeaAssetBrowserWidget::HideAssetDetailWidget(const bool bAnimate, const bool bPlaySound) {
	if (bAssetDetailWidgetVisible) {
		bAssetDetailWidgetVisible = false;
		if (AssetDetailWidget) {
			AssetDetailWidget->Hide();
		}
	}
}

void UUIOpenSeaAssetBrowserWidget::NavigateToAssetList(const bool bAnimate, const bool bPlaySound) {
	ShowAssetListWidget();
	HideAssetDetailWidget();
	HideOpenSeaQrCodeWidget();
}

void UUIOpenSeaAssetBrowserWidget::NavigateToAssetDetail(const bool bAnimate, const bool bPlaySound) {
	HideAssetListWidget();
	ShowAssetDetailWidget();
	HideOpenSeaQrCodeWidget();

}

void UUIOpenSeaAssetBrowserWidget::NavigateToOpenSeaQrCode(bool bAnimate, bool bPlaySound) {
	HideAssetListWidget();
	HideAssetDetailWidget();
	ShowOpenSeaQrCodeWidget();
}


#undef LOCTEXT_NAMESPACE
