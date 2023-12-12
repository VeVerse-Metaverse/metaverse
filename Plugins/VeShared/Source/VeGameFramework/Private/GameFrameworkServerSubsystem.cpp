// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "GameFrameworkServerSubsystem.h"

#include "PakPakSubsystem.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2WorldSubsystem.h"
#include "Api2ServerSubsystem.h"
#include "VeEnvironmentSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "VeGameFramework.h"
#include "GameFrameworkWorldItem.h"
#include "GameFrameworkWorldSubsystem.h"
#include "VeGameModeBase.h"

#undef VE_LOG_MODULE
#define VE_LOG_MODULE ServerSubsystem


// Default space id (Plaza).
const static FGuid DefaultServerSpaceId = FGuid(TEXT("63715884-b498-4c3e-8afe-5535a2b8c392"));
constexpr float ServerHeartbeatInterval = 60.f;

FName FGameFrameworkServerSubsystem::Name = TEXT("GameFrameworkServerSubsystem");

void FGameFrameworkServerSubsystem::Initialize() {
	if (!IsRunningDedicatedServer()) {
		return;
	}

	VeLog("Initialization");

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (!AuthSubsystem) {
		OnError();
		return;
	}

	FApiUserLoginMetadata Metadata;
	Metadata.Email = FVeConfigLibrary::GetServerApiEmail();
	Metadata.Password = FVeConfigLibrary::GetServerApiPassword();

	if (Metadata.Email.IsEmpty() || Metadata.Password.IsEmpty()) {
		const FString ErrorStr = TEXT("Failed to authorization. Check the environments");
		VeLogError("%s", *ErrorStr);
		OnError(ErrorStr);
		return;
	}

	ServerId = FVeConfigLibrary::GetSeverId();
	WorldId = FVeConfigLibrary::GetSeverWorldId();

	if (!OnPostWorldInitializationDelegateHandle.IsValid()) {
		OnPostWorldInitializationDelegateHandle = FWorldDelegates::OnPostWorldInitialization.AddLambda([=](UWorld* InWorld, UWorld::InitializationValues InInitializationValues) {
			if (!InWorld->GetGameInstance()) {
				return;
			}

			CurrentWorld = InWorld;

			VeLog("World initialization successful: %s", *InWorld->GetMapName());

			Stage_Start_World = true;
			Startup_Start();
		});
	}

	auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
		if (AuthSubsystem->IsAuthorized()) {
			VeLog("Authorization successful");

			Stage_Start_Login = true;
			Startup_Start();
		} else {
			const FString ErrorStr = TEXT("Failed to authorization");
			VeLogError("%s", *ErrorStr);
			OnError(ErrorStr);
		}
	};

	if (AuthSubsystem->IsAuthorized()) {
		OnChangeAuthorizationStateCallback(AuthSubsystem->GetAuthorizationState());
	} else {
		AuthSubsystem_OnChangeAuthorizationStateHandle = AuthSubsystem->GetOnChangeAuthorizationState().AddLambda(OnChangeAuthorizationStateCallback);
		AuthSubsystem->Login(Metadata);
	}
}

void FGameFrameworkServerSubsystem::Shutdown() {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		if (AuthSubsystem_OnChangeAuthorizationStateHandle.IsValid()) {
			AuthSubsystem->GetOnChangeAuthorizationState().Remove(AuthSubsystem_OnChangeAuthorizationStateHandle);
		}
	}

	if (OnPostWorldInitializationDelegateHandle.IsValid()) {
		FWorldDelegates::OnPostWorldInitialization.Remove(OnPostWorldInitializationDelegateHandle);
	}
}

