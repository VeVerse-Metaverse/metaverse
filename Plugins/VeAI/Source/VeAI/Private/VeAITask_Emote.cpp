// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.
// 
#include "VeAITask_Emote.h"

#include "AIController.h"
#include "VeAiBlackboardKeys.h"
#include "VeAiSubject.h"
#include "VeShared.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UVeAITask_Emote::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	// Get the AIController
	const auto* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController)) {
		LogErrorF("AIController is not valid");
		return EBTNodeResult::Succeeded;
	}

	// Get emotion
	auto Emotion = OwnerComp.GetBlackboardComponent()->GetValueAsString(VeAiBlackboardKeys::Emotion);
	if (Emotion.IsEmpty()) {
		Emotion = "...";
	}

	const auto Metadata = OwnerComp.GetBlackboardComponent()->GetValueAsString(VeAiBlackboardKeys::Metadata);

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

	// Check if the pawn implements the interface
	if (!PawnClass->ImplementsInterface(UVeAiSimpleFsmSubject::StaticClass())) {
		LogErrorF("the pawn does not implement the IVeAiSimpleFsmSubject interface");
		return EBTNodeResult::Succeeded;
	}

	// Get the interface and call the function
	IVeAiSimpleFsmSubject::Execute_Emote(Pawn, Emotion, Metadata);

	return EBTNodeResult::Succeeded;
}
