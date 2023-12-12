// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UINotifyManagerWidget.h"

#include "VeUI.h"
#include "Components/PanelWidget.h"
#include "UINotifyItemWidget.h"
#include "UIAnimationWidget.h"

void UUINotifyManagerWidget::AddItem(UUINotifyItemWidget* NotificationWidget) {
	if (!IsValid(ContainerWidget)) {
		return;
	}

	auto* AnimationWidget = CreateAnimationWidget(NotificationWidget);
	if (!AnimationWidget) {
		return;
	}

	if (!MaxVisibleNotifications || ContainerWidget->GetChildrenCount() < MaxVisibleNotifications) {
		auto* ResultSlot = ContainerWidget->AddChild(AnimationWidget);
		if (ResultSlot) {
			NativeOnChildAdded(ResultSlot);
			AnimationWidget->Show();
			return;
		}
	}

	// Add notification to queue
	NotificationQueue.Add(AnimationWidget);
}

UUIAnimationWidget* UUINotifyManagerWidget::CreateAnimationWidget(UUINotifyItemWidget* NotificationWidget) {
	if (!IsValid(NotificationWidget)) {
		return nullptr;
	}

	UUIAnimationWidget* AnimationWidget = nullptr;
	if (IsValid(NotificationWidget->AnimationClass)) {
		AnimationWidget = CreateWidget<UUIAnimationWidget>(NotificationWidget->AnimationClass);
	} else if (IsValid(DefaultAnimationClass)) {
		AnimationWidget = CreateWidget<UUIAnimationWidget>(DefaultAnimationClass);
	}

	if (!AnimationWidget) {
		VeLogError("Error animation widget class.");
		return nullptr;
	}

	AnimationWidget->SetContentWidget(NotificationWidget);
	NotificationWidget->SetAnimationWidget(AnimationWidget);

	AnimationWidget->GetOnShowStarted().AddUObject(this, &UUINotifyManagerWidget::OnShowStartedCallback);
	AnimationWidget->GetOnShowFinished().AddUObject(this, &UUINotifyManagerWidget::OnShowFinishedCallback);
	AnimationWidget->GetOnHideStarted().AddUObject(this, &UUINotifyManagerWidget::OnHideStartedCallback);
	AnimationWidget->GetOnHideFinished().AddUObject(this, &UUINotifyManagerWidget::OnHideFinishedCallback);

	return AnimationWidget;
}

void UUINotifyManagerWidget::OnShowStartedCallback(UUIAnimationWidget* InAnimationWidget) {
	InAnimationWidget->GetContentWidget<UUINotifyItemWidget>()->NativeOnShowStarted();
}

void UUINotifyManagerWidget::OnShowFinishedCallback(UUIAnimationWidget* InAnimationWidget) {
	InAnimationWidget->GetContentWidget<UUINotifyItemWidget>()->NativeOnShowFinished();
}

void UUINotifyManagerWidget::OnHideStartedCallback(UUIAnimationWidget* InAnimationWidget) {
	InAnimationWidget->GetContentWidget<UUINotifyItemWidget>()->NativeOnHideStarted();
}

void UUINotifyManagerWidget::OnHideFinishedCallback(UUIAnimationWidget* InAnimationWidget) {
	InAnimationWidget->GetContentWidget<UUINotifyItemWidget>()->NativeOnHideFinished();

	if (IsValid(InAnimationWidget)) {
		InAnimationWidget->RemoveFromParent();
	}

	// Show notification from queue
	if (NotificationQueue.Num()) {
		auto* AnimationWidget = NotificationQueue[0];
		auto* ResultSlot = ContainerWidget->AddChild(AnimationWidget);
		if (ResultSlot) {
			NativeOnChildAdded(ResultSlot);
			AnimationWidget->Show();
		}
		NotificationQueue.Remove(AnimationWidget);
	}
}
