// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CogAiPersonalityDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class VEAI_API UCogAiPersonalityDataAsset : public UDataAsset {
	GENERATED_BODY()

public:
	/** @brief Personality properties of the character. Key is a property name, value is a property value: -1 for low, 0 for medium, 1 for high. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(UIMin="-1", UIMax="1", ClampMin="-1", ClampMax="1"))
	TMap<FString, int32> PersonalityProperties;
};
