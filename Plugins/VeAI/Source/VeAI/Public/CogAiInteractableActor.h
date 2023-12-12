// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CogAiInteractableActor.generated.h"

UCLASS(Blueprintable, BlueprintType)
class VEAI_API ACogAiInteractableActor : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACogAiInteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Component that makes this actor interactable. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TObjectPtr<class UCogAiInteractableComponent> InteractableComponent;

	/** Transform for the AI to use when interacting with this actor. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TObjectPtr<class UCapsuleComponent> InteractionTransformMarker;

	/** Direction for the AI to use when interacting with this actor. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TObjectPtr<class UArrowComponent> InteractionDirectionMarker;
};
