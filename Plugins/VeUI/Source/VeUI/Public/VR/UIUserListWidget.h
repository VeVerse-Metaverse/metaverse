// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiUserMetadataObject.h"
#include "Components/UICommonPaginationWidget.h"
#include "UIUserFilterWidget.h"
#include "UIUserListItemWidget.h"
#include "UIWidget.h"
#include "Components/TileView.h"
#include "UIUserListWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIUserListWidget final : public UUIWidget {
	GENERATED_BODY()

	FDelegateHandle OnContainerItemClickedDelegateHandle;
	FDelegateHandle OnUserBatchLoadedDelegateHandle;
	FDelegateHandle OnUserListItemClickedDelegateHandle;
	FDelegateHandle OnAuthSubsystemUserLoginDelegateHandle;
	FDelegateHandle OnPaginationPageChangeDelegateHandle;
	FDelegateHandle OnUserNameFilterChangeDelegateHandle;
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	FDelegateHandle	OnRefreshButtonClickedDelegateHandle;

public:
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* RefreshButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUICommonPaginationWidget* PaginationWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUILoadingWidget* LoadingWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIUserFilterWidget* UserNameFilterWidget;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget|Content")
	TSubclassOf<UUIUserListItemWidget> ListItemWidgetClass;

	DECLARE_EVENT_OneParam(UUIUserListWidget, FOnUserSelected, TSharedPtr<FApiUserMetadata>);

	FOnUserSelected OnUserSelected;

	DECLARE_EVENT_OneParam(UUIUserListWidget, FOnUserListItemClicked, UApiUserMetadataObject*);

	FOnUserListItemClicked OnUserListItemClicked;

	FString CurrentQuery;

	void Refresh();

protected:
	
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeConstruct() override;

	void RequestUsers(const IApiBatchQueryRequestMetadata& RequestMetadata = IApiBatchQueryRequestMetadata());
	void SetIsProcessing(bool bInIsLoading) const;
	void SetUsers(const FApiUserBatchMetadata& BatchMetadata);
	void OnUsersLoaded(const FApiUserBatchMetadata& Metadata, bool bSuccessful, const FString& Error);
	
};
