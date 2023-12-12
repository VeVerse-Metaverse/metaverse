
#include "Components/UIImageButtonWidget.h"

#include "Components/Image.h"
#include "Components/Button.h"
#include "Sound/SoundBase.h"


void UUIImageButtonWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();
	if (PreviewImageButtonWidget) {
		PreviewImageButtonWidget->OnClicked.AddDynamic(this, &UUIImageButtonWidget::NativeOnClicked);
	}
}

void UUIImageButtonWidget::SynchronizeProperties() {
	Super::SynchronizeProperties();

	if (PreviewImageButtonWidget) {
		if (IsClickable) {
			PreviewImageButtonWidget->SetCursor(EMouseCursor::Hand);
			FButtonStyle NewButtonStyle(PreviewImageButtonWidget->WidgetStyle);
			FSlateSound Sound;
			Sound.SetResourceObject(HoveredSound);
			NewButtonStyle.SetHoveredSound(Sound);
			Sound.SetResourceObject(PressedSound);
			NewButtonStyle.SetPressedSound(Sound);
			PreviewImageButtonWidget->SetStyle(NewButtonStyle);
		} else {
			PreviewImageButtonWidget->SetCursor(EMouseCursor::Default);
	
			FSlateSound NullSound;
			FButtonStyle NewButtonStyle(PreviewImageButtonWidget->WidgetStyle);
			NewButtonStyle.SetHoveredSound(NullSound);
			NewButtonStyle.SetPressedSound(NullSound);
			PreviewImageButtonWidget->SetStyle(NewButtonStyle);
		}
	}
}

void UUIImageButtonWidget::NativeOnClicked() {
	OnClicked.Broadcast();
}
