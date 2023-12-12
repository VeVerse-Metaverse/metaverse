// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiSpaceMetadataObject.h"
#include "Components/UILoadingWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "UISpaceBrowserWidget.h"
#include "UISpaceDetailWidget.h"
#include "UISpaceFilterWidget.h"
#include "UISpaceListItemWidget.h"
#include "UIWidget.h"
#include "Components/TileView.h"
#include "Api2WorldSubsystem.h"
#include "UISpaceListWidget.generated.h"


class FApiUserMetadata;
/**
 * 
 */
UCLASS()
class VEUI_API UUISpaceListWidget final : public UUIWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	DECLARE_EVENT_OneParam(UUISpaceListWidget, FOnSpaceListItemClicked, UApiSpaceMetadataObject*);
	FOnSpaceListItemClicked OnSpaceListItemClicked;

	void GetSpaces(const FApiWorldIndexRequestMetadata& RequestMetadata);

	void SetSpaces(const FApi2WorldBatchMetadata& BatchMetadata);

	/** Auth service user login callback. */
	void OnUserLogin(const FApiUserMetadata&, const bool bSuccessful, const FString& Error);

	/** Space tile view item clicked callback. */
	void OnSpaceItemClicked(UApiSpaceMetadataObject* InSpace);

	void OnSpaceFormButtonClicked();
	
	/** Space batch loaded callback. */
	void OnSpacesLoaded(const FApi2WorldBatchMetadata& InMetadata, bool bInSuccessful, const FString& InError);

	/** Pagination page changed callback. */
	void OnPageChanged(int32 InCurrentPage, int32 InItemsPerPage);

	/** Filter widget filter change callback. */
	void OnFilterChanged(const FString& InQuery);
	
	FDelegateHandle OnSpaceBatchLoadedDelegateHandle;
	FDelegateHandle OnSpaceListItemClickedDelegateHandle;
	FDelegateHandle OnSpaceDetailWidgetClosedDelegateHandle;
	FDelegateHandle OnAuthSubsystemUserLoginDelegateHandle;
	FDelegateHandle OnPaginationPageChangeDelegateHandle;
	FDelegateHandle OnSpaceNameFilterChangeDelegateHandle;
	FDelegateHandle OnSpaceCreateButtonClickedDelegateHandle;
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	FDelegateHandle	OnRefreshButtonClickedDelegateHandle;

	virtual void NativeConstruct() override;

	void SetIsProcessing(bool bInIsLoading) const;
	
public:
	void Refresh();
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonPaginationWidget* PaginationWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUILoadingWidget* LoadingWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUISpaceFilterWidget* SpaceNameFilterWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CreateButtonClickedWidget;

	/** Current filter options. */
	UPROPERTY(EditAnywhere)
	FString CurrentQuery;

	/** Type of spaces to display. */
	UPROPERTY(EditAnywhere)
	FString SpaceType = "artheon";

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget|Content")
	TSubclassOf<UUISpaceListItemWidget> ListItemWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* RefreshButtonWidget;
	
	DECLARE_EVENT_OneParam(UUISpaceListWidget, FOnSpaceSelected, TSharedPtr<FApiSpaceMetadata>);
	FOnSpaceSelected OnSpaceSelected;

	FOnButtonClicked OnSpaceCreateButtonClicked;
};
