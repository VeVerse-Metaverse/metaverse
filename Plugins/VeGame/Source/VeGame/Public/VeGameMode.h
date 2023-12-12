// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "PlaceableActor.h"
#include "VePlaceableMetadata.h"
#include "VeGameModeBase.h"
#include "VeGameMode.generated.h"

class AVePortal;
class AVePlayerController;

/**
 * Base game mode class.
 */
UCLASS(Blueprintable, BlueprintType)
class VEGAME_API AVeGameMode final : public AVeGameModeBase {
	GENERATED_BODY()

	AVeGameMode();

	virtual void PostInitializeComponents() override;

	/**
	 * Handles game startup for online server and standalone cases.
	 * @remarks Registers server and make server travel to the desired map.
	 */
	virtual void BeginPlay() override;

	/** Handles game shutdown for online server and standalone cases. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Handles the connection of a new player.
	 * Parses UserId from connection options and provides it to the player controller UserComponent. 
	 */
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueNetId, FString& ErrorMessage) override;

	/** Handles the disconnection of the player. */
	virtual void Logout(AController* Exiting) override;

	/** Checks if the user with specified id exists at the server and allowed to login. */
	bool Login_RequestUserMetadata(APlayerController* PlayerController, const FGuid& PlayerId);

	/** Find portal or default player state with fallback to random player state as a last resort. */
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

	/** Determine the default pawn class depending on controller input mode. */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	/** Override HUD initialization as we don't need HUD to be spawned for VR players. */
	virtual void InitializeHUDForPlayer_Implementation(APlayerController* NewPlayer) override;

public:
	/** Request to kick the user from the server. Server-side only. */
	UFUNCTION(BlueprintAuthorityOnly)
	bool Authority_RequestUserKick(const FGuid& RequesterUserId, APlayerController* PlayerController);

	/** List of cached static map portals initialized at begin play. */
	UPROPERTY(VisibleAnywhere)
	TMap<FGuid, TWeakObjectPtr<AVePortal>> StaticPortals = {};

	/** Default player start (marked by the Default tag). */
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<class APlayerStart> DefaultPlayerStart = nullptr;

	/** VR pawn class to spawn for the player. */
	UPROPERTY(EditDefaultsOnly, Category="LE7EL")
	TSubclassOf<APawn> VRPawnClass = nullptr;

	/** Default placeable class to use to spawn new actors. */
	UPROPERTY(EditDefaultsOnly, Category="Placeable", meta=(MetaClass="/Script/VeGameFramework.Placeable"))
	TSubclassOf<AActor> PlaceableClass = APlaceableActor::StaticClass();

	UPROPERTY(EditAnywhere)
	float GracefulShutdownDelay = 300.0f;

	/** Each DetectPlayerDelay seconds check If there are no players remain connected to the server to shut it down. */
	UPROPERTY(EditAnywhere)
	float DetectPlayersDelay = 10.0f;

protected:

private:
	FTimerHandle DetectPlayersTimerHandle;
	FTimerHandle GracefulShutdownTimerHandle;

	/** Returns the current space id. */
	FGuid GetCurrentWorldId() const;

#pragma region Placeables

	/**
	 * Requests batch of placeables for the current space. Can be called from derived C++ or BP class at required time and conditions.
	 * @param InOffset where to start
	 * @param InLimit how many to load
	 * @param bRecursive should recursively load all placeables in batches or not, default true
	 */
	void SpawnPlaceables(const int32 InOffset = 0, const int32 InLimit = 100, bool bRecursive = true);

	/** Spawns a placeable. */
	void SpawnPlaceableItem(const FVePlaceableMetadata& InVeMetadata);

#pragma endregion Placeables

#pragma region PlayerNumber

public:
	int32 GetPlayerNumber() const { return ActivePlayerControllers.Num(); }

private:
	TMap<APlayerController*, FGuid> ActivePlayerControllers;

#pragma endregion PlayerNumber

private:	
	void Analytics_ReportUserEnteringWorld(const FGuid& UserId);
	void Analytics_ReportUserLeavingWorld(const FGuid& UserId);

};
