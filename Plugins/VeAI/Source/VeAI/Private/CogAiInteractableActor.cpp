// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiInteractableActor.h"

#include "CogAiInteractableComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
ACogAiInteractableActor::ACogAiInteractableActor() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractableComponent = CreateDefaultSubobject<UCogAiInteractableComponent>(TEXT("InteractableComponent"));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	InteractionTransformMarker = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionTransformMarker"));
	InteractionTransformMarker->SetupAttachment(RootComponent);

	InteractionDirectionMarker = CreateDefaultSubobject<UArrowComponent>(TEXT("InteractionDirectionMarker"));
	InteractionDirectionMarker->SetupAttachment(InteractionTransformMarker);
}

// Called when the game starts or when spawned
void ACogAiInteractableActor::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void ACogAiInteractableActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
