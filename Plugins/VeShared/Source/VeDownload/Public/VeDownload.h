// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Modules/ModuleManager.h"
#include "VeShared.h"

VEDOWNLOAD_API DECLARE_LOG_CATEGORY_EXTERN(LogVeDownload, All, All);

class VEDOWNLOAD_API FVeDownloadModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVeDownloadModule* Get() {
		if (FModuleManager::Get().IsModuleLoaded(TEXT("VeDownload"))) {
			return FModuleManager::Get().GetModulePtr<FVeDownloadModule>(TEXT("VeDownload"));
		}
		return FModuleManager::Get().LoadModulePtr<FVeDownloadModule>(TEXT("VeDownload"));
	}

	TSharedPtr<class FDownloadChunkSubsystem> GetChunkSubsystem() const;
	TSharedPtr<class FDownloadTextureSubsystem> GetTextureSubsystem() const;
	TSharedPtr<class FDownloadDownload1Subsystem> GetDownload1Subsystem() const;
	TSharedPtr<class FDownloadHttpDownloadSubsystem> GetHttpDownloadSubsystem() const;
	TSharedPtr<class FDownloadFileStorageSubsystem> GetFileStorageSubsystem() const;
};
