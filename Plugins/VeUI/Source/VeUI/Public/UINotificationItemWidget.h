// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIButtonWidget.h"
#include "UINotificationData.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UINotifyItemWidget.h"
#include "UINotificationItemWidget.generated.h"

class UUINotificationItemWidget;

/**
 * Base for notification widgets.
 */
UCLASS(HideDropdown)
class VEUI_API UUINotificationItemWidget final : public UUINotifyItemWidget {
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget|Interaction")
	float Lifetime = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	FUINotificationData NotificationData;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	/** Fired when notification is closed. */
	DECLARE_EVENT_OneParam(UUINotificationItemWidget, FOnNotificationCloseButtonClicked, UUINotificationItemWidget*);

	FOnNotificationCloseButtonClicked OnNotificationCloseButtonClicked;

#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* HeaderWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* MessageWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* IconInfoWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* IconSuccessWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* IconWarningWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UImage* IconFailureWidget;

#pragma region Widgets

	/** Sound to play on fade in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* InfoFadeInSound;

	/** Sound to play on fade in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* SuccessFadeInSound;

	/** Sound to play on fade in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* WarningFadeInSound;

	/** Sound to play on fade in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* FailureFadeInSound;

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void SetNotificationData(const FUINotificationData InNotificationData);

private:
	void UpdateStatusIcons() const;
	void HideStatusIcons() const;

protected:
	virtual void NativeOnShowStarted() override;
};
