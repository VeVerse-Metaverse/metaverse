// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiOpenSeaMetadata.h"
#include "UIPageContentWidget.h"
#include "OpenSeaAssetMetadataObject.h"
#include "UIOpenSeaListPageWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIOpenSeaListPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUICommonPaginationWidget* PaginationWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUILoadingWidget* LoadingWidget;

	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// class UUICommonFilterWidget* FilterWidget;

	// /** Show only current user assets filter checkbox. */
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	// class UCheckBox* FilterCheckBoxWidget;

#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* RefreshButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Detail Page"))
	TSubclassOf<UUIPageContentWidget> DetailPageClass;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;

	DECLARE_EVENT_OneParam(UUIOpenSeaListPageWidget, FOnOpenSeaAssetSelected, const FApiOpenSeaAssetMetadata&);
	FOnOpenSeaAssetSelected OnObjectSelected;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUICommonFilterWidget* FilterWidget;

	// DECLARE_EVENT_OneParam(UUIOpenSeaListPageWidget, FOnOpenSeaAssetListItemClicked, UOpenSeaAssetMetadataObject*);
	// FOnOpenSeaAssetListItemClicked OnObjectListItemClicked;

protected:
	virtual void NativeOnInitialized() override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeOnRefresh() override;

	/** Request metadata for ObjectList */
	FOpenSeaAssetBatchRequestMetadata RequestMetadata;
	FString CurrentUserEthAddress;

	void RequestOpenSeaAssets(const FOpenSeaAssetBatchRequestMetadata& InRequestMetadata);
	void OnOpenSeaAssetsReceived(const FApiOpenSeaAssetBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error);

	/** Set processing state to lock the whole widget with the loader overlay. */
	void SetIsProcessing(bool bInIsProcessing) const;

	// /** Callback for show only current user filter checkbox. */
	// UFUNCTION()
	// void OnFilterCheckStateChangedCallback(bool bIsChecked);

private:
	/** Container asset click delegate handle. */
	FDelegateHandle OnContainerItemClickDelegateHandle;

	/** Page change delegate handle. */
	FDelegateHandle OnPaginationPageChangeDelegateHandle;

	/** Refresh button click delegate handle. */
	FDelegateHandle OnRefreshButtonClickDelegateHandle;
};
