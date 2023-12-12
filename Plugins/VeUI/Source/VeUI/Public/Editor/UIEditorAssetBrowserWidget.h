#pragma once
#include "Api2PlaceableMetadataObject.h"
#include "Api2PlaceableSubsystem.h"
#include "UIEditorAssetBrowserItemWidget.h"
#include "UIWidgetBase.h"
#include "Components/ListView.h"
#include "Components/UICommonPaginationWidget.h"
#include "UIEditorAssetBrowserWidget.generated.h"

UCLASS()
class VEUI_API UUIEditorAssetBrowserWidget final : public UUIWidgetBase {
	GENERATED_BODY()

public:
#pragma region Widgets
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUICommonFilterWidget* FilterWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UComboBoxString* CategoryDropDown;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUILoadingWidget* LoadingWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UListView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUICommonPaginationWidget* PaginationWidget;

#pragma endregion Widgets

	void Refresh();
	
protected:
	virtual void NativeOnInitialized() override;

	FApi2PlaceableClassBatchQueryRequestMetadata RequestMetadata;

	TSubclassOf<UUIEditorAssetBrowserItemWidget> ListItemWidgetClass;
	
	void NativeOnRefresh();
	void MetadataRequest(const FApi2PlaceableClassBatchQueryRequestMetadata& InRequestMetadata);
	void OnMetadataResponse(const FApi2PlaceableClassBatchMetadata& InMetadata, bool bInSuccessful, const FString& InError);
	void SetIsProcessing(bool bInIsLoading) const;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 100;
};
