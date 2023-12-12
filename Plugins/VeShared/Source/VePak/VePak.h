// Author: Egor A. Pristavka

#pragma once

#include "PakPakSubsystem.h"
#include "VeShared.h"

VEPAK_API DECLARE_LOG_CATEGORY_EXTERN(LogVePak, All, All);

class VEPAK_API FVePakModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVePakModule* Get();

	FPakSubsystemPtr GetPakSubsystem();
};
