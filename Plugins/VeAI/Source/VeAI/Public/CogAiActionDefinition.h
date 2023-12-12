// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "CogAiActionParameterDefinition.h"
#include "CogAiActionDefinition.generated.h"

/**
 * @brief CogAI action definition, sent to the CogAI backend to register the action.
 */
USTRUCT(BlueprintType)
struct FCogAiActionDefinition {
	GENERATED_BODY()

	// inline bool operator==(const FCogAiActionDefinition& Other) const {
		// return Type == Other.Type && Parameters == Other.Parameters;
	// }

	bool operator==(const FCogAiActionDefinition& Other) const {
		return Type == Other.Type && Parameters == Other.Parameters;
	}

	/**
	 * @brief Type of the action.
	 * @note Max length 16 characters.
	 * @remark Example: "move", "attack", "talk"
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI", meta=(MaxLength="16"))
	FString Type;

	/**
	 * @brief List of parameters for the action and their descriptions.
	 * @note Max length 50 characters.
	 * @remark Example: {"target": "the target of the action"}
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI", meta=(MaxLen="50"))
	TArray<FCogAiActionParameterDefinition> Parameters;
};
