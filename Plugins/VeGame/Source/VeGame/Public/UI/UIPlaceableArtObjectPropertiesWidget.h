// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "UI/UIPlaceablePropertiesWidget.h"
#include "ApiEntityMetadata.h"
#include "ApiObjectMetadataObject.h"
#include "UIPlaceableArtObjectPropertiesWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIPlaceableArtObjectPropertiesWidget : public UUIPlaceablePropertiesWidget {
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
	FApiObjectMetadata GetMetadata() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	/** Request metadata for ObjectList */
	IApiBatchQueryRequestMetadata RequestMetadata;

	FGuid SelectedItem;
	void UpdateSelection();

	void NativeOnRefresh();
	void MetadataRequest(const IApiBatchQueryRequestMetadata& InRequestMetadata = IApiBatchQueryRequestMetadata());
	void OnMetadataResponse(const FApiObjectBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error);

private:
	bool LockChanged = false;

};
