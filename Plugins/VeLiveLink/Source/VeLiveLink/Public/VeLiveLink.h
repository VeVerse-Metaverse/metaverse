// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

VELIVELINK_API DECLARE_LOG_CATEGORY_EXTERN(LogVeLiveLink, Log, All);

class UVeLiveLinkLauncher;

class FVeLiveLinkModule : public IModuleInterface {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void DestroyLiveLinkStarter(); 

private:
	UVeLiveLinkLauncher* LiveLinkLauncher = nullptr;
};
