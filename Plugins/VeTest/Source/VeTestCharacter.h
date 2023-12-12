// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Components/StereoLayerComponent.h"
#include "GameFramework/Character.h"
#include "Pawns/InteractiveVRCharacter.h"
#include "VeTestCharacter.generated.h"

class UWidgetComponent;
class UVeUserComponent;

UCLASS()
class VETEST_API AVeTestCharacter : public AInteractiveVRCharacter {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVeTestCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Handles setup of replicated properties. */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly)
	float WalkMovementSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly)
	float RunMovementSpeed = 800.f;

	UFUNCTION()
	void Input_MoveForward(float InAxis);

	UFUNCTION()
	void Input_MoveRight(float InAxis);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetRunMode();

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetWalkMode();

	UFUNCTION(Client, Reliable)
	void ClientRPC_UpdateMovementSpeed(float MovementSpeed);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** User information widget displayed over remote pawns. */
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* UserInfoWidgetComponent;

	/** VR menu widget displayed for local pawn. */
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* UserVRMenuWidgetComponent;

	FDelegateHandle OnUserMetadataUpdatedDelegateHandle;

#pragma region VR Components
	UPROPERTY(VisibleAnywhere)
	UStereoLayerComponent* StereoLayer;
#pragma endregion

private:
	FTimerHandle UserInfoWidgetUpdateTimer;
};
