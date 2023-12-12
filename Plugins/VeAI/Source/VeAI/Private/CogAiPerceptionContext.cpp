// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "CogAiPerceptionContext.h"

#include "CogAiContextComponent.h"
#include "CogAiPerceptibleComponent.h"

// Sets default values
ACogAiPerceptionContext::ACogAiPerceptionContext() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ContextComponent = CreateDefaultSubobject<UCogAiContextComponent>("PerceptibleComponent");
}

// Called when the game starts or when spawned
void ACogAiPerceptionContext::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ACogAiPerceptionContext::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

TMap<FString, FString> ACogAiPerceptionContext::GetPerceptibleProperties() const {
	if (IsValid(ContextComponent)) {
		return ContextComponent->PerceptibleProperties;
	}
	return {};
}
