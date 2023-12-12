// Author: Egor A. Pristavka, Nikolay Bulatov, Khusan T. Yadgarov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "Components/UIButtonImageWidget.h"

#include "VeShared.h"
// #include "VeWorldSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/ButtonSlot.h"
#include "Components/Image.h"
#include "Components/NamedSlot.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundBase.h"

void UUIButtonImageWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	if (IsRunningDedicatedServer()) {
		return;
	}

	if (IsValid(ButtonWidget)) {
		if (!ButtonWidget->OnClicked.IsAlreadyBound(this, &UUIButtonImageWidget::OnButtonClickedCallback)) {
			ButtonWidget->OnClicked.AddDynamic(this, &UUIButtonImageWidget::OnButtonClickedCallback);
		}

		if (!ButtonWidget->OnHovered.IsAlreadyBound(this, &UUIButtonImageWidget::OnButtonHoveredCallback)) {
			ButtonWidget->OnHovered.AddDynamic(this, &UUIButtonImageWidget::OnButtonHoveredCallback);
		}

		if (!ButtonWidget->OnUnhovered.IsAlreadyBound(this, &UUIButtonImageWidget::OnButtonUnhoveredCallback)) {
			ButtonWidget->OnUnhovered.AddDynamic(this, &UUIButtonImageWidget::OnButtonUnhoveredCallback);
		}
	}
}

void UUIButtonImageWidget::NativePreConstruct() {
	if (IsRunningDedicatedServer()) {
		return;
	}

	UBorder* BorderWidget = Cast<UBorder>(RootWidget);

	if (IsValid(BorderWidget)) {
		UButtonSlot* ButtonSlot = Cast<UButtonSlot>(ButtonContentSlot->Slot);

		switch (ButtonPreset) {
		case EUIButtonImagePreset::Default:
			BorderWidget->Background.TintColor = DefaultButtonTintBackground;
			BorderWidget->SetBrushColor(DefaultButtonBrushBackground);
			ButtonImageWidget->SetColorAndOpacity(DefaultButtonForeground);
			if (ButtonSlot) {
				ButtonSlot->SetPadding(DefaultButtonPadding);
			}
			ButtonHoverSound = DefaultButtonHoverSound;
			ButtonClickSound = DefaultButtonClickSound;
			ButtonHoverAnimation = ButtonHoverDefaultAnimation;
			break;
		case EUIButtonImagePreset::Action:
			BorderWidget->Background.TintColor = ActionButtonTintBackground;
			BorderWidget->SetBrushColor(ActionButtonBrushBackground);
			ButtonImageWidget->SetColorAndOpacity(ActionButtonForeground);
			ButtonImageWidget->SetBrushTintColor(ActionButtonTintBackground);

			if (ButtonSlot) {
				ButtonSlot->SetPadding(ActionButtonPadding);
			}
			ButtonHoverSound = ActionButtonHoverSound;
			ButtonClickSound = ActionButtonClickSound;
			ButtonHoverAnimation = ButtonHoverActionAnimation;
			break;
		case EUIButtonImagePreset::Danger:
			BorderWidget->Background.TintColor = DangerButtonTintBackground;
			BorderWidget->SetBrushColor(DangerButtonBrushBackground);
			ButtonImageWidget->SetColorAndOpacity(DangerButtonTintBackground);
			ButtonImageWidget->SetBrushTintColor(DangerButtonForeground);
			if (ButtonSlot) {
				ButtonSlot->SetPadding(DangerButtonPadding);
			}
			ButtonHoverSound = DangerButtonHoverSound;
			ButtonClickSound = DangerButtonClickSound;
			ButtonHoverAnimation = ButtonHoverDangerAnimation;
			break;
		default:
			break;
		}

		// Override background color.
		if (ButtonBackgroundColor != FLinearColor::Transparent) {
			BorderWidget->Background.TintColor = ButtonBackgroundColor;
		}
	}

	// if (IsValid(ButtonImageWidget)) {
	// 	ButtonImageWidget->SetText(ButtonText);
	// }

	if (IsValid(ButtonWidget)) {
		if (ButtonHoverSound) {
			FSlateSound HoverSlateSound;
			HoverSlateSound.SetResourceObject(ButtonHoverSound);
			ButtonWidget->WidgetStyle.SetHoveredSound(HoverSlateSound);
		}

		if (ButtonClickSound) {
			FSlateSound ClickSlateSound;
			ClickSlateSound.SetResourceObject(ButtonClickSound);
			ButtonWidget->WidgetStyle.SetPressedSound(ClickSlateSound);
		}
	}

	if (IsValid(ButtonContentSlot)) {
		if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(ButtonContentSlot->Slot)) {
			ButtonSlot->SetPadding(ContentPadding);
		}
	}

	Super::NativePreConstruct();
}

