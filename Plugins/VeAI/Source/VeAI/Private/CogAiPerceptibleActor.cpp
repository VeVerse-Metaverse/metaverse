// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "CogAiPerceptibleActor.h"

#include "CogAiPerceptibleComponent.h"
#include "VeAI.h"
#include "VeShared.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

ACogAiPerceptibleActor::ACogAiPerceptibleActor() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.25f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	CogAiPerceptibleComponent = CreateDefaultSubobject<UCogAiPerceptibleComponent>(TEXT("CogAiPerceptibleComponent"));
	AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSourceComponent"));
}

void ACogAiPerceptibleActor::BeginPlay() {
	Super::BeginPlay();
	if (IsValid(AIPerceptionStimuliSourceComponent)) {
		AIPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
		AIPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Hearing::StaticClass());
	} else {
		VeLogErrorFunc("AIPerceptionStimuliSourceComponent is invalid");
	}
}

void ACogAiPerceptibleActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (IsValid(AIPerceptionStimuliSourceComponent)) {
		AIPerceptionStimuliSourceComponent->UnregisterFromSense(UAISense_Sight::StaticClass());
		AIPerceptionStimuliSourceComponent->UnregisterFromSense(UAISense_Hearing::StaticClass());
	} else {
		VeLogErrorFunc("AIPerceptionStimuliSourceComponent is invalid");
	}
	Super::EndPlay(EndPlayReason);
}

void ACogAiPerceptibleActor::SetPerceptibleProperty(const FString& InName, const FString& InValue) {
	if (IsValid(CogAiPerceptibleComponent)) {
		CogAiPerceptibleComponent->PerceptibleProperties.Add(InName, InValue);
		if (bAlertOnPerceptiblePropertyChange) {
			CogAiPerceptibleComponent->PerceptibleProperties.Add(TEXT("alert"), InValue);
			UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.0f, this, 0.0f, FName("Alert"));
		}
	}
}

void ACogAiPerceptibleActor::Alert(const FString& InValue) {
	CogAiPerceptibleComponent->PerceptibleProperties.Add(TEXT("alert"), InValue);
	UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.0f, this, 0.0f, FName("Alert"));
}

FString ACogAiPerceptibleActor::GetPerceptibleProperty(const FString& InName) const {
	if (IsValid(CogAiPerceptibleComponent)) {
		if (const auto* Value = CogAiPerceptibleComponent->PerceptibleProperties.Find(InName)) {
			return *Value;
		}
	}
	return FString();
}
