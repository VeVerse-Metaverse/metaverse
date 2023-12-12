// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UITabButtonWidget.h"

void UUITabButtonWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	
	if (IsValid(Button)) {
		Button->SetButtonText(TabName);
	}

	if (IsValid(UnderlineImage)) {
		UnderlineImage->ColorAndOpacity = GetColor();
	}
}

void UUITabButtonWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (IsValid(Button)) {
		if (!OnTabSelectedDelegateHandle.IsValid()) {
        	OnTabSelectedDelegateHandle = Button->GetOnButtonClicked().AddUObject(this, &UUITabButtonWidget::OnButtonClickedCallback);
        }
	}
	
}

void UUITabButtonWidget::SetTabName(const FText& Text) {
	TabName = Text;

	if (IsValid(Button)) {
		Button->SetButtonText(TabName);
	}
}

void UUITabButtonWidget::SetTabActive(const bool Active) {
	bActive = Active;
	if (UnderlineImage) {
		UnderlineImage->SetBrushTintColor(GetColor());
	}	
}

void UUITabButtonWidget::OnButtonClickedCallback() {
	OnTabSelectedEvent.Broadcast(TabIndex, TabName.ToString());
}

FLinearColor UUITabButtonWidget::GetColor() {
	return bActive ? ActiveTabColor : DefaultTabColor;
}
