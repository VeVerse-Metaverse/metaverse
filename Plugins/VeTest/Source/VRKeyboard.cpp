// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VRKeyboard.h"


// Sets default values
AVRKeyboard::AVRKeyboard() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	KeyboardMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("KeyboardMesh"));
	KeyboardWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(FName("KeyboardWidget"));
}

// Called when the game starts or when spawned
void AVRKeyboard::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void AVRKeyboard::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

}
