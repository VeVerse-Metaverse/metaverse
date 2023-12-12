// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CogAiPersonalityDefinitionDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class VEAI_API UCogAiPersonalityDefinitionDataAsset : public UDataAsset {
	GENERATED_BODY()

public:
	/** @brief Supported personality properties defining the character. Key is the property name, value is the property description. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TMap<FString, FString> SupportedPersonalityProperties;
};
