// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiObjectMetadataObject.h"
#include "Components/UIInputWidget.h"
#include "Components/UILoadingWidget.h"
#include "UIObjectFilterWidget.h"
#include "UIObjectListItemWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "UIWidgetBase.h"

#include "Components/TileView.h"
#include "UIObjectListWidget.generated.h"


/**
 * 
 */
UCLASS()
class VEUI_API UUIObjectListWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	void GetObjects(const IApiBatchQueryRequestMetadata& RequestMetadata = IApiBatchQueryRequestMetadata());

	void OnObjectFormButtonClicked() const;
	
	FDelegateHandle OnObjectBatchLoadedDelegateHandle;
	FDelegateHandle OnObjectListItemClickedDelegateHandle;
	FDelegateHandle OnObjectDetailWidgetClosedDelegateHandle;
	FDelegateHandle OnAuthSubsystemUserLoginDelegateHandle;
	FDelegateHandle OnPaginationPageChangeDelegateHandle;
	FDelegateHandle OnObjectQueryFilterChangeDelegateHandle;
	FDelegateHandle OnObjectCreateButtonClickedDelegateHandle;
	FDelegateHandle	OnRefreshButtonClickedDelegateHandle;

	virtual void NativeConstruct() override;

	void SetIsProcessing(bool bInIsLoading) const;


public:
	void Refresh();
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUICommonPaginationWidget* PaginationWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUILoadingWidget* LoadingWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIObjectFilterWidget* ObjectQueryFilterWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CreateButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* RefreshButtonWidget;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;

	DECLARE_EVENT_OneParam(UUIObjectListWidget, FOnObjectSelected, TSharedPtr<FApiObjectMetadata>);
	FOnObjectSelected OnObjectSelected;
	
	FOnButtonClicked OnObjectCreateButtonClicked;

	void SetObjects(const FApiObjectBatchMetadata& BatchMetadata);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget|Content")
	TSubclassOf<UUIObjectListItemWidget> ListItemWidgetClass;

	DECLARE_EVENT_OneParam(UUIObjectListWidget, FOnObjectListItemClicked, UApiObjectMetadataObject*);
	FOnObjectListItemClicked OnObjectListItemClicked;

	FString CurrentQuery;

	void OnObjectsLoaded(const FApiObjectBatchMetadata& Metadata, const bool bSuccessful, const FString& Error);
};
