// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "UIHUDWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIHUDWidget : public UUserWidget
 {
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category="Widget", meta=(BindWidgetOptional))
	class UUIWidgetBase* CrosshairWidget;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category="Widget", meta=(BindWidgetOptional))
	class UUINotificationIconWidget* NotificationIconWidget;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category="Widget", meta=(BindWidgetOptional))
	class UUIWidgetBase* VoiceIconWidget;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Widget")
	class UUIWidgetBase* InspectWidget;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void ShowVoiceIndicator() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void HideVoiceIndicator() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void ShowNotification(bool bLoop = true) const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void HideNotification() const;

	void ShowInspectWidget();
	void HideInspectWidget();
};
