// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIPlaceableMeshPropertiesWidget.h"

#include "Components/ComboBoxString.h"

void UUIPlaceableMeshPropertiesWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(MeshComboBoxWidget)) {
		MeshComboBoxWidget->OnSelectionChanged.AddDynamic(this, &UUIPlaceableMeshPropertiesWidget::MeshComboBox_OnSelectionChanged);
	}

	SetSaveEnabled(true);
}

void UUIPlaceableMeshPropertiesWidget::SetMetadata(const FVePlaceableMetadata& InMetadata) {
	PlaceableMetadata = InMetadata;

	const FVePropertyMetadata* FoundProperty = InMetadata.Properties.FindByPredicate([=](const FVePropertyMetadata& InProperty) {
		return InProperty.Name == TEXT("mesh") && InProperty.Type == EVePropertyType::String ? true : false;
	});

	if (FoundProperty) {
		MeshComboBoxWidget->SetSelectedOption(FoundProperty->Value);
	} else {
		MeshComboBoxWidget->SetSelectedIndex(0);
	}
}

FString UUIPlaceableMeshPropertiesWidget::GetMeshType() const {
	return MeshComboBoxWidget->GetSelectedOption();
}

void UUIPlaceableMeshPropertiesWidget::MeshComboBox_OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) { }
