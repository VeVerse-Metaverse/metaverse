// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "TestVRMenu.generated.h"

class UUIWidget;
class UWidgetComponent;

UCLASS()
class VETEST_API ATestVRMenu final : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestVRMenu();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Set owning pawn the widget will follow. */
	void SetPawn(APawn* InPawn);

	UPROPERTY(EditAnywhere)
	float Distance = 1.0f;
	
	UPROPERTY(EditAnywhere)
	float InterpolationSpeed = 0.250f;
	
	/** Pawn that owns the widget. */
	UPROPERTY(EditAnywhere)
	APawn* OwningPawn;

	/** The main menu widget. */
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* MenuWidget;

};
