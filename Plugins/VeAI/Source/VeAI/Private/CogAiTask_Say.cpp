// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiTask_Say.h"

#include "AIController.h"
#include "CogAiMessage.h"
#include "VeShared.h"
#include "BehaviorTree/BlackboardComponent.h"

static const FName NAME_TargetActor = TEXT("TargetActor");
static const FName NAME_ActionMessage = TEXT("ActionMessage");

EBTNodeResult::Type UCogAiTask_Say::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	// Get the AIController
	const auto* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController)) {
		LogErrorF("AIController is not valid");
		return EBTNodeResult::Succeeded;
	}

	// Get the target
	if (const auto* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(NAME_TargetActor)); !IsValid(Target)) {
		LogErrorF("failed to get a target from the blackboard");
		return EBTNodeResult::Succeeded;
	}

	// Get message
	if (const auto Message = Cast<UCogAiActionMessage>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(NAME_ActionMessage)); !IsValid(Message)) {
		LogErrorF("failed to get a action data from the blackboard");
		return EBTNodeResult::Succeeded;
	} else {
		// Get the pawn
		auto* Pawn = AIController->GetPawn();
		if (!IsValid(Pawn)) {
			LogErrorF("failed to get the pawn from the AIController");
			return EBTNodeResult::Succeeded;
		}

		// Get the pawn class
		const auto* PawnClass = Pawn->GetClass();
		if (!IsValid(PawnClass)) {
			LogErrorF("failed to get the pawn class");
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Succeeded;
}
