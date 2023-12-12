// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Subsystems/UINotificationSubsystem.h"

#include "UIRootWidget.h"
#include "VR/VRRootWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


const FName FUINotificationSubsystem::Name = FName("UINotificationSubsystem");

void FUINotificationSubsystem::AddNotification(const FUINotificationData& NotificationData) const {
	OnNotification.Broadcast(NotificationData);
	
	if (VRRootWidget.IsValid()) {
		VRRootWidget->AddNotification(NotificationData);
	}
}

void FUINotificationSubsystem::SetRootWidget(UVRRootWidget* InRootWidget) {
	VRRootWidget = InRootWidget;
}

void FUINotificationSubsystem::Initialize() {
}

void FUINotificationSubsystem::Shutdown() {
}

#undef LOCTEXT_NAMESPACE