void FGameFrameworkServerSubsystem::Startup_Start() {
	if (!Stage_Start_World || !Stage_Start_Login) {
		return;
	}

	if (OnPostWorldInitializationDelegateHandle.IsValid()) {
		FWorldDelegates::OnPostWorldInitialization.Remove(OnPostWorldInitializationDelegateHandle);
		OnPostWorldInitializationDelegateHandle.Reset();
	}

	VeLog("Start server initialization");

	const auto Callback = MakeShared<FOnSpaceMetadataDelegate>();
	Callback->BindLambda([=](const FApiSpaceMetadata& InMetadata, bool bSuccessful) {
		if (bSuccessful) {
			Startup_OnWorldMetadataReceived(InMetadata);
		} else {
			OnError("Failed to get WorldMetadata");
		}
	});

	// Try to request a designated world.
	if (WorldId.IsValid()) {
		Startup_RequestDesignatedWorldMetadata(Callback, WorldId);
		return;
	}

	// Try to request a scheduled world.
	Startup_RequestScheduledWorldMetadata(Callback);
}

void FGameFrameworkServerSubsystem::Startup_RequestDesignatedWorldMetadata(TSharedRef<FOnSpaceMetadataDelegate> InCallback, const FGuid& ServerWorldId) {
	GET_MODULE_SUBSYSTEM(WorldSubsystem, Api2, World);
	if (!WorldSubsystem) {
		InCallback->ExecuteIfBound(FApiSpaceMetadata(), false);
		return;
	}

	const auto Callback = MakeShared<FOnWorldRequestCompleted2>();
	Callback->BindLambda([=](const FApiSpaceMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (bInSuccessful && InMetadata.Id.IsValid()) {
			InCallback->ExecuteIfBound(InMetadata, true);
			return;
		}

		VeLogError("Failed to get a designated space: %s, falling back to a scheduled space", *InError);

		if (IsRunningDedicatedServer()) {
			Startup_RequestScheduledWorldMetadata(InCallback);
		} else {
			Startup_RequestDefaultWorldMetadata(InCallback);
		}
	});

	WorldSubsystem->Get(ServerWorldId, Callback);
}

void FGameFrameworkServerSubsystem::Startup_RequestScheduledWorldMetadata(TSharedRef<FOnSpaceMetadataDelegate> InCallback) {
	GET_MODULE_SUBSYSTEM(ServerSubsystem, Api2, Server);
	if (!ServerSubsystem) {
		InCallback->ExecuteIfBound(FApiSpaceMetadata(), false);
		return;
	}

	const auto Callback = MakeShared<FOnWorldRequestCompleted2>();
	Callback->BindLambda([=](const FApiSpaceMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (bInSuccessful && InMetadata.Id.IsValid()) {
			InCallback->ExecuteIfBound(InMetadata, true);
			return;
		}

		VeLogError("Failed to get scheduled space: %s, falling back to a default space", *InError);

		Startup_RequestDefaultWorldMetadata(InCallback);
	});

	ServerSubsystem->GetScheduledWorld(Callback);
}

void FGameFrameworkServerSubsystem::Startup_RequestDefaultWorldMetadata(TSharedRef<FOnSpaceMetadataDelegate> InCallback) {
	GET_MODULE_SUBSYSTEM(WorldSubsystem, Api2, World);
	if (!WorldSubsystem) {
		InCallback->ExecuteIfBound(FApiSpaceMetadata(), false);
		return;
	}

	const auto Callback = MakeShared<FOnWorldRequestCompleted2>();
	Callback->BindLambda([=](const FApiSpaceMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (bInSuccessful && InMetadata.Id.IsValid()) {
			InCallback->ExecuteIfBound(InMetadata, true);
			return;
		}

		VeLogError("Failed to get a default space: %s", *InError);

		InCallback->ExecuteIfBound(FApiSpaceMetadata(), false);
	});

	WorldSubsystem->Get(DefaultServerSpaceId, Callback);
}

void FGameFrameworkServerSubsystem::Startup_OnWorldMetadataReceived(const FApiSpaceMetadata& InMetadata) {
	GET_MODULE_SUBSYSTEM(WorldSubsystem, GameFramework, World);
	if (!WorldSubsystem) {
		OnError(TEXT("Failed to get module GameFrameworkWorldSubsystem"));
		return;
	}

	WorldId = InMetadata.Id;

	// Get WorldItem
	WorldItem = WorldSubsystem->GetItem(WorldId, InMetadata);

	Startup_MountPackage();
}

