// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "Api2WorldSubsystem.h"
#include "UIPageContentWidget.h"
#include "ApiSpaceMetadataObject.h"
#include "UISpaceListPageWidget.generated.h"

enum class ESortType : uint8 {
	Content UMETA(DisplayName = "Content"),
	View UMETA(DisplayName = "View"),
	Like UMETA(DisplayName = "Like"),
};

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUISpaceListPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UUISpaceListPageWidget, FOnSpaceListItemClicked, UApiSpaceMetadataObject*);

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

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* CreateButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* SortContentButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* SortViewButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* SortLikeButtonWidget;

#pragma endregion Buttons
	
	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Detail Page"))
	TSubclassOf<UUIPageContentWidget> DetailPageClass;

	/** Form Page Class for create a new space. */
	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Form Page"))
	TSubclassOf<class UUISpaceFormPageWidget> FormPageClass;
	
	/** Type of spaces to display. */
	UPROPERTY(EditAnywhere)
	FString SpaceType = "artheon";

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;

	bool bSortContent = false;
	bool bSortView = false;
	bool bSortLike = false;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnRefresh() override;

	/** Request metadata for SpaceList */
	FApi2BatchSortRequestMetadata RequestMetadata;

	void MetadataRequest(const FApi2BatchSortRequestMetadata& InRequestMetadata = FApi2BatchSortRequestMetadata());
	void OnMetadataResponse(const FApi2WorldBatchMetadata& InMetadata, bool bInSuccessful, const FString& InError);
	void SetIsProcessing(bool bInIsLoading) const;

	void SortValidation(const ESortType SortType);
	
};
