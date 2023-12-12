// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UINotificationData.h"
#include "UINotifyManagerWidget.h"
#include "UINotificationManagerWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUINotificationManagerWidget : public UUINotifyManagerWidget {
	GENERATED_BODY()

public:
	UUINotificationManagerWidget();

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<class UUINotificationItemWidget> NotificationClass;

	UPROPERTY(EditAnywhere, Category="Widget|Notification", meta=(DisplayName="Padding"))
	FMargin NotificationPadding = {};

	UPROPERTY(EditAnywhere, Category="Widget|Notification", meta=(DisplayName="Horizontal Alignment"))
	TEnumAsByte<EHorizontalAlignment> NotificationHorizontalAlignment = HAlign_Fill;

	UPROPERTY(EditAnywhere, Category="Widget|Notification", meta=(DisplayName="Vertical Alignment"))
	TEnumAsByte<EVerticalAlignment> NotificationVerticalAlignment = VAlign_Fill;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnChildAdded(UPanelSlot* InSlot) override;

private:
	void AddNotification(const FUINotificationData& NotificationData);
};
