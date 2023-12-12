// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Components/UICheckBoxWidget.h"

#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


bool UUICheckBoxWidget::IsChecked() const {
	return (CheckBoxWidget) ? CheckBoxWidget->IsChecked() : false;
}

void UUICheckBoxWidget::SetIsChecked(bool InIsChecked) {
	if (CheckBoxWidget) {
		CheckBoxWidget->SetIsChecked(InIsChecked);
	}
}

void UUICheckBoxWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	if (CheckBoxWidget) {
		CheckBoxWidget->OnCheckStateChanged.AddDynamic(this, &UUICheckBoxWidget::NativeOnChanged);
	}

	if (ButtonWidget) {
		ButtonWidget->OnClicked.AddDynamic(this, &UUICheckBoxWidget::TextBlock_OnClicked);
	}
}

void UUICheckBoxWidget::SynchronizeProperties() {
	Super::SynchronizeProperties();

	if (TextWidget) {
		TextWidget->SetText(Text);
	}
}

void UUICheckBoxWidget::NativeOnChanged(bool bInChecked) {
	OnCheckStateChanged.Broadcast(bInChecked);
}

void UUICheckBoxWidget::TextBlock_OnClicked() {
	if (CheckBoxWidget) {
		const bool Checked = !CheckBoxWidget->IsChecked();
		CheckBoxWidget->SetIsChecked(Checked);
		NativeOnChanged(Checked);
	}
	NativeOnChanged(false);
}
