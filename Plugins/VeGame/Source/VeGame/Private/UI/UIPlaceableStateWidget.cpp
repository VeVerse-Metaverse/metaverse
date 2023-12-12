// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIPlaceableStateWidget.h"

#include "VeShared.h"
#include "VeGameModule.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "VeGame"

void UUIPlaceableStateWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (TextWidget) {
		TextWidget->SetText(FText::FromString(TEXT("")));
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UUIPlaceableStateWidget::SetPlaceableInterface(IPlaceable* PlaceableInterface) {
	if (!TextWidget) {
		return;
	}
	PlaceableInterface->GetOnPlaceableStateChanged().AddWeakLambda(this, [=](EPlaceableState State, float Ratio) {
		switch (State) {
		case EPlaceableState::Downloading:
			SetVisibility(ESlateVisibility::Visible);
			// VeLog(">>> Downloading %s", *FText::AsPercent(Ratio).ToString());
			TextWidget->SetText(FText::Format(LOCTEXT("OnPlaceableStateChanged", "Downloading {0}"), FText::AsPercent(Ratio)));
			break;

		case EPlaceableState::Uploading:
		default:
			SetVisibility(ESlateVisibility::Collapsed);
		}

	});
}

#undef LOCTEXT_NAMESPACE
