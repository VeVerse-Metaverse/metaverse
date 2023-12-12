// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "VeShared.h"
#include "UINotificationData.h"

class UVRRootWidget;
class UUIRootWidget;


class VEUI_API FUINotificationSubsystem final : public IModuleSubsystem {
	DECLARE_EVENT_OneParam(FUINotificationSubsystem, FNotificationEvent, const FUINotificationData& NotificationData);
	
public:
	const static  FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;
	
	/** Add a notification to the notification area. */
	void AddNotification(const FUINotificationData& NotificationData) const;

	void SetRootWidget(UVRRootWidget* InRootWidget);

	FNotificationEvent OnNotification;
	
private:
	/** Root widget for the UI. */
	TWeakObjectPtr<UVRRootWidget> VRRootWidget = nullptr;
	
};

typedef TSharedPtr<FUINotificationSubsystem> FUINotificationSubsystemPtr;
