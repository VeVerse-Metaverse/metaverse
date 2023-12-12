// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "VeMonitoring/Public/MonitoringMonitoringSubsystem.h"
#include "Engine/World.h"
#include "Engine.h"
#include "VeShared.h"

#if PLATFORM_IOS || PLATFORM_MAC || PLATFORM_ANDROID
#include "BugsnagFunctionLibrary.h"
#endif

#if PLATFORM_WINDOWS
#include "VeSentry/VeSentryModule.h"
#endif

FName FMonitoringMonitoringSubsystem::Name{TEXT("MonitoringSubsystem")};

void FMonitoringMonitoringSubsystem::Initialize() {
	if (GEngine) {
		if (!OnEngineTravelFailureDelegateHandle.IsValid()) {
			OnEngineTravelFailureDelegateHandle = GEngine->OnTravelFailure().AddRaw(this, &FMonitoringMonitoringSubsystem::OnEngineTravelFailure);
		}

		if (!OnEngineNetworkFailureDelegateHandle.IsValid()) {
			OnEngineNetworkFailureDelegateHandle = GEngine->OnNetworkFailure().AddRaw(this, &FMonitoringMonitoringSubsystem::OnEngineNetworkFailure);
		}
	} else {
		ReportMessage(FString(TEXT("Engine")), FString(TEXT("error")), FString(TEXT("GEngine is nullptr")));
	}
}

void FMonitoringMonitoringSubsystem::Shutdown() {
	if (GEngine) {
		if (OnEngineTravelFailureDelegateHandle.IsValid()) {
			GEngine->OnTravelFailure().Remove(OnEngineTravelFailureDelegateHandle);
			OnEngineTravelFailureDelegateHandle.Reset();
		}

		if (OnEngineNetworkFailureDelegateHandle.IsValid()) {
			GEngine->OnNetworkFailure().Remove(OnEngineNetworkFailureDelegateHandle);
			OnEngineNetworkFailureDelegateHandle.Reset();
		}
	}
}

void FMonitoringMonitoringSubsystem::SetUser(const FVeMonitoringUser& InUserMetadata) {
	CurrentUser = InUserMetadata;

#if PLATFORM_IOS || PLATFORM_MAC || PLATFORM_ANDROID 
	UBugsnagFunctionLibrary::SetUser(InUserMetadata.Id, InUserMetadata.Email, InUserMetadata.Name);
#endif

#if PLATFORM_WINDOWS
	if (FVeSentryModule* SentryModule = FVeSentryModule::Get()) {
		SentryModule->SetUser(InUserMetadata.Id, InUserMetadata.Email, InUserMetadata.Name);
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Sentry)); }
#endif
}

void FMonitoringMonitoringSubsystem::ReportMessage(const FString& InCategory, const FString& InLevel, const FString& InMessage) {
#if PLATFORM_IOS || PLATFORM_MAC || PLATFORM_ANDROID 
	UBugsnagFunctionLibrary::Notify(InCategory, InMessage);
#endif

#if PLATFORM_WINDOWS
	if (FVeSentryModule* SentryModule = FVeSentryModule::Get()) {
		SentryModule->SendMessage(InCategory, InLevel, InMessage);
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Sentry)); }
#endif
}

void FMonitoringMonitoringSubsystem::OnEngineTravelFailure(UWorld* InWorld, ETravelFailure::Type InFailureType, const FString& InError) {
	if (IsValid(InWorld)) {
		ReportMessage(FString(TEXT("Network")), TEXT("error"),
					  FString::Printf(TEXT("travel failure; world: %s, failure type: %s, error: %s"), *InWorld->GetMapName(), ETravelFailure::ToString(InFailureType), *InError)
		);
	} else {
		ReportMessage(FString(TEXT("Network")), TEXT("error"),
					  FString::Printf(TEXT("travel failure; world: NULL, failure type: %s, error: %s"), ETravelFailure::ToString(InFailureType), *InError)
		);
	}
}

void FMonitoringMonitoringSubsystem::OnEngineNetworkFailure(UWorld* InWorld, UNetDriver* InNetDriver, ENetworkFailure::Type InFailureType, const FString& InError) {
	if (IsValid(InWorld)) {
		ReportMessage(FString(TEXT("Network")), TEXT("error"), FString::Printf(
						  TEXT("network failure; world: %s, net driver: %s, failure type: %s, error: %s"), *InWorld->GetMapName(), *InNetDriver->GetName(),
						  ENetworkFailure::ToString(InFailureType), *InError)
		);
	} else {
		ReportMessage(FString(TEXT("Network")), TEXT("error"), FString::Printf(
						  TEXT("network failure; world: NULL, net driver: %s, failure type: %s, error: %s"), *InNetDriver->GetName(),
						  ENetworkFailure::ToString(InFailureType), *InError)
		);
	}
}
