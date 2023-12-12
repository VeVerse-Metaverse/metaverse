// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "Api2ObjectSubsystem.h"
#include "UIPageContentWidget.h"
#include "ApiObjectMetadataObject.h"
#include "UIObjectListPageWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIObjectListPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUICommonPaginationWidget* PaginationWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUILoadingWidget* LoadingWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUICommonFilterWidget* FilterWidget;
	
#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* CreateButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* RefreshButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Detail Page"))
	TSubclassOf<UUIPageContentWidget> DetailPageClass;

	/** Form Page Class for create a new object. */
	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Form Page"))
	TSubclassOf<class UUIObjectFormPageWidget> FormPageClass;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;

	DECLARE_EVENT_OneParam(UUIObjectListPageWidget, FOnObjectSelected, TSharedPtr<FApiObjectMetadata>);
	FOnObjectSelected OnObjectSelected;

	DECLARE_EVENT_OneParam(UUIObjectListPageWidget, FOnObjectListItemClicked, UApiObjectMetadataObject*);
	FOnObjectListItemClicked OnObjectListItemClicked;
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnRefresh() override;

	/** Request metadata for ObjectList */
	IApiBatchQueryRequestMetadata RequestMetadata;
	
	void MetadataRequest(const IApiBatchQueryRequestMetadata& InRequestMetadata = FApiObjectIndexRequestMetadata());
	void OnMetadataResponse(const FApiObjectBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& Error);
	void SetIsProcessing(bool bInIsLoading) const;
	
};
