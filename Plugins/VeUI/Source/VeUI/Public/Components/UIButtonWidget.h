// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "UIButtonWidget.generated.h"

class UBorder;
UENUM()
enum class EUIButtonPreset : uint8 {
	None,
	/** Transparent background, gray foreground. */
	Default,

	Close,
	/** Blue background, white foreground. */
	Action,
	/** Green background, white foreground. */
	Success,
	/** Yellow background, white foreground. */
	Warning,
	/** Red background, white foreground. */
	Danger,
	/** Transparent background, blue foreground. */
	Link,
	/** Clear buttons for inputs. */
	Clear,
	/** Style for breadcrumbs. */
	Breadcrumb,
	
	/** Buttons for virtual keyboard. */
	KeyboardVR,
	/** Highlighted buttons for virtual keyboard. Optimized for VR. */
	KeyboardHighlightVR
};

USTRUCT(BlueprintType)
struct FUIButtonStyle {
	GENERATED_BODY()

	// TintBackground
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FLinearColor Background = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FLinearColor Foreground = FLinearColor(0.0f, 0.450786f, 0.838799f, 1.0f);

	// BrushBackground
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FLinearColor NormalTint = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="222")
	FLinearColor HoveredTint = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FLinearColor PressedTint = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FLinearColor DisabledTint = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FMargin Padding = FMargin(5.0f, 5.0f, 5.0f, 5.0f);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float FontSize = 12.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float FontOutlineSize = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FLinearColor FontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USoundBase* ClickSound = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USoundBase* HoverSound = nullptr;
};

DECLARE_MULTICAST_DELEGATE(FOnButtonClicked);
DECLARE_MULTICAST_DELEGATE(FOnButtonHovered);

UCLASS(HideDropdown)
class VEUI_API UUIButtonWidget final : public UUIWidgetBase {
	GENERATED_BODY()

protected:
	virtual void NativeOnInitializedShared() override;
	virtual void NativePreConstruct() override;

public:
	UUIButtonWidget(const FObjectInitializer& ObjectInitializer);

#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UPanelWidget* RootWidget;

#pragma endregion Widgets

	void SetButtonPreset(const EUIButtonPreset& InButtonPreset);

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category="Widget|Content")
	EUIButtonPreset ButtonPreset;

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category="Widget|Content")
	FMargin ContentPadding = 4.0f;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UButton* ButtonWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* ButtonTextWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UNamedSlot* ButtonContentSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	FLinearColor ButtonBackgroundColor = FLinearColor::Transparent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	int32 ButtonFontSize = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	FText ButtonText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	bool bUseURL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	FString URL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* ButtonClickSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* ButtonHoverSound;

private:
	/** Called when the button is clicked */
	FOnButtonClicked OnButtonClicked;

	/** Called when the button is hovered */
	FOnButtonHovered OnButtonHovered;

	/** Called when the button is unhovered */
	FOnButtonHovered OnButtonUnhovered;

public:
	FOnButtonClicked& GetOnButtonClicked() {
		return OnButtonClicked;
	}

	FOnButtonHovered& GetOnButtonHovered() {
		return OnButtonHovered;
	}

	FOnButtonHovered& GetOnButtonUnhovered() {
		return OnButtonUnhovered;
	}

	UPROPERTY()
	UWidgetAnimation* ButtonHoverAnimation = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ButtonHoverDefaultAnimation = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ButtonHoverActionAnimation = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ButtonHoverSuccessAnimation = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ButtonHoverWarningAnimation = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ButtonHoverDangerAnimation = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ButtonHoverLinkAnimation = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ButtonHoverClearAnimation = nullptr;

#pragma region Button Templates

	// Desktop

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle DefaultButtonStyle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle CloseButtonStyle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle ActionButtonStyle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle SuccessButtonStyle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle WarningButtonStyle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle DangerButtonStyle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle LinkButtonStyle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle ClearButtonStyle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle BreadcrumbButtonStyle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle KeyboardVRStyle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets")
	FUIButtonStyle KeyboardHighlightVRStyle;


