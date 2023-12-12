// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "AVideoPlayer.h"


// Sets default values
AAVideoPlayer::AAVideoPlayer() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAVideoPlayer::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void AAVideoPlayer::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

