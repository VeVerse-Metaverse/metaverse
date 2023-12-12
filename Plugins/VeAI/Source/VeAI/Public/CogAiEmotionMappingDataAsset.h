// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CogAiEmotionDefinition.h"
#include "CogAiEmotionMappingDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class VEAI_API UCogAiEmotionMappingDataAsset : public UDataAsset {
	GENERATED_BODY()

public:
	/** @brief List of actions supported by the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TMap<FName, FCogAiEmotionDefinition> Emotions;
};
