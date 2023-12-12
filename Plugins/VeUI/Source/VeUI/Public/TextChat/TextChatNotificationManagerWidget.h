// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UINotifyManagerWidget.h"
#include "RpcWebSocketsTextChatTypes.h"
#include "TextChatNotificationManagerWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UTextChatNotificationManagerWidget : public UUINotifyManagerWidget {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<class UTextChatNotificationItemWidget> NotificationClass;

	UPROPERTY(EditAnywhere, Category="Widget|Notification", meta=(DisplayName="Padding"))
	FMargin NotificationPadding = {};

	UPROPERTY(EditAnywhere, Category="Widget|Notification", meta=(DisplayName="Horizontal Alignment"))
	TEnumAsByte<EHorizontalAlignment> NotificationHorizontalAlignment = HAlign_Fill;

	UPROPERTY(EditAnywhere, Category="Widget|Notification", meta=(DisplayName="Vertical Alignment"))
	TEnumAsByte<EVerticalAlignment> NotificationVerticalAlignment = VAlign_Fill;

	void AddMessage(const TSharedPtr<FRpcWebSocketsTextChatMessage>& Message);
	
protected:
	virtual void NativeOnChildAdded(UPanelSlot* InSlot) override;

};
