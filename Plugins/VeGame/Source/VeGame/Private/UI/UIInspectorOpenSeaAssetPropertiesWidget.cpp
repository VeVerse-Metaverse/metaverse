// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIInspectorOpenSeaAssetPropertiesWidget.h"

#include "UI/UIInspectorArtObjectPropertiesWidget.h"

#include "ApiObjectMetadata.h"
#include "ApiOpenSeaMetadata.h"
#include "Components/UITextPropertyWidget.h"

#define LOCTEXT_NAMESPACE "VeGame"


void UUIInspectorOpenSeaAssetPropertiesWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
}

void UUIInspectorOpenSeaAssetPropertiesWidget::SetProperties(const FApiOpenSeaAssetMetadata& InMetadata) {
	if (NameWidget) {
		NameWidget->SetText(FText::FromString(InMetadata.AssetName));
	}

	if (DescriptionWidget) {
		DescriptionWidget->SetText(FText::FromString(InMetadata.AssetDescription));
	}

	if (OwnerTextPropertyWidget) {
		if (InMetadata.Owner.IsEmpty()) {
			OwnerTextPropertyWidget->SetVisible(false);	
		} else {
			OwnerTextPropertyWidget->SetVisible(true);
			OwnerTextPropertyWidget->Value = FText::FromString(InMetadata.Owner);
		}
	}

	if (OpenPageButtonWidget) {
		OpenPageButtonWidget->URL = InMetadata.Permalink;
	}
}


#undef LOCTEXT_NAMESPACE
