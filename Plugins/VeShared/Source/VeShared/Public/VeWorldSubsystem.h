// Author: Egor A. Pristavka

#pragma once

class VESHARED_API FVeWorldSubsystem {
public:
	/**
	 * Gets the current world from the engine.
	 * @returns Current world or nullptr.
	 */
	static UWorld* GetWorld();

	/**
	 * Gets the game instance from the engine.
	 * @returns Game instance or nullptr.
	 */
	// static UGameInstance* GetGameInstance();

	/**
	 * Gets the current world net mode.
	 */
	// static ENetMode GetNetMode();

	/**
	 * Gets the local game mode.
	 * @note Valid only for server and standalone worlds.
	 */
	// static AGameModeBase* GetLocalGameMode();

	/**
	 * Gets the local game state.
	 * @returns Game state for the current world or nullptr.
	 */
	// static AGameStateBase* GetGameState();

	/**
	 * Gets the local player state.
	 * @returns Player state for the current player or nullptr.
	 */
	// static APlayerState* GetLocalPlayerState();

	/**
	 * Gets the local player controller from the current world.
	 * @returns Current player controller or nullptr.
	 */
	// static APlayerController* GetLocalPlayerController();

	/**
	 * Gets the locally controlled pawn.
	 * @returns Locally controlled pawn or nullptr if local controller has no pawn.
	 */
	// static APawn* GetLocallyControlledPawn();
};