void UUIButtonImageWidget::SetButtonPreset(const EUIButtonImagePreset& InButtonPreset) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	UBorder* BorderWidget = Cast<UBorder>(RootWidget);

	ButtonPreset = InButtonPreset;

	if (IsValid(BorderWidget)) {
		UButtonSlot* ButtonSlot = Cast<UButtonSlot>(ButtonContentSlot->Slot);

		switch (InButtonPreset) {
		case EUIButtonImagePreset::Default:
			BorderWidget->Background.TintColor = DefaultButtonTintBackground;
			BorderWidget->SetBrushColor(DefaultButtonBrushBackground);
			ButtonImageWidget->SetColorAndOpacity(DefaultButtonForeground);
			ButtonImageWidget->SetBrushTintColor(DefaultButtonTintBackground);
			if (ButtonSlot) {
				ButtonSlot->SetPadding(DefaultButtonPadding);
			}
			ButtonHoverSound = DefaultButtonHoverSound;
			ButtonClickSound = DefaultButtonClickSound;
			ButtonHoverAnimation = ButtonHoverDefaultAnimation;
			break;
		case EUIButtonImagePreset::Action:
			BorderWidget->Background.TintColor = ActionButtonTintBackground;
			BorderWidget->SetBrushColor(ActionButtonBrushBackground);
			ButtonImageWidget->SetColorAndOpacity(ActionButtonForeground);
			ButtonImageWidget->SetBrushTintColor(ActionButtonTintBackground);
			if (ButtonSlot) {
				ButtonSlot->SetPadding(ActionButtonPadding);
			}
			ButtonHoverSound = ActionButtonHoverSound;
			ButtonClickSound = ActionButtonClickSound;
			ButtonHoverAnimation = ButtonHoverActionAnimation;
			break;
		case EUIButtonImagePreset::Danger:
			BorderWidget->Background.TintColor = DangerButtonTintBackground;
			BorderWidget->SetBrushColor(DangerButtonBrushBackground);
			ButtonImageWidget->SetColorAndOpacity(DangerButtonForeground);
			ButtonImageWidget->SetBrushTintColor(DangerButtonTintBackground);
			if (ButtonSlot) {
				ButtonSlot->SetPadding(DangerButtonPadding);
			}
			ButtonHoverSound = DangerButtonHoverSound;
			ButtonClickSound = DangerButtonClickSound;
			ButtonHoverAnimation = ButtonHoverDangerAnimation;
			break;
		default:
			break;
		}
	}
}

// void UUIButtonImageWidget::SetButtonText(const FText& Text) {
// 	if (IsRunningDedicatedServer()) {
// 		return;
// 	}
//
// 	ButtonText = Text;
//
// 	if (IsValid(ButtonTextWidget)) {
// 		ButtonTextWidget->SetText(ButtonText);
// 	}
// }

void UUIButtonImageWidget::SetButtonIcon(UTexture2D* Texture, const float InSizeX, const float InSizeY) const {
	if (IsRunningDedicatedServer()) {
		return;
	}

	UImage* ImageSlot = Cast<UImage>(ButtonContentSlot->GetContent());
	if (ImageSlot) {
		ImageSlot->SetBrushFromTexture(Texture);
		ImageSlot->SetDesiredSizeOverride(FVector2D(InSizeX, InSizeY));
	}
}

void UUIButtonImageWidget::SetIsFocusable(const bool bInFocusable) const {
	if (IsRunningDedicatedServer()) {
		return;
	}

	if (ButtonWidget) {
		ButtonWidget->IsFocusable = bInFocusable;
		ButtonWidget->TakeWidget();
	}
}

void UUIButtonImageWidget::OnButtonClickedCallback() {
	if (IsRunningDedicatedServer()) {
		return;
	}

	if (ButtonClickSound) {
		PlaySound(ButtonClickSound);
	}

	if (bUseURL) {
		UKismetSystemLibrary::LaunchURL(URL);
	}

	if (OnButtonClicked.IsBound()) {
		OnButtonClicked.Broadcast();
	}
}

void UUIButtonImageWidget::OnButtonHoveredCallback() {}

void UUIButtonImageWidget::OnButtonUnhoveredCallback() {}
