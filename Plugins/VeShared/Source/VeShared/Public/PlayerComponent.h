// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerComponent.generated.h"

/**
 * Base class for components that attach to pawns or player controllers.
 * @note Simplifies access to owning pawn or player controller and some networking/replication features.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VESHARED_API UPlayerComponent : public UActorComponent {
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerControllerChanged, APlayerController*, NewPlayerController, APlayerController*, OldPlayerController);

public:
	UPlayerComponent();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetPlayerController(APlayerController* NewPlayerController);

	UFUNCTION(BlueprintPure)
	APlayerController* GetPlayerController() const { return MyPlayerController.Get(); }

	UPROPERTY(BlueprintAssignable)
	FOnPlayerControllerChanged OnPlayerControllerChanged;

protected:
	virtual void NativeOnPlayerControllerChanged(APlayerController* NewPlayerController, APlayerController* OldPlayerController);

private:
	UPROPERTY()
	TWeakObjectPtr<APlayerController> MyPlayerController;

public:
	/**
	 * @return Owner pawn if component is attached to the pawn.
	 * @return Controlled pawn if component is attached to the player controller.
	 * @return nullptr if not attached to pawn or player controller.
	 */
	UFUNCTION(BlueprintPure)
	virtual APawn* GetPawn() const;

	/**
	 * @return Owner controller if component is attached to the player controller.
	 * @return Pawn's controller if component is attached to the pawn.
	 * @return nullptr if not attached to pawn or player controller.
	 */
	virtual AController* GetController() const;

	/** @return true if attached to a pawn or player controller. */
	UFUNCTION(BlueprintPure)
	bool HasPlayer() const;

	/** @returns true if attached to a pawn */
	UFUNCTION(BlueprintPure)
	virtual bool IsAttachedToPawn() const;

	/** @returns true if attached to a player controller */
	UFUNCTION(BlueprintPure)
	virtual bool IsAttachedToPlayerController() const;

	/** @returns true if the owning actor is in standalone world */
	UFUNCTION(BlueprintPure)
	bool IsStandalone() const { return GetNetMode() == NM_Standalone; }

	/** @return true if the owning actor is in the server world */
	UFUNCTION(BlueprintPure)
	bool IsServer() const { return GetNetMode() < NM_Client; }

	/** @return true if the owning actor is in the client world */
	UFUNCTION(BlueprintPure)
	bool IsClient() const { return GetNetMode() == NM_Client; }

	/** Returns the owning pawn local role. */
	ENetRole GetLocalRole() const;

	/** Check if the owning pawn has network authority. */
	bool HasAuthority() const;

	/** Check if the owning player pawn is locally controlled. */
	bool IsLocallyControlled() const;

	/** Helper to get the world timer manager. */
	FTimerManager* GetWorldTimerManager() const;

};
