// 

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "VeAITask_CompleteSimpleFsmState.generated.h"

class AAIController;

/**
 * @brief Make the AI say something to the target.
 */
UCLASS(BlueprintType, Blueprintable)
class VEAI_API UVeAITask_CompleteSimpleFsmState final : public UBTTaskNode {
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
