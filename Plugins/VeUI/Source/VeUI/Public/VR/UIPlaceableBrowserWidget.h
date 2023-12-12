// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

// #include "OpenSeaAssetMetadata.h"
// #include "UIObjectFormWidget.h"
#include "UIFileBrowserWidget.h"
#include "UIPlaceableUrlWidget.h"
#include "UITabBoxWidget.h"
#include "UIWidgetBase.h"

#include "UIPlaceableBrowserWidget.generated.h"

class UUIOpenSeaAssetListWidget;
class UUIObjectListWidget;
//
// UENUM(BlueprintType)
// enum class EPlaceableType : uint8 {
// 	Url,
// 	OpenSea,
// 	Artheon
// };
//
// USTRUCT(BlueprintType)
// struct VEUI_API FPlaceableSelectedInfo {
// 	UPROPERTY()
// 	EPlaceableType Type;
//
// 	/** URL for url placeables */
// 	UPROPERTY()
// 	FString Url;
//
// 	/** Object metadata for Artheon */
// 	TSharedPtr<FApiObjectMetadata> ObjectMetadata;
//
// 	/** OpenSea asset metadata for OpenSea */
// 	FOpenSeaAssetMetadata OpenSeaAssetMetadata;
// };

/**
 * 
 */
UCLASS()
class VEUI_API UUIPlaceableBrowserWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

protected:
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	FDelegateHandle OnShowPlaceableBrowserDelegateHandle;

	FDelegateHandle OnObjectSelectedDelegateHandle;
	FDelegateHandle OnOpenSeaAssetSelectedDelegateHandle;
	FDelegateHandle OnObjectBrowserClosedDelegateHandle;
	FDelegateHandle OnPlaceableUrlWidgetDelegateHandle;

public:
	virtual void OnNavigate() override;

	virtual void NativeConstruct() override;

	/** Tab buttons. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUITabBoxWidget* TabBoxWidget = nullptr;

	/** List of object. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIObjectListWidget* ObjectListWidget = nullptr;

	/** List of NFTs. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIOpenSeaAssetListWidget* OpenSeaAssetListWidget = nullptr;

	/** List of NFTs. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIPlaceableUrlWidget* PlaceableUrlWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget = nullptr;

	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);

	FOnClosed OnClosedDelegate;

protected:
	UFUNCTION()
	void OnTabSelectedCallback(int Index, FString TabName);

#pragma region Navigation
private:
	bool bObjectListWidgetVisible = true;
	bool bOpenSeaNftListWidgetVisible = true;
	bool bPlaceableUrlWidgetVisible = true;

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowObjectListWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowOpenSeaNftListWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowPlaceableUrlWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideObjectListWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideOpenSeaNftListWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HidePlaceableUrlWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToObjectList(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToOpenSeaNftListWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToPlaceableUrlWidget(bool bAnimate = true, bool bPlaySound = true);
#pragma endregion
};
