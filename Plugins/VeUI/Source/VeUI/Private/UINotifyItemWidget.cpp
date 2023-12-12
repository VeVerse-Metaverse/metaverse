// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UINotifyItemWidget.h"
#include "UIAnimationWidget.h"

void UUINotifyItemWidget::Show() {
	if (AnimationWidget.IsValid()) {
		if (HideTimerHandle.IsValid()) {
			HideTimerHandle.Invalidate();
		}
		AnimationWidget->Show();
	}
}

void UUINotifyItemWidget::Hide() {
	if (auto* TimerManager = GetTimerManager()) {
		TimerManager->ClearTimer(HideTimerHandle);
	}

	if (AnimationWidget.IsValid()) {
		if (HideTimerHandle.IsValid()) {
			HideTimerHandle.Invalidate();
		}
		AnimationWidget->Hide();
	}
}

void UUINotifyItemWidget::NativeOnShowFinished() {
	if (AnimationWidget.IsValid() && TimeToLive) {
		FTimerDelegate TimerHide;
		TimerHide.BindWeakLambda(this, [&]() {
			Hide();
		});

		if (auto* TimerManager = GetTimerManager()) {
			TimerManager->SetTimer(HideTimerHandle, TimerHide, TimeToLive, false);
		}
	}
}

void UUINotifyItemWidget::SetAnimationWidget(UUIAnimationWidget* InAnimationWidget) {
	AnimationWidget = InAnimationWidget;
}