void FGameFrameworkServerSubsystem::Startup_MountPackage() {
	const auto Callback = MakeShared<FOnPackageState>();
	Callback->BindLambda([=](EVePackageState InState) {
		if (InState == EVePackageState::Mounted) {
			Startup_OnPackageMounted();
			return;
		}

		const FString ErrorMsg = FString::Printf(TEXT("Failed to mount the package '%s'"), *WorldItem->GetMetadata().Name);
		VeLogError("%s", *ErrorMsg);
		OnError(ErrorMsg);
	});

	switch (WorldItem->GetState()) {
	case EVeWorldState::PendingDownloadPackage: {
		WorldItem->DownloadPackage(true, Callback);
		return;
	}
	case EVeWorldState::PackageDownloaded: {
		WorldItem->MountPackage(Callback);
		return;
	}
	case EVeWorldState::PackageMounted: {
		Callback->ExecuteIfBound(EVePackageState::Mounted);
		return;
	}
	default: {
		const FString ErrorMsg = FString::Printf(TEXT("Error: WorldItem state is %d"), WorldItem->GetState());
		VeLogError("%s", *ErrorMsg);
		OnError(ErrorMsg);
	}
	}
}

void FGameFrameworkServerSubsystem::Startup_OnPackageMounted() {
	ServerTravel(WorldItem->GetMetadata().Map, WorldItem->GetMetadata().GameMode);
}

void FGameFrameworkServerSubsystem::OnError(const FString& ErrMessage) {
	VeLogError("Fatal Error");
	UE_LOG(LogCore, Log, TEXT("## ExitCode: 1"));
	UE_LOG(LogCore, Log, TEXT("## ExitMessage: %s"), *ErrMessage);
	FPlatformMisc::RequestExit(false);
}

void FGameFrameworkServerSubsystem::StartTimerForUnregisterServer() {
	if (CurrentWorld.IsValid()) {
		VeLog("Start timer for unregister server");
		CurrentWorld->GetTimerManager().SetTimer(GracefulShutdownTimerHandle, [=] {
			UnregisterServer();
		}, GracefulShutdownDelay, false);
	}
}

void FGameFrameworkServerSubsystem::StopTimerForUnregisterServer() {
	if (GracefulShutdownTimerHandle.IsValid()) {
		if (CurrentWorld.IsValid()) {
			VeLog("Stop timer for unregister server");
			CurrentWorld->GetTimerManager().ClearTimer(GracefulShutdownTimerHandle);
			GracefulShutdownTimerHandle.Invalidate();
		}
	}
}

