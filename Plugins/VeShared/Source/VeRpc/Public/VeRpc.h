// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeShared.h"

VERPC_API DECLARE_LOG_CATEGORY_EXTERN(LogVeRpc, All, All);

class VERPC_API FVeRpcModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVeRpcModule* Get();

	TSharedPtr<class FRpcWebSocketsServiceSubsystem> GetWebSocketsServiceSubsystem();
	TSharedPtr<class FRpcWebSocketsTextChatSubsystem> GetWebSocketsTextChatSubsystem();
};
