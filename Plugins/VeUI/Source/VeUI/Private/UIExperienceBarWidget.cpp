// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIExperienceBarWidget.h"


void UUIExperienceBarWidget::SetPercent(float Percent) {
	if (ProgressBarWidget) {
		ProgressBarWidget->SetPercent(Percent);
	}
}
