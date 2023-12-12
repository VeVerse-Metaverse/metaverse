// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "Components/UIButtonWidget.h"

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

UUIButtonWidget::UUIButtonWidget(const FObjectInitializer& ObjectInitializer) {

	// Desktop

	DefaultButtonStyle.Background = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	DefaultButtonStyle.Foreground = FLinearColor(0.0f, 0.450786f, 0.838799f, 1.0f);
	DefaultButtonStyle.FontSize = 12.0f;
	DefaultButtonStyle.FontOutlineSize = 0.0f;
	DefaultButtonStyle.Padding = FMargin(5.0f, 5.0f, 5.0f, 5.0f);
	DefaultButtonStyle.FontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	DefaultButtonStyle.HoverSound = nullptr;
	DefaultButtonStyle.ClickSound = nullptr;
	
	CloseButtonStyle = DefaultButtonStyle;
	CloseButtonStyle.NormalTint = FLinearColor(0.06f, 0.06f, 0.06f, 1.0f);
	CloseButtonStyle.Foreground = FLinearColor(0.672443f, 0.111932f, 0.991102f, 1.0f);
	CloseButtonStyle.FontOutlineSize = 1.0f;
	
	ActionButtonStyle = DefaultButtonStyle;
	ActionButtonStyle.NormalTint = FLinearColor(0.06f, 0.06f, 0.06f, 1.0f);
	ActionButtonStyle.Background = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	ActionButtonStyle.Foreground = FLinearColor(0.111932f, 0.462077f, 0.991102f, 1.0f);
	
	SuccessButtonStyle = DefaultButtonStyle;
	SuccessButtonStyle.NormalTint = FLinearColor(0.111932f, 0.462077f, 0.991102f, 1.0f);
	SuccessButtonStyle.Background = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	SuccessButtonStyle.Foreground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	SuccessButtonStyle.FontOutlineSize = 0.50f;
	
	WarningButtonStyle = DefaultButtonStyle;
	WarningButtonStyle.NormalTint = FLinearColor(0.55f, 0.55f, 0.01f, 1.0f);
	WarningButtonStyle.Background = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	WarningButtonStyle.Foreground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	WarningButtonStyle.FontOutlineSize = 0.50f;
	
	DangerButtonStyle = DefaultButtonStyle;
	DangerButtonStyle.NormalTint = FLinearColor(0.06f, 0.06f, 0.06f, 1.0f);
	DangerButtonStyle.Background = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	DangerButtonStyle.Foreground = FLinearColor(0.672443f, 0.111932f, 0.991102f, 1.0f);
	
	LinkButtonStyle = DefaultButtonStyle;
	LinkButtonStyle.Foreground = FLinearColor(0.0f, 0.2f, 1.0f, 1.0f);
	LinkButtonStyle.FontOutlineSize = 0.50f;
	LinkButtonStyle.Padding = FMargin(0.0f, 0.0f, 0.0f, 0.0f);
	
	ClearButtonStyle = DefaultButtonStyle;
	ClearButtonStyle.Foreground = FLinearColor(0.5f, 0.5f, 0.5f, 1.f);
	ClearButtonStyle.Padding = FMargin(0.0f, 0.0f, 0.0f, 0.0f);
	
	BreadcrumbButtonStyle = DefaultButtonStyle;
	BreadcrumbButtonStyle.Foreground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void UUIButtonWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	if (IsRunningDedicatedServer()) {
		return;
	}

	if (IsValid(ButtonWidget)) {
		if (!ButtonWidget->OnClicked.IsAlreadyBound(this, &UUIButtonWidget::OnButtonClickedCallback)) {
			ButtonWidget->OnClicked.AddDynamic(this, &UUIButtonWidget::OnButtonClickedCallback);
		}

		if (!ButtonWidget->OnHovered.IsAlreadyBound(this, &UUIButtonWidget::OnButtonHoveredCallback)) {
			ButtonWidget->OnHovered.AddDynamic(this, &UUIButtonWidget::OnButtonHoveredCallback);
		}

		if (!ButtonWidget->OnUnhovered.IsAlreadyBound(this, &UUIButtonWidget::OnButtonUnhoveredCallback)) {
			ButtonWidget->OnUnhovered.AddDynamic(this, &UUIButtonWidget::OnButtonUnhoveredCallback);
		}
	}
}

