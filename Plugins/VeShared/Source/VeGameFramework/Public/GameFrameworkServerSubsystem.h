// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "VeShared.h"
#include "VeServerMetadata.h"
#include "VeWorldMetadata.h"

class FApiSpaceMetadata;
class AVeGameStateBase;

#define OLDVERSION 0


class VEGAMEFRAMEWORK_API FGameFrameworkServerSubsystem final : public IModuleSubsystem {

	typedef TDelegate<void(const FApiSpaceMetadata& InMetadata, bool bSuccessful)> FOnSpaceMetadataDelegate;

	virtual void Initialize() override;
	virtual void Shutdown() override;

public:
	static FName Name;

	const FGuid& GetServerId() const { return ServerId; }
	const FGuid& GetWorldId() const { return WorldId; }

protected:
	/**
	 * Called on initial startup, requests server information to travel to the scheduled map.
	 */
	void Startup_Start();

	/**
	 * Request world metadata designated for startup via environment.
	 */
	void Startup_RequestDesignatedWorldMetadata(TSharedRef<FOnSpaceMetadataDelegate> InCallback, const FGuid& ServerWorldId);

	/**
	 * Request scheduled world id from the API and then travel to the scheduled map.
	 */
	void Startup_RequestScheduledWorldMetadata(TSharedRef<FOnSpaceMetadataDelegate> InCallback);

	/**
	 * Request a default world metadata.
	 * @note Used as a fallback if failed to get a scheduled or designated world.
	 */
	void Startup_RequestDefaultWorldMetadata(TSharedRef<FOnSpaceMetadataDelegate> InCallback);

	UWorld* GetWorld() const { return CurrentWorld.Get(); }

	void OnError(const FString& ErrMessage = TEXT(""));

	float GracefulShutdownDelay = 600.0f;
	void StartTimerForUnregisterServer();
	void StopTimerForUnregisterServer();
	FTimerHandle GracefulShutdownTimerHandle;

private:;
	bool Stage_Start_World = false;
	bool Stage_Start_Login = false;

	FDelegateHandle AuthSubsystem_OnChangeAuthorizationStateHandle;

	void ServerTravel(const FString& Map, const FString& GameMode = TEXT(""));

	/**
	 * Called when initial startup completed so we can provide world and server IDs to the game state.
	 */
	void ServerTravel_OnWorldInitializedActors(const UWorld::FActorsInitializedParams& InInitializationValues);

#pragma region World and Server Metadata

private:
	/**
	 * World metadata received.
	 */
	void Startup_OnWorldMetadataReceived(const FApiSpaceMetadata& InMetadata);

	void Startup_MountPackage();

	void Startup_OnPackageMounted();

	TSharedPtr<class FGameFrameworkWorldItem> WorldItem;

	/** Server Id. */
	FGuid ServerId;

	/** Space Id. */
	FGuid WorldId;

	TWeakObjectPtr<UWorld> CurrentWorld;
	
	FDelegateHandle OnPostWorldInitializationDelegateHandle;

#pragma endregion

#pragma region Registration and Heartbeats

private:
	/** Start sending server heartbeats after server has been initialized and registered. */
	void Heartbeats_Start();

	/** Stop sending heartbeats on shutdown. */
	void Heartbeats_Stop();

	/** Send a heartbeat request notifying the backend that the server is still online and ready to accept connections. */
	void Heartbeats_SendHeartbeat();

	/** Timer handle for sending server heartbeats. */
	FTimerHandle Heartbeat_TimerHandle;

protected:
	void UnregisterServer();

#pragma endregion

#pragma region User Handling

protected:
	/** Registers user with the server by adding to the server players. */
	void GameMode_OnUserLogin(APlayerController* InPlayerController, const FGuid& UserId);

	/** Removes user from the server players. */
	void GameMode_OnUserLogout(APlayerController* InPlayerController, const FGuid& UserId);

#pragma endregion

};

typedef TSharedPtr<FGameFrameworkServerSubsystem> FGameFrameworkServerSubsystemPtr;
