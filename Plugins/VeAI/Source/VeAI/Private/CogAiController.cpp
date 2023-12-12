// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiController.h"

#include "CogAiNpcComponent.h"
#include "CogAiNpcSettingsComponent.h"

ACogAiController::ACogAiController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");
	NpcComponent = CreateDefaultSubobject<UCogAiNpcComponent>("CogAiNpc");
}

void ACogAiController::BeginPlay() {
	Super::BeginPlay();

	if (IsValid(NpcComponent)) {
		NpcComponent->InitializeWithController(this);
	}
}

void ACogAiController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	NpcSettingsComponentRef = InPawn->FindComponentByClass<UCogAiNpcSettingsComponent>();
	if (IsValid(NpcSettingsComponentRef)) {
		if (IsValid(NpcComponent)) {
			NpcComponent->NpcSettingsComponentRef = NpcSettingsComponentRef;

			NpcComponent->InitializeWithPawn(InPawn);
		}
	}
}

void ACogAiController::OnUnPossess() {
	Super::OnUnPossess();

	NpcSettingsComponentRef = nullptr;
}
