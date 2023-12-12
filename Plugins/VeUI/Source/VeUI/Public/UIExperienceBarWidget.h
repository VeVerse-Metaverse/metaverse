// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/ProgressBar.h"
#include "UIWidgetBase.h"

#include "UIExperienceBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIExperienceBarWidget : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UProgressBar* ProgressBarWidget;

	void SetPercent(float Percent);
};