#pragma region deprecated
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	FLinearColor DefaultButtonBrushBackground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	FLinearColor DefaultButtonTintBackground = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	FLinearColor DefaultButtonForeground = FLinearColor(0.0f, 0.450786f, 0.838799f, 1.f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	FMargin DefaultButtonPadding = 5.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	float DefaultButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	float DefaultButtonFontOutlineSize = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	FLinearColor DefaultButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	USoundBase* DefaultButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	USoundBase* DefaultButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Close")
	FLinearColor CloseButtonBrushBackground = FLinearColor(0.06f, 0.06f, 0.06f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Close")
	FLinearColor CloseButtonTintBackground = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Close")
	FLinearColor CloseButtonForeground = FLinearColor(0.672443f, 0.111932f, 0.991102f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Close")
	FMargin CloseButtonPadding = 5.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Close")
	float CloseButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Close")
	float CloseButtonFontOutlineSize = 1.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Close")
	FLinearColor CloseButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Close")
	USoundBase* CloseButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Close")
	USoundBase* CloseButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	FLinearColor ActionButtonBrushBackground = FLinearColor(0.06f, 0.06f, 0.06f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	FLinearColor ActionButtonTintBackground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	FLinearColor ActionButtonForeground = FLinearColor(0.111932f, 0.462077f, 0.991102f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	FMargin ActionButtonPadding = 5.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	float ActionButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	float ActionButtonFontOutlineSize = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	FLinearColor ActionButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	USoundBase* ActionButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	USoundBase* ActionButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Success")
	FLinearColor SuccessButtonBrushBackground = FLinearColor(0.111932f, 0.462077f, 0.991102f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Success")
	FLinearColor SuccessButtonTintBackground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Success")
	FLinearColor SuccessButtonForeground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Success")
	FMargin SuccessButtonPadding = 5.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Success")
	float SuccessButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Success")
	float SuccessButtonFontOutlineSize = 0.50f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Success")
	FLinearColor SuccessButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Success")
	USoundBase* SuccessButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Success")
	USoundBase* SuccessButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Warning")
	FLinearColor WarningButtonBrushBackground = FLinearColor(0.55f, 0.55f, 0.01f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Warning")
	FLinearColor WarningButtonTintBackground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Warning")
	FLinearColor WarningButtonForeground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Warning")
	FMargin WarningButtonPadding = 5.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Warning")
	float WarningButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Warning")
	float WarningButtonFontOutlineSize = 0.50f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Warning")
	FLinearColor WarningButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Warning")
	USoundBase* WarningButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Warning")
	USoundBase* WarningButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	FLinearColor DangerButtonBrushBackground = FLinearColor(0.06f, 0.06f, 0.06f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	FLinearColor DangerButtonTintBackground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	FLinearColor DangerButtonForeground = FLinearColor(0.672443f, 0.111932f, 0.991102f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	FMargin DangerButtonPadding = 5.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	float DangerButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	float DangerButtonFontOutlineSize = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	FLinearColor DangerButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	USoundBase* DangerButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	USoundBase* DangerButtonHoverSound;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Link")
	FLinearColor LinkButtonBrushBackground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Link")
	FLinearColor LinkButtonTintBackground = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Link")
	FLinearColor LinkButtonForeground = FLinearColor(0.0f, 0.2f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Link")
	FMargin LinkButtonPadding = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Link")
	float LinkButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Link")
	float LinkButtonFontOutlineSize = 0.50f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Link")
	FLinearColor LinkButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Link")
	USoundBase* LinkButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Link")
	USoundBase* LinkButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Clear")
	FLinearColor ClearButtonBrushBackground = FLinearColor(1.f, 1.f, 1.f, 1.f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Clear")
	FLinearColor ClearButtonTintBackground = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Clear")
	FLinearColor ClearButtonForeground = FLinearColor(.5f, .5f, .5f, 1.f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Clear")
	FMargin ClearButtonPadding = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Clear")
	float ClearButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Clear")
	float ClearButtonFontOutlineSize = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Clear")
	FLinearColor ClearButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Clear")
	USoundBase* ClearButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Clear")
	USoundBase* ClearButtonHoverSound;

#pragma endregion

#pragma region VR Button Templates
/*
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor DefaultVRButtonBrushBackground = FLinearColor(0.8f, 0.8f, 0.8f, 1.f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor DefaultVRButtonTintBackground = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor DefaultVRButtonForeground = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FMargin DefaultVRButtonPadding = 7.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float DefaultVRButtonFontSize = 24.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float DefaultVRButtonFontOutlineSize = 2.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor DefaultVRButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* DefaultVRButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* DefaultVRButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor CloseVRButtonBrushBackground = FLinearColor(0.8f, 0.8f, 0.8f, 1.f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor CloseVRButtonTintBackground = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor CloseVRButtonForeground = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FMargin CloseVRButtonPadding = 7.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float CloseVRButtonFontSize = 14.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float CloseVRButtonFontOutlineSize = 2.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor DCloseRButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* CloseVRButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* CloseVRButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor ActionVRButtonBrushBackground = FLinearColor(0.0f, 0.05f, 0.2f, 0.8f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor ActionVRButtonTintBackground = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor ActionVRButtonForeground = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FMargin ActionVRButtonPadding = 7.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float ActionVRButtonFontSize = 14.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float ActionVRButtonFontOutlineSize = 2.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor ActionVRButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* ActionVRButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* ActionVRButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor DangerVRButtonBrushBackground = FLinearColor(0.16f, 0.01f, 0.01f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor DangerVRButtonTintBackground = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor DangerVRButtonForeground = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FMargin DangerVRButtonPadding = 7.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float DangerVRButtonFontSize = 14.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float DangerVRButtonFontOutlineSize = 2.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor DangerVRButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* DangerVRButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* DangerVRButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor LinkVRButtonBrushBackground = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor LinkVRButtonTintBackground = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor LinkVRButtonForeground = FLinearColor(0.0f, 0.1f, 0.8f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FMargin LinkVRButtonPadding = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float LinkVRButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float LinkVRButtonFontOutlineSize = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor LinkVRButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* LinkVRButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* LinkVRButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor ClearVRButtonBrushBackground = FLinearColor(0.8f, 0.8f, 0.8f, 1.f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor ClearVRButtonTintBackground = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor ClearVRButtonForeground = FLinearColor(0.5f, 0.5f, 0.5f, 1.f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FMargin ClearVRButtonPadding = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float ClearVRButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float ClearVRButtonFontOutlineSize = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor ClearVRButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* ClearVRButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* ClearVRButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor KeyboardVRButtonBrushBackground = FLinearColor(0.8f, 0.8f, 0.8f, 1.f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor KeyboardVRButtonTintBackground = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor KeyboardVRButtonForeground = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FMargin KeyboardVRButtonPadding = 10.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float KeyboardVRButtonFontSize = 30.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float KeyboardVRButtonFontOutlineSize = 2.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor KeyboardVRButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* KeyboardVRButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* KeyboardVRButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor KeyboardHighlightVRButtonBrushBackground = FLinearColor(0.0f, 0.05f, 0.2f, 0.8f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor KeyboardHighlightVRButtonTintBackground = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor KeyboardHighlightVRButtonForeground = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FMargin KeyboardHighlightVRButtonPadding = 10.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float KeyboardHighlightVRButtonFontSize = 30.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	float KeyboardHighlightVRButtonFontOutlineSize = 2.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	FLinearColor KeyboardHighlightVRButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* KeyboardHighlightVRButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Deprecated")
	USoundBase* KeyboardHighlightVRButtonHoverSound;
*/
#pragma endregion

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void SetButtonText(const FText& Text);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void SetButtonIcon(UTexture2D* Texture, const float InSizeX, const float InSizeY) const;

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void SetIsFocusable(bool bInFocusable) const;

protected:
	UFUNCTION()
	void OnButtonClickedCallback();

	UFUNCTION()
	void OnButtonHoveredCallback();

	UFUNCTION()
	void OnButtonUnhoveredCallback();
};
