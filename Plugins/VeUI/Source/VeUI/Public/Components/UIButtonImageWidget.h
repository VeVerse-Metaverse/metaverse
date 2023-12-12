// Author: Egor A. Pristavka, Nikolay Bulatov, Khusan T. Yadgarov 
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "UIButtonImageWidget.generated.h"

UENUM()
enum class EUIButtonImagePreset : uint8 {
	None,
	/** Transparent background, gray foreground. */
	Default,
	/** Green background, white foreground. */
	Action,
	/** Red background, white foreground. */
	Danger,

};

DECLARE_MULTICAST_DELEGATE(FOnButtonClicked);
DECLARE_MULTICAST_DELEGATE(FOnButtonHovered);

UCLASS(HideDropdown)
class VEUI_API UUIButtonImageWidget final : public UUIWidgetBase {
	GENERATED_BODY()

protected:
	virtual void NativeOnInitializedShared() override;
	virtual void NativePreConstruct() override;

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UPanelWidget* RootWidget;

#pragma endregion Widgets

	void SetButtonPreset(const EUIButtonImagePreset& InButtonPreset);

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category="Widget|Content")
	EUIButtonImagePreset ButtonPreset;

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category="Widget|Content")
	FMargin ContentPadding = 4.0f;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UButton* ButtonWidget = nullptr;

	UPROPERTY(EditAnywhere, Category="Widget|Content", meta=(BindWidget))
	class UImage* ButtonImageWidget = nullptr;

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
	FOnButtonClicked& GetOnButtonClicked() { return OnButtonClicked; }
	FOnButtonHovered& GetOnButtonHovered() { return OnButtonHovered; }
	FOnButtonHovered& GetOnButtonUnhovered() { return OnButtonUnhovered; }

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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	FLinearColor DefaultButtonBrushBackground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	FLinearColor DefaultButtonTintBackground = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	FLinearColor DefaultButtonForeground = FLinearColor(0.5f, 0.5f, 0.5f, 1.f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	FMargin DefaultButtonPadding = 5.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	float DefaultButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	float DefaultButtonFontOutlineSize = 1.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	FLinearColor DefaultButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	USoundBase* DefaultButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Default")
	USoundBase* DefaultButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	FLinearColor ActionButtonBrushBackground = FLinearColor(0.111932f, 0.462077f, 0.991102f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	FLinearColor ActionButtonTintBackground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	FLinearColor ActionButtonForeground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	FMargin ActionButtonPadding = 5.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	float ActionButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	float ActionButtonFontOutlineSize = 0.50f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	FLinearColor ActionButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	USoundBase* ActionButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Action")
	USoundBase* ActionButtonHoverSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	FLinearColor DangerButtonBrushBackground = FLinearColor(0.672443f, 0.111932f, 0.991102f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	FLinearColor DangerButtonTintBackground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	FLinearColor DangerButtonForeground = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	FMargin DangerButtonPadding = 5.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	float DangerButtonFontSize = 12.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	float DangerButtonFontOutlineSize = 0.50f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	FLinearColor DangerButtonFontOutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	USoundBase* DangerButtonClickSound;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget|Presets|Danger")
	USoundBase* DangerButtonHoverSound;

#pragma endregion
	
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
