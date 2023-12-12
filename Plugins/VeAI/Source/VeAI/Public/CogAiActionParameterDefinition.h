// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CogAiActionParameterDefinition.generated.h"

/**
 * @brief CogAI action parameter definition, sent to the CogAI backend.
 */
USTRUCT(BlueprintType)
struct FCogAiActionParameterDefinition {
	GENERATED_BODY()

	bool operator==(const FCogAiActionParameterDefinition& Other) const {
		return Name == Other.Name && Description == Other.Description;
	}

	/**
	 * @brief Name of the parameter.
	 * @note Max length 16 characters.
	 * @remark Example: "target", "message", "emotion", "thoughts".
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI", meta=(MaxLen="16"))
	FString Name;

	/**
	 * @brief Description of the parameter.
	 * @note Max length 32 characters.
	 * @note Example: "whom to say to", "what to say", "what do you feel", "your thoughts".
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI", meta=(MaxLen="32"))
	FString Description;
};
