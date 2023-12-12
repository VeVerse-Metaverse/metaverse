// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeShared.h"

class FVeTestModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVeTestModule* Get();

	static void LogPakFilesCommand(UWorld* InWorld);
	static void LoadPakMapCommand(const TArray<FString>& InArgs, UWorld* InWorld);
};
