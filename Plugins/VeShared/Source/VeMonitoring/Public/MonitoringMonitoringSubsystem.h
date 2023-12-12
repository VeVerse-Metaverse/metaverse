// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "VeShared.h"
#include "Engine.h"

struct FVeMonitoringUser {
	FString Id{};
	FString Email{};
	FString Name{};
};

class VEMONITORING_API FMonitoringMonitoringSubsystem : public IModuleSubsystem {
public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/** Set a user to be used for reporting errors. */
	void SetUser(const FVeMonitoringUser& InUserMetadata);

	/** Report a custom message or warning. */
	void ReportMessage(const FString& InCategory, const FString& InLevel, const FString& InMessage);

private:
	FVeMonitoringUser CurrentUser;

	FDelegateHandle OnEngineTravelFailureDelegateHandle;
	FDelegateHandle OnEngineNetworkFailureDelegateHandle;

	void OnEngineTravelFailure(UWorld* InWorld, ETravelFailure::Type InFailureType, const FString& InError);
	void OnEngineNetworkFailure(UWorld* InWorld, UNetDriver* InNetDriver, ENetworkFailure::Type InFailureType, const FString& InError);
};
