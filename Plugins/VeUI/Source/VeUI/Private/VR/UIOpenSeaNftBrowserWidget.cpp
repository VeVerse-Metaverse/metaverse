// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIOpenSeaNftBrowserWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VeApi.h"
#include "VeOpenSea/VeOpenSeaModule.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIOpenSeaNftBrowserWidget::NativeConstruct() {
	bOpenSeaQrCodeVisible = true;
	bOpenSeaWidgetVisible = true;
	bOpenSeaDetailWidgetVisible = true;

	NavigateToBrowser(false, false);
	
	Super::NativeConstruct();
}

void UUIOpenSeaNftBrowserWidget::RegisterCallbacks() {
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

	if (IsValid(OpenSeaWidget)) {
		if (!OnOpenSeaQrCodeCloseButtonClickedDelegateHandle.IsValid()) {
			OnOpenSeaQrCodeCloseButtonClickedDelegateHandle = OpenSeaWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToBrowser();
			});
		}
	}

	if (IsValid(OpenSeaWidget)) {
		if (!OnOpenSeaCloseButtonClickedDelegateHandle.IsValid()) {
			OnOpenSeaCloseButtonClickedDelegateHandle = OpenSeaWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				NavigateToBrowser();
			});
		}
	}

	if (IsValid(OpenSeaNftListWidget) && IsValid(OpenSeaQrCodeWidget))
	{
		OpenSeaNftListWidget->OnGetEthAddressFailed.AddUObject(this, &UUIOpenSeaNftBrowserWidget::OnGetEthAddressFailed);
	}
}

void UUIOpenSeaNftBrowserWidget::UnregisterCallbacks() {

	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnOpenSeaQrCodeCloseButtonClickedDelegateHandle.IsValid()) {
		if (OpenSeaQrCodeWidget) {
			OpenSeaQrCodeWidget->OnClosedDelegate.Remove(OnOpenSeaQrCodeCloseButtonClickedDelegateHandle);
			OnOpenSeaQrCodeCloseButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnOpenSeaCloseButtonClickedDelegateHandle.IsValid()) {
		if (OpenSeaWidget) {
			OpenSeaWidget->OnClosedDelegate.Remove(OnOpenSeaCloseButtonClickedDelegateHandle);
			OnOpenSeaCloseButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUIOpenSeaNftBrowserWidget::OnNavigate() {
	OpenSeaNftListWidget->Refresh();

	Super::OnNavigate();
}

void UUIOpenSeaNftBrowserWidget::OnGetEthAddressFailed()
{
	return NavigateToOpenSeaQrCode();	
}

void UUIOpenSeaNftBrowserWidget::ShowOpenSeaQrCodeWidget(bool bAnimate, bool bPlaySound) {
	if (!bOpenSeaQrCodeVisible) {
		bOpenSeaQrCodeVisible = true;
		if (OpenSeaQrCodeWidget) {
			// OpenSeaQrCodeWidget->ResetState();
			// OpenSeaQrCodeWidget->ResetAnimationState();
			OpenSeaQrCodeWidget->Show();
		}
	}
}

void UUIOpenSeaNftBrowserWidget::ShowOpenSeaWidget(bool bAnimate, bool bPlaySound) {
	if (!bOpenSeaWidgetVisible) {
		bOpenSeaWidgetVisible = true;
		if (OpenSeaWidget) {
			// OpenSeaWidget->ResetState();
			// OpenSeaWidget->ResetAnimationState();
			OpenSeaWidget->Show();
		}
	}
}

void UUIOpenSeaNftBrowserWidget::HideOpenSeaQrCodeWidget(bool bAnimate, bool bPlaySound) {
	if (bOpenSeaQrCodeVisible) {
		bOpenSeaQrCodeVisible = false;
		if (OpenSeaQrCodeWidget) {
			OpenSeaQrCodeWidget->Hide();
		}
	}
}

void UUIOpenSeaNftBrowserWidget::HideOpenSeaWidget(bool bAnimate, bool bPlaySound) {
	if (bOpenSeaWidgetVisible) {
		bOpenSeaWidgetVisible = false;
		if (OpenSeaWidget) {
			OpenSeaWidget->Hide();
		}
	}
}

void UUIOpenSeaNftBrowserWidget::HideOpenSeaNFTListWidget(bool bAnimate, bool bPlaySound)
{
	if (OpenSeaNftListWidget && OpenSeaNftListWidget->IsVisible())
	{
		OpenSeaNftListWidget->Hide();
	}
}

void UUIOpenSeaNftBrowserWidget::NavigateToOpenSeaQrCode(bool bAnimate, bool bPlaySound) {
	ShowOpenSeaQrCodeWidget();
	HideOpenSeaWidget();
	HideOpenSeaNFTListWidget();
}

void UUIOpenSeaNftBrowserWidget::NavigateToOpenSea(bool bAnimate, bool bPlaySound) {
	HideOpenSeaQrCodeWidget();
	ShowOpenSeaWidget();
}

void UUIOpenSeaNftBrowserWidget::NavigateToBrowser(bool bAnimate, bool bPlaySound) {
	HideOpenSeaWidget();
	HideOpenSeaQrCodeWidget();
}


#undef LOCTEXT_NAMESPACE
