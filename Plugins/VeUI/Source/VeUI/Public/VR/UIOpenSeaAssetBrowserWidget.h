// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIBreadcrumbTrailWidget.h"
#include "UIObjectFormWidget.h"
#include "UIFileBrowserWidget.h"
#include "UIOpenSeaQrCodeWidget.h"
#include "UIWidgetBase.h"

#include "UIOpenSeaAssetBrowserWidget.generated.h"

class UUIOpenSeaAssetDetailWidget;
class UUIOpenSeaAssetListWidget;
class UUIObjectListWidget;
class UUIObjectDetailWidget;

/**
 * 
 */
UCLASS()
class VEUI_API UUIOpenSeaAssetBrowserWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnObjectDetailWidgetClosedDelegateHandle;
	FDelegateHandle OnObjectSelectedDelegateHandle;
	FDelegateHandle OnObjectCreateButtonClickedDelegateHandle;
	FDelegateHandle OnObjectEditButtonClickedDelegateHandle;
	FDelegateHandle OnUserEthAddressEmptyDelegateHandle;
	FDelegateHandle OnQrCodeButtonCLickedDelegateHandle;
	FDelegateHandle OnObjectDeleteButtonClickedDelegateHandle;
	FDelegateHandle OnObjectFormClosedDelegateHandle;
	FDelegateHandle OnObjectBreadcrumbNavigationDelegateHandle;
	FDelegateHandle OnCloseButtonClickedDelegateHandle;


public:
	
	virtual void OnNavigate() override;
	
	virtual void NativeConstruct() override;
	
	/** List of object. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIOpenSeaAssetListWidget* AssetListWidget;
	
	/** Object details. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIOpenSeaAssetDetailWidget* AssetDetailWidget;
	
	/** Object details. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIOpenSeaQrCodeWidget* OpenSeaQrCodeWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIBreadcrumbTrailWidget* BreadcrumbTrailWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);
	FOnClosed OnClosedDelegate;

#pragma region Navigation
private:
	bool bAssetListWidgetVisible;
	bool bAssetDetailWidgetVisible;
	bool bOpenSeaQrCodeWidgetVisible;

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowAssetListWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowAssetDetailWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowOpenSeaQrCodeWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideAssetListWidget(bool bAnimate = true, bool bPlaySound = true);
	
	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideOpenSeaQrCodeWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideAssetDetailWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToAssetList(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToAssetDetail(bool bAnimate = true, bool bPlaySound = true);
	
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToOpenSeaQrCode(bool bAnimate = true, bool bPlaySound = true);

#pragma endregion
};
