// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiOpenSeaMetadata.h"
#include "OpenSeaAssetMetadata.h"
#include "UI/UIPlaceablePropertiesWidget.h"

#include "UIEditorOpenSeaAssetsPropertiesWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIEditorOpenSeaAssetsPropertiesWidget final : public UUIPlaceablePropertiesWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUICommonPaginationWidget* PaginationWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUICommonFilterWidget* FilterWidget;

	/** Show only current user assets filter checkbox. */
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	// class UCheckBox* FilterCheckBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* RefreshButtonWidget;

#pragma endregion Widgets

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;

	void SetEntityId(const FGuid& InEntityId);
	FGuid GetEntityId() const;
	FApiOpenSeaAssetMetadata GetMetadata() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	/** Request metadata for ObjectList */
	FOpenSeaAssetBatchRequestMetadata RequestMetadata;

	FGuid SelectedItem;
	void UpdateSelection();

	void NativeOnRefresh();
	void MetadataRequest(const FOpenSeaAssetBatchRequestMetadata& InRequestMetadata);
	void OnMetadataResponse(const FApiOpenSeaAssetBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error);

private:
	bool LockChanged = false;

};
