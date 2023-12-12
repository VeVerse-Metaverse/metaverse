// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiEntityMetadata.h"
#include "UIPageContentWidget.h"
#include "ApiEventMetadataObject.h"
#include "UIEventListPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIEventListPageWidget final : public UUIPageContentWidget {
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
	class UUIButtonWidget* RefreshButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Detail Page"))
	TSubclassOf<UUIPageContentWidget> DetailPageClass;

	UPROPERTY(EditAnywhere, Category="Widget")
	int32 ItemsPerPage = 12;

protected:
	virtual void NativeOnInitialized() override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeOnRefresh() override;

	/** Request metadata for EventList */
	IApiBatchQueryRequestMetadata RequestMetadata;

	void MetadataRequest(const IApiBatchQueryRequestMetadata& InRequestMetadata = IApiBatchQueryRequestMetadata());
	void OnMetadataResponse(const FApiEventBatchMetadata& InMetadata, bool bSuccessful, const FString& Error);
	void SetIsProcessing(bool bInIsLoading) const;

private:
	FDelegateHandle OnContainerItemClickedDelegateHandle;
	FDelegateHandle OnPaginationPageChangeDelegateHandle;
	FDelegateHandle OnFilterChangeDelegateHandle;
	FDelegateHandle OnRefreshButtonClickedDelegateHandle;
};
