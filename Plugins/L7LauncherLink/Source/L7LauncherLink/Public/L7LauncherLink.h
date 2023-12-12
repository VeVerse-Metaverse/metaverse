// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "L7LLSocketConnectionManager.h"
#include "Modules/ModuleManager.h"

L7LAUNCHERLINK_API DECLARE_LOG_CATEGORY_EXTERN(LogL7LauncherLink, All, All);

class FL7LauncherLinkModule : public IModuleInterface {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TUniquePtr<FL7LLSocketConnectionManager> ConnectionManager;
};
