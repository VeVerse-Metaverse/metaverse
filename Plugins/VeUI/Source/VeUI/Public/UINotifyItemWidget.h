// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"
#include "UINotifyItemWidget.generated.h"

class UUINotifyManagerWidget;
class UUIAnimationWidget;

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUINotifyItemWidget : public UUIWidgetBase {
	GENERATED_BODY()

	friend UUINotifyManagerWidget;

public:
	UPROPERTY(EditAnywhere, Category="Notification", meta=(DisplayName="Animation"))
	TSubclassOf<class UUIAnimationWidget> AnimationClass;

	/** 0.f = infinity */
	UPROPERTY(EditAnywhere, Category="Notification")
	float TimeToLive = 3.f;

	virtual void Show() override;
	virtual void Hide() override;

protected:
	virtual void NativeOnShowStarted() {
	}

	virtual void NativeOnShowFinished();

	virtual void NativeOnHideStarted() {
	}

	virtual void NativeOnHideFinished() {
	}

private:
	void SetAnimationWidget(UUIAnimationWidget* InAnimationWidget);
	TWeakObjectPtr<UUIAnimationWidget> AnimationWidget = nullptr;

	FTimerHandle HideTimerHandle;
};
