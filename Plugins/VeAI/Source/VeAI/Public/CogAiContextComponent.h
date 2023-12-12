// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CogAiPerceptibleComponent.h"
#include "CogAiContextComponent.generated.h"

class UCogAiActionDataAsset;

/**
 * @brief Used to define a context for the AI, can be added to any actor.
 * @note There can be several contexts in the game world, AI will use the closest one. 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UCogAiContextComponent : public UCogAiPerceptibleComponent {
	GENERATED_BODY()

public:
	UCogAiContextComponent();

	/** @brief Data asset with the list of supported actions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UCogAiActionDataAsset> ActionDataAssetRef = nullptr;
};
