// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPageContentWidget.h"
#include "ApiPackageMetadataObject.h"
#include "Api2PackageSubsystem.h"
#include "UIMetaverseListPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIMetaverseListPageWidget final : public UUIPageContentWidget {
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
	virtual void NativeOnRefresh() override;

	/** Request metadata for MetaverseList */
	FApiPackageIndexRequestMetadata RequestMetadata;

	void MetadataRequest(const FApiPackageIndexRequestMetadata& InRequestMetadata);
	void OnMetadataResponse(const FApiPackageBatchMetadata& InMetadata, bool bSuccessful, const FString& Error);
	void SetIsProcessing(bool bInIsLoading) const;

};