void FGameFrameworkServerSubsystem::UnregisterServer() {
	if (!ServerId.IsValid()) {
		return;
	}

	VeLog("Start to unregister server");

	Heartbeats_Stop();

	const auto Callback = MakeShared<FOnGenericRequestCompleted2>();
	Callback->BindLambda([=](const EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (!bSuccessful) {
			VeLogError("Failed to unregister server: %s", *InError);
		} else {
			VeLog("Server unregistred");
		}
	});

	GET_MODULE_SUBSYSTEM(ServerSubsystem, Api2, Server);
	if (ServerSubsystem) {
		ServerSubsystem->Delete(ServerId, Callback);
	} else {
		Callback->ExecuteIfBound(EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FGameFrameworkServerSubsystem::GameMode_OnUserLogin(APlayerController* InPlayerController, const FGuid& UserId) {
	// todo: request to the api to add user to the server players list

	StopTimerForUnregisterServer();
}

void FGameFrameworkServerSubsystem::GameMode_OnUserLogout(APlayerController* InPlayerController, const FGuid& UserId) {
	// todo: request to the api to remove the user from the server players list

	if (auto* VeGameModeBase = CurrentWorld->GetAuthGameMode<AVeGameModeBase>()) {
		const int32 NumPlayers = VeGameModeBase->GetNumPlayers();
		if (NumPlayers == 1) {
			StartTimerForUnregisterServer();
		}
	}
}

#pragma region Heartbeats

void FGameFrameworkServerSubsystem::Heartbeats_Start() {
	auto* World = GetWorld();
	if (!World) {
		return;
	}

	const auto* GameInstance = World->GetGameInstance();
	if (!GameInstance) {
		return;
	}

	VeLog("Started sending heartbeats");

	GameInstance->GetTimerManager().SetTimer(Heartbeat_TimerHandle, [=]() {
		Heartbeats_SendHeartbeat();
	}, ServerHeartbeatInterval, true);

	Heartbeats_SendHeartbeat();
}

void FGameFrameworkServerSubsystem::Heartbeats_Stop() {
	VeLog("Stopped sending heartbeats");

	auto* World = GetWorld();
	if (!World) {
		return;
	}

	const auto* GameInstance = World->GetGameInstance();
	if (!GameInstance) {
		return;
	}

	GameInstance->GetTimerManager().ClearTimer(Heartbeat_TimerHandle);
}

void FGameFrameworkServerSubsystem::Heartbeats_SendHeartbeat() {
	VeLog("Sending heartbeat");

	if (!ServerId.IsValid()) {
		VeLogError("Server id is invalid");
		Heartbeats_Stop();
		return;
	}

	const auto Callback = MakeShared<FOnGenericRequestCompleted2>();
	Callback->BindLambda([=](const EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (!bSuccessful) {
			VeLogError("Failed to send heartbeat: %s", *InError);
		}
	});

	GET_MODULE_SUBSYSTEM(ServerSubsystem, Api2, Server)
	if (ServerSubsystem) {
		FApi2RequestServerHeartbeatMetadata RequestMetadata;
		RequestMetadata.Status = EApi2ServerStatus::Online;

		ServerSubsystem->Heartbeat(ServerId, RequestMetadata, Callback);
	} else {
		Callback->ExecuteIfBound(EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

#pragma endregion Heartbeats

void FGameFrameworkServerSubsystem::ServerTravel(const FString& Map, const FString& GameMode) {
	auto* World = GetWorld();
	if (!World) {
		const FString ErrorStr = FString::Printf(TEXT("Travel to %s canceled: failed to get world."), *Map);
		VeLogErrorFunc("%s", *ErrorStr);
		OnError(ErrorStr);
		return;
	}

	OnPostWorldInitializationDelegateHandle = FWorldDelegates::OnWorldInitializedActors.AddRaw(this, &FGameFrameworkServerSubsystem::ServerTravel_OnWorldInitializedActors);

	if (GameMode.IsEmpty()) {
		// Start game with default mode
		World->ServerTravel(FString::Printf(TEXT("%s?WorldId=%s"), *Map, *WorldId.ToString(EGuidFormats::DigitsWithHyphensLower)), true);
	} else {
		// Start game with assigned mode
		World->ServerTravel(FString::Printf(TEXT("%s?WorldId=%s?Game=%s"), *Map, *WorldId.ToString(EGuidFormats::DigitsWithHyphensLower), *GameMode), true);
	}
}

void FGameFrameworkServerSubsystem::ServerTravel_OnWorldInitializedActors(const UWorld::FActorsInitializedParams& InInitializationValues) {
	if (OnPostWorldInitializationDelegateHandle.IsValid()) {
		FWorldDelegates::OnWorldInitializedActors.Remove(OnPostWorldInitializationDelegateHandle);
		OnPostWorldInitializationDelegateHandle.Reset();
	}

	auto* InWorld = InInitializationValues.World;

	if (!InWorld) {
		const FString ErrorStr = FString::Printf(TEXT("Travel canceled: failed to get world."));
		VeLogError("%s", *ErrorStr);
		OnError(ErrorStr);
		return;
	}

	CurrentWorld = InWorld;

	if (auto* VeGameModeBase = CurrentWorld->GetAuthGameMode<AVeGameModeBase>()) {
		VeGameModeBase->OnUserLogin.AddRaw(this, &FGameFrameworkServerSubsystem::GameMode_OnUserLogin);
		VeGameModeBase->OnUserLogout.AddRaw(this, &FGameFrameworkServerSubsystem::GameMode_OnUserLogout);
		StartTimerForUnregisterServer();
	} else {
		const FString ErrorStr = FString::Printf(TEXT("Travel canceled: failed to get VeGameModeBase."));
		VeLogError("%s", *ErrorStr);
		OnError(ErrorStr);
		return;
	}

	Heartbeats_Start();
}
