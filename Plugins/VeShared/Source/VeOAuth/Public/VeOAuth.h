// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VeShared.h"

class VEOAUTH_API FVeOAuthModule final : public ISubsystemModule {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * @brief Returns the singleton instance of this module. Initializes the module on the first call.
	 */
	static FVeOAuthModule* Get();

	/**
	 * @brief Returns the OAuth subsystem.
	 */
	TSharedPtr<class FOAuthOAuthSubsystem> GetOAuthSubsystem();
};
