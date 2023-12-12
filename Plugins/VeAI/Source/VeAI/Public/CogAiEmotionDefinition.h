// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CogAiEmotionDefinition.generated.h"

/**
 * @brief CogAI emotion definition. Used to map emotions to morph targets.
 */
USTRUCT(BlueprintType)
struct FCogAiEmotionDefinition {
	GENERATED_BODY()

	/** @brief Curve table to use to map the emotion value to the morph target value. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	UCurveTable* CurveTable = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TArray<UAnimSequenceBase*> Animations;
};
