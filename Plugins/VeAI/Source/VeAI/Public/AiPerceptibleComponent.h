// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AiPerceptionData.h"
#include "Components/ActorComponent.h"
#include "AiPerceptibleComponent.generated.h"

/**
 * @brief The UAiPerceptibleComponent is a custom UActorComponent that enables AI-powered characters to perceive stimuli in their environment. By attaching this component to an actor, the AI character can dynamically receive sensory input (such as visual, auditory, or other types) from the actor, which can then be processed by the AI neural network to determine the appropriate response or behavior.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UAiPerceptibleComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UAiPerceptibleComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief The Perception property holds an instance of the FAiPerception struct, which provides information about the type of perception stimulus, the name of the stimulus, and additional details about the stimulus. This data is used as input to the AI neural network to recognize the perceived stimuli and determine the AI character's response or behavior accordingly.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FAiPerceptionData PerceptionData;
};
