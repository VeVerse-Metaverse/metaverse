// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.
// 
#include "VeAITask_CompleteSimpleFsmState.h"

#include "AIController.h"
#include "VeAiSimpleFsmBehaviorComponent.h"
#include "VeAiSimpleFsmManager.h"
#include "VeShared.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UVeAITask_CompleteSimpleFsmState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	// Get the AIController
	const auto* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController)) {
		return EBTNodeResult::Failed;
	}

	auto* Pawn = AIController->GetPawn();
	if (!IsValid(Pawn)) {
		return EBTNodeResult::Failed;
	}

	// Todo: Refactor and simplify access to the FSM manager.
	const auto* BehaviorComp = Pawn->FindComponentByClass<UVeAiSimpleFsmBehaviorComponent>();
	if (!IsValid(BehaviorComp)) {
		return EBTNodeResult::Failed;
	}

	auto* FsmManager = BehaviorComp->GetFsmManager();
	if (!IsValid(FsmManager)) {
		return EBTNodeResult::Failed;
	}

	FsmManager->OnStateCompleted();

	return EBTNodeResult::Succeeded;
}
