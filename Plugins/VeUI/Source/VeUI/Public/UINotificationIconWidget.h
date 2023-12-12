// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIWidgetBase.h"

#include "UINotificationIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUINotificationIconWidget : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* NotificationAnimation;

	void PlayRippleAnimation(bool bInLoop);
};
