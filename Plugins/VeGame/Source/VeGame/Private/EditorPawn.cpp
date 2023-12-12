// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "EditorPawn.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AEditorPawn::AEditorPawn() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = true;
	bUseControllerRotationYaw = true;

	auto* CharacterMovementComp = GetCharacterMovement();
	CharacterMovementComp->DefaultLandMovementMode = EMovementMode::MOVE_Flying;
	CharacterMovementComp->BrakingFrictionFactor = 60.0f;	
}

// Called when the game starts or when spawned
void AEditorPawn::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AEditorPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEditorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("InteractAxisMoveForward", this, &AEditorPawn::MoveForward);
	PlayerInputComponent->BindAxis("InteractAxisMoveRight", this, &AEditorPawn::MoveRight);
	PlayerInputComponent->BindAxis("Move Up", this, &AEditorPawn::MoveUp);

	PlayerInputComponent->BindAxis("InteractAxisTurnRight", this, &AEditorPawn::TurnRight);
	PlayerInputComponent->BindAxis("InteractAxisLookUp", this, &AEditorPawn::LookUp);
}

void AEditorPawn::MoveForward(float Value) {
	if (Value != 0.0f) {
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AEditorPawn::MoveRight(float Value) {
	if (Value != 0.0f) {
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AEditorPawn::MoveUp(float Value) {
	if (Value != 0.0f) {
		AddMovementInput(GetActorUpVector(), Value);
	}
}

void AEditorPawn::TurnRight(float Value) {
	if (Value != 0.0f) {
		AddControllerYawInput(Value);
	}
}

void AEditorPawn::LookUp(float Value) {
	if (Value != 0.0f) {
		AddControllerPitchInput(Value);
	}
}
