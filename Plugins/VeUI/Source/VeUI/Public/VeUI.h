// Author: Egor A. Pristavka
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once
#include "VeShared.h"

class FUIPlaceableSubsystem;
class FUICharacterCustomizationSubsystem;
class FUINavigationSubsystem;
class FUIDialogSubsystem;
class FUIBreadcrumbSubsystem;
class FUIFocusSubsystem;
class FUINotificationSubsystem;
class FUIPageSubsystem;


VEUI_API DECLARE_LOG_CATEGORY_EXTERN(LogVeUI, All, All);


class VEUI_API FVeUIModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVeUIModule* Get();

	TSharedPtr<FUIBreadcrumbSubsystem> GetBreadcrumbSubsystem();
	TSharedPtr<FUIFocusSubsystem> GetFocusSubsystem();
	TSharedPtr<FUIDialogSubsystem> GetDialogSubsystem();
	TSharedPtr<FUINotificationSubsystem> GetNotificationSubsystem();
	TSharedPtr<FUINavigationSubsystem> GetNavigationSubsystem();
	TSharedPtr<FUICharacterCustomizationSubsystem> GetCharacterCustomizationSubsystem();
	TSharedPtr<FUIPageSubsystem> GetPageSubsystem();
};
