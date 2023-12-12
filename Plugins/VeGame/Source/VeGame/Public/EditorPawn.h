// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#pragma once

#include "GameFramework/Character.h"
#include "EditorPawn.generated.h"

class UFloatingPawnMovement;
UCLASS()
class VEGAME_API AEditorPawn : public ACharacter {
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEditorPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Value);

	/** Handles strafing movement, left and right */
	void MoveRight(float Value);

	/** Handles moving up/down */
	void MoveUp(float Value);

	void TurnRight(float Value);
	void LookUp(float Value);
};
