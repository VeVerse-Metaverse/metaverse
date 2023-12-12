// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/DEPRECATED_UIPlaceableDialogWidget.h"

#include "UIPageManagerWidget.h"
#include "Components/UIButtonWidget.h"
#include "UIPlayerController.h"
#include "UI/DEPRECATED_UIPlaceablePropertiesWidgetOLD.h"
#include "Components/Border.h"


void UDEPRECATED_UIPlaceableDialogWidget::SetProperties(TSubclassOf<UDEPRECATED_UIPlaceablePropertiesWidgetOLD> PropertiesWidgetClass, const TArray<FVePropertyMetadata>& InProperties) {
	if (!ContainerWidget) {
		return;
	}

	ContainerWidget->ClearChildren();

	PlaceablePropertiesWidget = CreateWidget<UDEPRECATED_UIPlaceablePropertiesWidgetOLD>(PropertiesWidgetClass);
	if (!PlaceablePropertiesWidget.IsValid()) {
		return;
	}

	if(auto* WidgetSlot = ContainerWidget->AddChild(PlaceablePropertiesWidget.Get())) {
		
	}

	PlaceablePropertiesWidget->SetProperties(InProperties);
}

void UDEPRECATED_UIPlaceableDialogWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (ConfirmButtonWidget) {
		ConfirmButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
			if (PlaceablePropertiesWidget.IsValid()) {
				OnPropertiesChanged.Broadcast(PlaceablePropertiesWidget->GetProperties());
			}
			NativeOnClose();
		});
	}

	if (CancelButtonWidget) {
		CancelButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
			NativeOnClose();
		});
	}

	auto* PC = Cast<AUIPlayerController>(GetOwningPlayer());
	if (IsValid(PC)) {
		PC->SwitchToUIMode();
	}
}

void UDEPRECATED_UIPlaceableDialogWidget::NativeOnClose() {
	auto* PC = Cast<AUIPlayerController>(GetOwningPlayer());
	if (IsValid(PC)) {
		PC->SwitchToGameMode();
	}
	RemoveFromParent();
}
