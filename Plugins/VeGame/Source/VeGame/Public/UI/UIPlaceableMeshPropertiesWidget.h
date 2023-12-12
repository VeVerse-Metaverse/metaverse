// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "UI/UIPlaceablePropertiesWidget.h"
#include "VePlaceableMetadata.h"
#include "UIPlaceableMeshPropertiesWidget.generated.h"

class UComboBoxString;
struct FVePlaceableMetadata;

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIPlaceableMeshPropertiesWidget : public UUIPlaceablePropertiesWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UComboBoxString* MeshComboBoxWidget;

#pragma endregion

	FString GetMeshType() const; 
	
protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY()
	FVePlaceableMetadata PlaceableMetadata;

	void SetMetadata(const FVePlaceableMetadata& InMetadata);

	UFUNCTION()
	void MeshComboBox_OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
};
