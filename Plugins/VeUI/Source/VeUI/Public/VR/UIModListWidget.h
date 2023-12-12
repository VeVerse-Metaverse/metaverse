// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIInputWidget.h"
#include "Components/UILoadingWidget.h"
#include "UIMetaverseFilterWidget.h"
#include "UIMetaverseListItemWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "UIWidget.h"
#include "Components/TileView.h"
#include "UIModListWidget.generated.h"


/**
 * 
 */
UCLASS()
class VEUI_API UUIModListWidget final : public UUIWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	void GetMods(const IApiBatchQueryRequestMetadata& RequestMetadata = IApiBatchQueryRequestMetadata());

	FDelegateHandle OnModBatchLoadedDelegateHandle;
	FDelegateHandle OnMetaverseListItemClickedDelegateHandle;
	FDelegateHandle OnModDetailWidgetClosedDelegateHandle;
	FDelegateHandle OnAuthSubsystemUserLoginDelegateHandle;
	FDelegateHandle OnPaginationPageChangeDelegateHandle;
	FDelegateHandle OnModNameFilterChangeDelegateHandle;
	FDelegateHandle	OnRefreshButtonClickedDelegateHandle;

	virtual void NativeConstruct() override;

	void SetIsProcessing(bool bInIsLoading) const;

public:
	void Refresh();
	void SetMetaverses(const FApiPackageBatchMetadata& BatchMetadata);
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUICommonPaginationWidget* PaginationWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* RefreshButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUILoadingWidget* LoadingWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIMetaverseFilterWidget* ModNameFilterWidget;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget|Content")
	TSubclassOf<UUIMetaverseListItemWidget> ListItemWidgetClass;

	DECLARE_EVENT_OneParam(UUIModListWidget, FOnMetaverseListItemClicked, UApiPackageMetadataObject*);
	FOnMetaverseListItemClicked OnMetaverseListItemClicked;
	
	DECLARE_EVENT_OneParam(UUIModListWidget, FOnMetaverseSelected, TSharedPtr<FApiPackageMetadata>);
	FOnMetaverseSelected OnMetaverseSelected;

	FString CurrentQuery;

	void OnModBatchLoaded(const FApiPackageBatchMetadata& Metadata, const bool bSuccessful, const FString& Error);
};
