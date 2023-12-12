// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeShared.h"

typedef TDelegate<void(const FString& Token, const bool bInSuccessful, const FString& InError)> FOnOAuthCompleted;

/** @brief The subsystem to manage OAuth helper process and perform OAuth using different providers. */
class VEOAUTH_API FOAuthOAuthSubsystem final : public IModuleSubsystem {
public:
	virtual void Initialize() override;
	virtual void Shutdown() override;

	void CopyOAuthUrlToClipboard(const FString &Buffer);

	const static FName Name;

	/** @brief Starts the external login process to perform OAuth using the given provider. */
	void Login(const FString& Provider, const TSharedRef<FOnOAuthCompleted>& InCallback);

	/** @brief Returns the OAuth link provided by the helper process. */
	FString GetOAuthLink() const {
		return OAuthLink;
	}

private:
	/** @brief Ticker delegate to check the status of the helper process and read the data from the pipe. */
	FTickerDelegate TickerDelegate;

	/** @brief Handle to the ticker delegate. Used to stop the ticker when the helper process is finished and clean up the delegate. */
	FTSTicker::FDelegateHandle TickerDelegateHandle;

	/** @brief Handle to the helper process, valid while the process is running. */
	FProcHandle ProcessHandle;

	/** @brief Read pipe handle to read the data from the helper process. */
	void* ReadPipe = nullptr;

	/** @brief Dynamic buffer to store the data read from the pipe. */
	FString ReadPipeBuffer = {};

	/** @brief Link for the manual authorization if automatic authorization failed (to copy to the buffer). */
	FString OAuthLink = {};

	/** @brief The token extracted from the JSON returned by the helper process. */
	FString Token = {};

	/** @brief Returns true if the external app is still running. */
	bool IsCurrentlyProcessing() const {
		return ProcessHandle.IsValid();
	}
};
