// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "OrbitingPawn.h"

#include "CogAiPerceptibleComponent.h"
#include "VeGameModule.h"
#include "VePlayerStateBase.h"
#include "VeShared.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

AOrbitingPawn::AOrbitingPawn() {
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
}

void AOrbitingPawn::BeginPlay() {
	Super::BeginPlay();

	if (IsValid(AIPerceptionStimuliSourceComponent)) {
		AIPerceptionStimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
		AIPerceptionStimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense_Hearing>());
	} else {
		VeLogErrorFunc("AIPerceptionStimuliSourceComponent is invalid");
	}

	const auto* VePlayerState = GetPlayerState<AVePlayerStateBase>();

	if (IsValid(VePlayerState)) {
		if (IsValid(CogAiPerceptibleComponent)) {
			CogAiPerceptibleComponent->SetName(VePlayerState->GetUserMetadata().Name);
			CogAiPerceptibleComponent->SetDescription(VePlayerState->GetUserMetadata().Description);
			CogAiPerceptibleComponent->SetInspectDescription(VePlayerState->GetUserMetadata().Description);
		}
	}

	CurrentRotation = FVector2D::ZeroVector;
	RotationVelocity = FVector2D::ZeroVector;
	CurrentZoom = DistanceToTarget;
	ZoomVelocity = 0.0f;
}

void AOrbitingPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!IsValid(TargetActor)) {
		return;
	}

	const float PitchSoftLimitFactor = CalculateSoftLimitFactor(CurrentRotation.X, PitchLimits.X, PitchLimits.Y, InputAxis.Y);
	const float YawSoftLimitFactor = CalculateSoftLimitFactor(CurrentRotation.Y, YawLimits.X, YawLimits.Y, InputAxis.X);

	// Apply inertia and damping to the rotation.
	RotationVelocity = FMath::Lerp(RotationVelocity, FVector2D(InputAxis.Y, InputAxis.X) * OrbitSpeed, Inertia * FVector2D(PitchSoftLimitFactor, YawSoftLimitFactor) * DeltaTime);
	RotationVelocity *= FMath::Clamp(1.0f - Damping * DeltaTime, 0.0f, 1.0f);
	RotationVelocity.X *= PitchSoftLimitFactor;
	RotationVelocity.Y *= YawSoftLimitFactor;
	CurrentRotation += RotationVelocity * DeltaTime;

	// Clamp pitch and yaw.
	CurrentRotation.X = FMath::Clamp(CurrentRotation.X, PitchLimits.X, PitchLimits.Y);
	CurrentRotation.Y = FMath::Clamp(CurrentRotation.Y, YawLimits.X, YawLimits.Y);

	const float ZoomSoftLimitFactor = CalculateSoftLimitFactor(CurrentZoom, ZoomLimits.X, ZoomLimits.Y, InputAxis.Z);

	// Apply inertia and damping to the zoom.
	ZoomVelocity = FMath::Lerp(ZoomVelocity, InputAxis.Z * OrbitSpeed, Inertia * ZoomSoftLimitFactor * DeltaTime);
	ZoomVelocity *= FMath::Clamp(1.0f - Damping * DeltaTime, 0.0f, 1.0f);
	ZoomVelocity *= ZoomSoftLimitFactor;
	CurrentZoom += ZoomVelocity * DeltaTime;

	// Clamp zoom.
	CurrentZoom = FMath::Clamp(CurrentZoom, ZoomLimits.X, ZoomLimits.Y);

	// Calculate new position and rotation.
	const FVector TargetLocation = TargetActor->GetActorLocation();
	const FVector CameraOffset = FVector::UpVector * FMath::Sin(FMath::DegreesToRadians(CurrentRotation.X)) + FVector::RightVector * FMath::Cos(FMath::DegreesToRadians(CurrentRotation.X)) * FMath::Sin(FMath::DegreesToRadians(CurrentRotation.Y)) + FVector::ForwardVector * FMath::Cos(FMath::DegreesToRadians(CurrentRotation.X)) * FMath::Cos(FMath::DegreesToRadians(CurrentRotation.Y));
	const FVector NewLocation = TargetLocation - CameraOffset * CurrentZoom;
	const FRotator NewRotation = FRotationMatrix::MakeFromX(TargetLocation - NewLocation).Rotator();

	// Apply new position and rotation.
	SetActorLocationAndRotation(NewLocation, NewRotation);
}

void AOrbitingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("InteractAxisTurnRight", this, &AOrbitingPawn::OrbitHorizontal);
	PlayerInputComponent->BindAxis("InteractAxisLookUp", this, &AOrbitingPawn::OrbitVertical);
	PlayerInputComponent->BindAxis("InspectWheelAxis", this, &AOrbitingPawn::Zoom);
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AOrbitingPawn::OnPinchBegan);
}

void AOrbitingPawn::OrbitHorizontal(const float AxisValue) {
	InputAxis.X = AxisValue;
}

void AOrbitingPawn::OrbitVertical(const float AxisValue) {
	InputAxis.Y = AxisValue;
}

void AOrbitingPawn::Zoom(const float AxisValue) {
	InputAxis.Z = AxisValue * ZoomSpeed;
}

void AOrbitingPawn::OnPinchBegan(const ETouchIndex::Type Type, const FVector Location) {
	if (Type == ETouchIndex::Touch1) {
		FVector Touch2Location = FVector::ZeroVector;
		bool bIsCurrentlyPressed = false;

		if (const auto* PC = Cast<APlayerController>(GetController())) {
			PC->GetInputTouchState(ETouchIndex::Touch2, Touch2Location.X, Touch2Location.Y, bIsCurrentlyPressed);
		}

		if (bIsCurrentlyPressed) {
			InitialPinchDistance = FVector::Dist(Location, Touch2Location);
		}
	}
}

void AOrbitingPawn::OnPinchUpdate(const ETouchIndex::Type Type, const FVector Location) {
	if (Type == ETouchIndex::Touch1) {
		FVector Touch2Location = FVector::ZeroVector;
		bool bIsCurrentlyPressed = false;

		if (const auto* PC = Cast<APlayerController>(GetController())) {
			PC->GetInputTouchState(ETouchIndex::Touch2, Touch2Location.X, Touch2Location.Y, bIsCurrentlyPressed);
		}

		if (bIsCurrentlyPressed) {
			const float CurrentPinchDistance = FVector::Dist(Location, Touch2Location);
			const float DeltaZoom = InitialPinchDistance - CurrentPinchDistance;

			// Apply inertia and damping to zoom.
			CurrentZoom = FMath::Lerp(CurrentZoom, CurrentZoom + DeltaZoom, Inertia);
			CurrentZoom = FMath::Clamp(CurrentZoom, ZoomLimits.X, ZoomLimits.Y);

			InitialPinchDistance = CurrentPinchDistance;
		}
	}
}

float AOrbitingPawn::CalculateSoftLimitFactor(const float CurrentValue, const float MinLimit, const float MaxLimit, const float Direction) const {
	const float Range = MaxLimit - MinLimit;
	const float SoftLimitRange = Range * SoftLimitFactor;
	const float LowerSoftLimit = MinLimit + SoftLimitRange;
	const float UpperSoftLimit = MaxLimit - SoftLimitRange;

	if (CurrentValue < LowerSoftLimit && Direction < 0) {
		const float DampingFactor = FMath::Clamp((CurrentValue - MinLimit) / SoftLimitRange, 0.0f, 1.0f);
		return DampingFactor;
	}

	if (CurrentValue > UpperSoftLimit && Direction > 0) {
		const float DampingFactor = FMath::Clamp((MaxLimit - CurrentValue) / SoftLimitRange, 0.0f, 1.0f);
		return DampingFactor;
	}

	return 1.0f;
}

void AOrbitingPawn::OnReady_PlayerState(FOrbitiongPawnPlayerStateDelegate Callback) {
	OnPlayerStateChanged.AddUFunction(Callback.GetUObject(), Callback.GetFunctionName());
	Callback.Execute(GetPlayerState());
}

void AOrbitingPawn::NativeOnPlayerStateChanged(APlayerState* InPlayerState) {
	OnPlayerStateChanged.Broadcast(InPlayerState);
}

void AOrbitingPawn::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	NativeOnPlayerStateChanged(GetPlayerState());
}

void AOrbitingPawn::UnPossessed() {
	Super::UnPossessed();
	NativeOnPlayerStateChanged(GetPlayerState());
}

void AOrbitingPawn::OnRep_PlayerState() {
	Super::OnRep_PlayerState();
	NativeOnPlayerStateChanged(GetPlayerState());
}