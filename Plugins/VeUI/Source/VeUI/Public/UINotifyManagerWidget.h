// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"
#include "UINotifyManagerWidget.generated.h"

class UUIAnimationWidget;
class UUINotifyItemWidget;

/**
 * 
 */
UCLASS()
class VEUI_API UUINotifyManagerWidget : public UUIWidgetBase {
	GENERATED_BODY()

	friend UUINotifyItemWidget;

public:
#pragma region Widgets

	/** Container for child animation widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UPanelWidget* ContainerWidget;

#pragma endregion Widgets

	UPROPERTY(EditAnywhere, Category="Widget", meta=(DisplayName="Default Animation"))
	TSubclassOf<UUIAnimationWidget> DefaultAnimationClass;

	UPROPERTY(EditAnywhere, Category="Widget")
	int32 MaxVisibleNotifications = 0;

	virtual void AddItem(UUINotifyItemWidget* NotificationWidget);

protected:
	virtual UUIAnimationWidget* CreateAnimationWidget(UUINotifyItemWidget* NotificationWidget);

	UPROPERTY()
	TArray<UUIAnimationWidget*> NotificationQueue;

	virtual void OnShowStartedCallback(UUIAnimationWidget* InAnimationWidget);
	virtual void OnShowFinishedCallback(UUIAnimationWidget* InAnimationWidget);
	virtual void OnHideStartedCallback(UUIAnimationWidget* InAnimationWidget);
	virtual void OnHideFinishedCallback(UUIAnimationWidget* InAnimationWidget);

	virtual void NativeOnChildAdded(UPanelSlot* InSlot) {}
};
