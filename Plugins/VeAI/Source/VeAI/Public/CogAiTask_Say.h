// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CogAiTask_Say.generated.h"

class AAIController;

/**
 * @brief Make the AI say something to the target.
 */
UCLASS(BlueprintType, Blueprintable)
class VEAI_API UCogAiTask_Say final : public UBTTaskNode {
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
