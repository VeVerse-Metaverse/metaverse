// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.
 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CogAiActionDefinition.h"
#include "CogAiActionDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class VEAI_API UCogAiActionDataAsset : public UDataAsset {
	GENERATED_BODY()

public:
	/** @brief List of actions supported by the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<FCogAiActionDefinition> SupportedActions;
};
