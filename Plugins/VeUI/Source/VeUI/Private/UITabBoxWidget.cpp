// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UITabBoxWidget.h"

void UUITabBoxWidget::NativePreConstruct() {
	Super::NativePreConstruct();

	Tabs.Empty();
	HorizontalBox->ClearChildren();
	for (int Index=0; Index < TabNames.Num(); Index++) {
		const auto TabButton = CreateWidget(TabButtonClass);
		if (TabButton) {
			UUITabButtonWidget* Tab = Cast<UUITabButtonWidget>(TabButton);
			if (Tab) {
				Tabs.Add(Tab);
                Tab->TabIndex = Index;
                Tab->SetTabActive(Index == ActiveIndex);
                Tab->SetTabName(FText::FromString(TabNames[Index]));
			}
			HorizontalBox->AddChildToHorizontalBox(TabButton);
		}
	}
}

void UUITabBoxWidget::NativeConstruct() {
	Super::NativeConstruct();
	
	for (const auto Tab :Tabs) {
		if (!Tab->OnTabSelectedEvent.IsAlreadyBound(this, &UUITabBoxWidget::OnTabSelectedCallback)) {
			Tab->OnTabSelectedEvent.AddDynamic(this, &UUITabBoxWidget::OnTabSelectedCallback);
		}
	}
}

void UUITabBoxWidget::OnTabSelectedCallback(int Index, FString TabName) {
	OnTabSelectedEvent.Broadcast(Index, TabName);
	for (const auto Tab :Tabs) {
		if (Tab->TabIndex == Index) {
			Tab->SetTabActive(true);
		} else {
			Tab->SetTabActive(false);
		}
	}
}
