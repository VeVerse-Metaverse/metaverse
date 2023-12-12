// Author: Khusan Yadgarov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "Api2SharedAccessSubsystem.h"
#include "UIPageContentWidget.h"
#include "ApiSharedAccessMetadataObject.h"
#include "UISharedAccessListPageWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUISharedAccessListPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UUISharedAccessListPageWidget, FOnSharedAccessListItemClicked, UApiSharedAccessMetadataObject*);

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
	class UUIButtonWidget* RefreshButtonWidget;
#pragma endregion Buttons
	//
	// UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Detail Page"))
	// TSubclassOf<UUIPageContentWidget> DetailPageClass;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;


protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnRefresh() override;

	/** Request metadata for SharedAccessList */
	FApi2SharedAccessBatchRequestMetadata RequestMetadata;

	void MetadataRequest(const FApi2SharedAccessBatchRequestMetadata& InRequestMetadata = FApi2SharedAccessBatchRequestMetadata());
	void OnMetadataResponse(const FApi2SharedAccessBatchMetadata& InMetadata, bool bInSuccessful, const FString& InError);
	void SetIsProcessing(bool bInIsLoading) const;

};
