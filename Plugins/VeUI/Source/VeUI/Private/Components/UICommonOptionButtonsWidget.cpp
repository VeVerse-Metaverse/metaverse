// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Components/UICommonOptionButtonsWidget.h"

#include "VeShared.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUICommonOptionButtonsWidget::NativePreConstruct() {
	Super::NativePreConstruct();

	if (IsValid(ContainerWidget) && IsValid(ButtonClass)) {
		ContainerWidget->ClearChildren();
		for (const auto& Name : OptionList) {
			if (const auto Button = CreateWidget<UUIButtonWidget>(ButtonClass)) {
				Button->ButtonText = Name;
				auto* ButtonSlot = Cast<UHorizontalBoxSlot>(ContainerWidget->AddChild(Button));
				ButtonSlot->SetSize(ESlateSizeRule::Fill);
			}
		}
	}

}

void UUICommonOptionButtonsWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
	
	if (OptionList.Num()) {
		CurrentValue = 0;
	}
}

void UUICommonOptionButtonsWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (IsValid(ContainerWidget)) {
		int32 Index = 0;
		for (auto ChildrenWidget : ContainerWidget->GetAllChildren()) {
			auto* Button = Cast<UUIButtonWidget>(ChildrenWidget);
			if (Button) {
				Button->GetOnButtonClicked().AddWeakLambda(this, [this, Index]() {
					CurrentValue = Index;
					if (OnOptionChanged.IsBound()) {
						OnOptionChanged.Broadcast();
					}
				});
			}
			++Index;
		}
	}
}

void UUICommonOptionButtonsWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();
}


#undef LOCTEXT_NAMESPACE
