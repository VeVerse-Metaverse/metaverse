// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeAiSimpleFsmAIController.h"

#include "VeAiSimpleFsmBehaviorComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"

// ReSharper disable once CppMemberFunctionMayBeConst
void AVeAiSimpleFsmAIController::OnTargetPerceptionUpdated(AActor* Actor, const FAIStimulus Stimulus) {
	if (IsValid(BehaviorComponentRef)) {
		BehaviorComponentRef->OnTargetPerceptionUpdated(Actor, Stimulus);
	}
}

AVeAiSimpleFsmAIController::AVeAiSimpleFsmAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	SetPathFollowingComponent(CreateDefaultSubobject<UCrowdFollowingComponent>(TEXT("PathFollowing")));

	// Add perception component
	PerceptionComponentRef = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	if (IsValid(PerceptionComponentRef)) {
		PerceptionComponentRef->OnTargetPerceptionUpdated.AddDynamic(this, &AVeAiSimpleFsmAIController::OnTargetPerceptionUpdated);
	}
}

void AVeAiSimpleFsmAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	BehaviorComponentRef = InPawn->FindComponentByClass<UVeAiSimpleFsmBehaviorComponent>();
	if (IsValid(BehaviorComponentRef)) {
		BehaviorComponentRef->InitializeWithController(this);
	}
}

void AVeAiSimpleFsmAIController::OnUnPossess() {
	Super::OnUnPossess();

	BehaviorComponentRef = nullptr;
}
