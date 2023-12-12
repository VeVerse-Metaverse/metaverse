// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIButtonWidget.h"
#include "UIOpenSeaNftListWidget.h"
#include "UIOpenSeaWidget.h"
#include "UIOpenSeaQrCodeWidget.h"
#include "UIWidgetBase.h"

#include "UIOpenSeaNftBrowserWidget.generated.h"

class FOpenSeaAssetBatchRequestMetadata;
UCLASS()
class VEUI_API UUIOpenSeaNftBrowserWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void OnNavigate() override;
	
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	FDelegateHandle OnOpenSeaQrCodeCloseButtonClickedDelegateHandle;
	FDelegateHandle OnOpenSeaCloseButtonClickedDelegateHandle;
	
	/** List of object. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIOpenSeaNftListWidget* OpenSeaNftListWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CloseButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIOpenSeaQrCodeWidget* OpenSeaQrCodeWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIOpenSeaWidget* OpenSeaWidget;

	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);

	FOnClosed OnClosedDelegate;
//void GetMods(const IApiBatchQueryRequestMetadata& RequestMetadata = IApiBatchQueryRequestMetadata()) const;

protected:
	void OnGetEthAddressFailed();
	
#pragma region Navigation
private:
	bool bOpenSeaQrCodeVisible;
	bool bOpenSeaWidgetVisible;
	bool bOpenSeaDetailWidgetVisible;

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowOpenSeaQrCodeWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowOpenSeaWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideOpenSeaQrCodeWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideOpenSeaWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the NFT list widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideOpenSeaNFTListWidget(bool bAnimate = true, bool bPlaySound = true);	

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToOpenSeaQrCode(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToOpenSea(bool bAnimate = true, bool bPlaySound = true);
	//
	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void NavigateToDetailOpenSea(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToBrowser(bool bAnimate = true, bool bPlaySound = true);

#pragma endregion
};
