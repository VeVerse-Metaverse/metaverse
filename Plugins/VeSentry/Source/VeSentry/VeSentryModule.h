// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Modules/ModuleManager.h"

VESENTRY_API DECLARE_LOG_CATEGORY_EXTERN(LogVeSentry, All, All);

class VESENTRY_API FVeSentryModule final : public IModuleInterface {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if WITH_SENTRY
	void* _sentrySdkHandle;

	void* LoadSharedLibrary(const TCHAR* libraryName);
#endif

	void SetUser(const FString& InId, const FString& InEmail, const FString& InName);

	void SendMessage(const FString& InCategory, const FString& InLevel, const FString& InMessage);

	static FVeSentryModule* Get() {
		if (FModuleManager::Get().IsModuleLoaded(TEXT("VeSentry"))) {
			return FModuleManager::Get().GetModulePtr<FVeSentryModule>(TEXT("VeSentry"));
		}
		return FModuleManager::Get().LoadModulePtr<FVeSentryModule>(TEXT("VeSentry"));
	}
};
