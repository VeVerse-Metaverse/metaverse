// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIPlaceableArtObjectItemPropertiesWidget.h"

#include "ApiObjectMetadataObject.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "VeShared.h"
#include "Components/UILoadingWidget.h"
#include "Components/UIImageWidget.h"
#include "AnimatedTexture2D.h"
#include "Components/Border.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIPlaceableArtObjectItemPropertiesWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	MainBorderWidget->SetBrushColor(NormalColor);
}

void UUIPlaceableArtObjectItemPropertiesWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	const auto* ObjectObject = Cast<UApiObjectMetadataObject>(ListItemObject);
	if (!ObjectObject) {
		return;
	}
	
	Metadata = ObjectObject->Metadata;

	if (NameTextWidget) {
		if (Metadata->Name.IsEmpty()) {
			NameTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectNameUnknown", "Unnamed"));
		} else {
			NameTextWidget->SetText(FText::FromString(TruncateString(Metadata->Name, 16)));
		}
	}

	// PreviewImage
	if (ImageWidget) {
		ImageWidget->ShowImage(Metadata->Files, EApiFileType::ImagePreview);
	}

	const FLinearColor Color = (UObject::IsSelected()) ? SelectedColor : NormalColor;
	MainBorderWidget->SetBrushColor(Color);
}

void UUIPlaceableArtObjectItemPropertiesWidget::NativeOnItemSelectionChanged(bool bIsSelected) {
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	const FLinearColor Color = (bIsSelected) ? SelectedColor : NormalColor;
	MainBorderWidget->SetBrushColor(Color);
}


#undef LOCTEXT_NAMESPACE
