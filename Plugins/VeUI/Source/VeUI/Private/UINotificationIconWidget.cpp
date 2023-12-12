// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UINotificationIconWidget.h"

void UUINotificationIconWidget::PlayRippleAnimation(const bool bInLoop) {
	if (NotificationAnimation) {
		PlayAnimationForward(NotificationAnimation);
		if (bInLoop) {
			SetNumLoopsToPlay(NotificationAnimation, 0);
		} else {
			SetNumLoopsToPlay(NotificationAnimation, 1);
		}
	}
}
