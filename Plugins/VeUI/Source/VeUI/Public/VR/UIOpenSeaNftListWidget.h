// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiObjectMetadataObject.h"
#include "OpenSeaAssetMetadata.h"
#include "OpenSeaAssetMetadataObject.h"
#include "Components/UIInputWidget.h"
#include "Components/UILoadingWidget.h"
#include "UIOpenSeaNftFilterWidget.h"
#include "UIOpenSeaNftListItemWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "UIWidgetBase.h"
#include "UIObjectDetailWidget.h"
#include "Components/TileView.h"
#include "UIOpenSeaNftListWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIOpenSeaNftListWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	void GetOpenSeaAssets(const FOpenSeaAssetBatchRequestMetadata& InRequestMetadata);

	//void GetObjects(const IApiBatchQueryRequestMetadata& RequestMetadata = IApiBatchQueryRequestMetadata());

	void OnObjectFormButtonClicked() const;
	void OnOpenSeaNftClicked(TSharedPtr<FApiObjectMetadata> Metadata);
	
	FDelegateHandle OnOpenSeaNftBatchLoadedDelegateHandle;
	FDelegateHandle OnOpenSeaNftListItemClickedDelegateHandle;
	FDelegateHandle OnOpenSeaNftDetailWidgetClosedDelegateHandle;
	FDelegateHandle OnAuthSubsystemUserLoginDelegateHandle;
	FDelegateHandle OnPaginationPageChangeDelegateHandle;
	FDelegateHandle OnOpenSeaNftQueryFilterChangeDelegateHandle;
	FDelegateHandle OnOpenSeaNftCreateButtonClickedDelegateHandle;
	FDelegateHandle	OnRefreshButtonClickedDelegateHandle;

	virtual void NativeConstruct() override;

	void SetIsProcessing(bool bInIsLoading) const;

public:
	DECLARE_MULTICAST_DELEGATE(FOnGetEthAddressFailed);
	FOnGetEthAddressFailed OnGetEthAddressFailed;
	
	void Refresh();
	
	void OnAssetsLoaded(const FOpenSeaAssetBatchMetadata& InMetadata, bool bInSuccessful, const FString& String);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUICommonPaginationWidget* PaginationWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUILoadingWidget* LoadingWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIOpenSeaNftFilterWidget* OpenSeaNftFilterWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CreateButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* RefreshButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIObjectDetailWidget* DetailWidget;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;

	DECLARE_EVENT_OneParam(UUIOpenSeaNftListWidget, FOnOpenSeaNftSelected, TSharedPtr<FApiObjectMetadata>);
	FOnOpenSeaNftSelected OnOpenSeaNftSelected;
	
	FOnButtonClicked OnObjectCreateButtonClicked;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget|Content")
	TSubclassOf<UUIOpenSeaNftListItemWidget> ListItemWidgetClass;

	DECLARE_EVENT_OneParam(UUIOpenSeaNftListWidget, FOnOpenSeaNftListItemClicked, UOpenSeaAssetMetadataObject*);
	FOnOpenSeaNftListItemClicked OnOpenSeaNftListItemClicked;

	FString CurrentQuery;
};
