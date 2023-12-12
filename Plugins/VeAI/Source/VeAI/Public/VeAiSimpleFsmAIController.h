// 

#pragma once

#include "CoreMinimal.h"
#include "VeAiController.h"
#include "VeAiSimpleFsmAIController.generated.h"

class UVeAiSimpleFsmBehaviorComponent;

/**
 * @brief AI controller that handles the AI logic.
 */
UCLASS(BlueprintType)
class VEAI_API AVeAiSimpleFsmAIController final : public AVeAiController {
	GENERATED_BODY()

	/**
	 * @brief Called when the target actor is perceived to perform sense logic.
	 */
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

public:
	explicit AVeAiSimpleFsmAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * @brief Called when the AI controller possesses a pawn.
	 */
	virtual void OnPossess(APawn* InPawn) override;

	/**
	 * @brief Called when the AI controller stops controlling a pawn.
	 */
	virtual void OnUnPossess() override;

	/**
	 * @brief Component that handles the AI perception.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VeAI")
	UAIPerceptionComponent* PerceptionComponentRef = nullptr;

	/**
	 * @brief Behavior component that handles the AI logic.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VeAI")
	UVeAiSimpleFsmBehaviorComponent* BehaviorComponentRef = nullptr;
};