void UUIButtonWidget::NativePreConstruct() {
	if (IsRunningDedicatedServer()) {
		return;
	}

	UBorder* BorderWidget = Cast<UBorder>(RootWidget);
	if (IsValid(BorderWidget)) {

		SetButtonPreset(ButtonPreset);

		// Override background color.
		if (ButtonBackgroundColor != FLinearColor::Transparent) {
			BorderWidget->Background.TintColor = ButtonBackgroundColor;
		}

		// Override font size.
		if (ButtonFontSize > 0) {
			FSlateFontInfo FontInfo = ButtonTextWidget->GetFont();
			FontInfo.Size = ButtonFontSize;
			ButtonTextWidget->SetFont(FontInfo);
		}
	}

	if (IsValid(ButtonTextWidget)) {
		ButtonTextWidget->SetText(ButtonText);
	}

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

void UUIButtonWidget::SetButtonPreset(const EUIButtonPreset& InButtonPreset) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	ButtonPreset = InButtonPreset;

	FUIButtonStyle* Style = nullptr;

	switch (InButtonPreset) {
	case EUIButtonPreset::Close:
		Style = &CloseButtonStyle;
		break;
	case EUIButtonPreset::Action:
		Style = &ActionButtonStyle;
		break;
	case EUIButtonPreset::Success:
		Style = &SuccessButtonStyle;
		break;
	case EUIButtonPreset::Warning:
		Style = &WarningButtonStyle;
		break;
	case EUIButtonPreset::Danger:
		Style = &DangerButtonStyle;
		break;
	case EUIButtonPreset::Link:
		Style = &LinkButtonStyle;
		break;
	case EUIButtonPreset::Clear:
		Style = &ClearButtonStyle;
		break;
	case EUIButtonPreset::Breadcrumb:
		Style = &BreadcrumbButtonStyle;
		break;
	case EUIButtonPreset::KeyboardVR:
		Style = &KeyboardVRStyle;
		break;
	case EUIButtonPreset::KeyboardHighlightVR:
		Style = &KeyboardHighlightVRStyle;
		break;
	default:
		Style = &DefaultButtonStyle;
		break;
	}

	UBorder* BorderWidget = Cast<UBorder>(RootWidget);
	if (IsValid(BorderWidget)) {
		BorderWidget->Background.TintColor = Style->Background;
		BorderWidget->SetBrushColor(Style->NormalTint);
	}

	ButtonTextWidget->SetColorAndOpacity(Style->Foreground);

	if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(ButtonContentSlot->Slot)) {
		ButtonSlot->SetPadding(Style->Padding);
	}

	// if (ButtonWidget) {
	// 	ButtonWidget->BackgroundColor = Style->Background;
	// 	ButtonWidget->WidgetStyle.Normal.TintColor = Style->NormalTint;
	// 	ButtonWidget->WidgetStyle.Hovered.TintColor = Style->HoveredTint;
	// 	ButtonWidget->WidgetStyle.Pressed.TintColor = Style->PressedTint;
	// 	ButtonWidget->WidgetStyle.Disabled.TintColor = Style->DisabledTint;
	// }

	FSlateFontInfo FontInfo = ButtonTextWidget->GetFont();
	FontInfo.Size = Style->FontSize;
	FontInfo.OutlineSettings.OutlineSize = Style->FontOutlineSize;
	FontInfo.OutlineSettings.OutlineColor = Style->FontOutlineColor;
	ButtonTextWidget->SetFont(FontInfo);

	ButtonHoverSound = Style->HoverSound;
	ButtonClickSound = Style->ClickSound;

	// ButtonHoverAnimation = ButtonHoverDefaultAnimation;
}

void UUIButtonWidget::SetButtonText(const FText& Text) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	ButtonText = Text;

	if (IsValid(ButtonTextWidget)) {
		ButtonTextWidget->SetText(ButtonText);
	}
}

void UUIButtonWidget::SetButtonIcon(UTexture2D* Texture, const float InSizeX, const float InSizeY) const {
	if (IsRunningDedicatedServer()) {
		return;
	}

	UImage* ImageSlot = Cast<UImage>(ButtonContentSlot->GetContent());
	if (ImageSlot) {
		ImageSlot->SetBrushFromTexture(Texture);
		ImageSlot->SetDesiredSizeOverride(FVector2D(InSizeX, InSizeY));
	}
}

void UUIButtonWidget::SetIsFocusable(const bool bInFocusable) const {
	if (IsRunningDedicatedServer()) {
		return;
	}

	if (ButtonWidget) {
		ButtonWidget->IsFocusable = bInFocusable;
		ButtonWidget->TakeWidget();
	}
}

void UUIButtonWidget::OnButtonClickedCallback() {
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

void UUIButtonWidget::OnButtonHoveredCallback() {}

void UUIButtonWidget::OnButtonUnhoveredCallback() {}
