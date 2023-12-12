// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once



#include "UINotificationData.h"
#include "UINotificationWidget.h"
#include "UIWidgetBase.h"

#include "Blueprint/UserWidget.h"

#include "UINotificationRootWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUINotificationRootWidget : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	UUINotificationRootWidget();

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UPanelWidget* NotificationContainer;

	/** Class of widgets to spawn. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	TSubclassOf<UUINotificationWidget> NotificationWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content", meta=(UIMin="1", UIMax="10"))
	int32 MaxVisibleNotifications;

	/** Pending notifications. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content")
	TArray<UUINotificationWidget*> NotificationQueue;

	/** Displayed notifications. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content")
	TArray<UUINotificationWidget*> VisibleNotifications;

	/** Handles notification enqueuing and dequeuing. */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Enqueue a new notification to be displayed in the notification block. */
	UFUNCTION(BlueprintCallable, Category="Widget")
	void AddNotification(const FUINotificationData& InNotificationData);
};
