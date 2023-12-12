// 

#include "Components/UITextPropertyWidget.h"

#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"


void UUITextPropertyWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
	if (TitleTextWidget) {
		TitleTextWidget->SetText(FText::GetEmpty());
	}

	if (ValueTextWidget) {
		ValueTextWidget->SetText(FText::GetEmpty());
	}
}

void UUITextPropertyWidget::SynchronizeProperties() {
	Super::SynchronizeProperties();

	if (TitleTextWidget) {
		TitleTextWidget->SetText(Title);
		TitleTextWidget->SetFont(Font);
	}
	if (ValueTextWidget) {
		ValueTextWidget->SetText(Value);
		ValueTextWidget->SetFont(Font);
	}

	SetTitleSize(TitleSize);
	SetVisible(!(HideIfValueEmpty && Value.IsEmpty()));
}

void UUITextPropertyWidget::SetValue(const FText& InTitle, const FText& InValue) {
	if (TitleTextWidget) {
		TitleTextWidget->SetText(InTitle);
	}

	if (ValueTextWidget) {
		ValueTextWidget->SetText(InValue);
	}
}

void UUITextPropertyWidget::SetTitleSize(float InValue) {
	if (!TitleTextWidget || !ValueTextWidget) {
		return;
	}

	InValue = FMath::Clamp(InValue, 0.0f, 1.0f);

	if (auto* BoxSlot = Cast<UHorizontalBoxSlot>(TitleTextWidget->Slot.Get())) {
		FSlateChildSize SlateChildSize;
		SlateChildSize.SizeRule = ESlateSizeRule::Fill;
		SlateChildSize.Value = InValue;
		BoxSlot->SetSize(SlateChildSize);
	}

	if (auto* BoxSlot = Cast<UHorizontalBoxSlot>(ValueTextWidget->Slot.Get())) {
		FSlateChildSize SlateChildSize;
		SlateChildSize.SizeRule = ESlateSizeRule::Fill;
		SlateChildSize.Value = 1.0f - InValue;
		BoxSlot->SetSize(SlateChildSize);
	}
}

void UUITextPropertyWidget::SetVisible(bool InValue) {
	SetVisibility(InValue ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
