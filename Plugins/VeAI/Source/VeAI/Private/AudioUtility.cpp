// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "AudioUtility.h"

// Sets default values
AAudioUtility::AAudioUtility()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAudioUtility::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAudioUtility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

