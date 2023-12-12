// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OrbitingPawn.generated.h"

class UCogAiPerceptibleComponent;
class UAIPerceptionStimuliSourceComponent;

UCLASS()
class VEGAME_API AOrbitingPawn : public APawn {
	GENERATED_BODY()

public:
	AOrbitingPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	float OrbitSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	float ZoomSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	float Inertia = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	float Damping = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	float DistanceToTarget = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	FVector2D PitchLimits = FVector2D(-30.0f, 30.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	FVector2D YawLimits = FVector2D(-30.0f, 30.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	FVector2D ZoomLimits = FVector2D(100.0f, 300.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	float SoftLimitFactor = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orbit Camera")
	TObjectPtr<class UCameraComponent> Camera = nullptr;

	UFUNCTION()
	void OrbitHorizontal(float AxisValue);

	UFUNCTION()
	void OrbitVertical(float AxisValue);

	UFUNCTION()
	void Zoom(float AxisValue);

	UFUNCTION()
	void OnPinchBegan(ETouchIndex::Type Type, FVector Location);

	UFUNCTION()
	void OnPinchUpdate(ETouchIndex::Type Type, FVector Location);

	/** @brief Calculates the soft limit factor for the given value. */
	float CalculateSoftLimitFactor(const float CurrentValue, const float MinLimit, const float MaxLimit, const float Direction) const;

private:
	FVector InputAxis = FVector::ZeroVector;
	FVector2D CurrentRotation = FVector2D::ZeroVector;
	FVector2D RotationVelocity = FVector2D::ZeroVector;
	float CurrentZoom = 0;
	float ZoomVelocity = 0;
	float InitialPinchDistance = 0;

#pragma region PlayerState
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOrbitiongPawnPlayerStateDelegate, APlayerState*, InPlayerState);

	DECLARE_EVENT_OneParam(AOrbitingPawn, FOnOrbitiongPawnPlayerStateChanged, APlayerState*);

public:
	UFUNCTION(BlueprintCallable, Category="Metaverse")
	void OnReady_PlayerState(FOrbitiongPawnPlayerStateDelegate Callback);

	FOnOrbitiongPawnPlayerStateChanged OnPlayerStateChanged;

protected:
	virtual void NativeOnPlayerStateChanged(APlayerState* InPlayerState);
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;

#pragma endregion PlayerState

#pragma region AI

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSourceComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UCogAiPerceptibleComponent> CogAiPerceptibleComponent = nullptr;

#pragma endregion
};
