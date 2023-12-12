// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "UObject/Interface.h"
#include "CogPerceptible.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class UCogPerceptible : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class VEAI_API ICogPerceptible {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	FString GetName();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	FString GetDescription();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	FString GetInspectDescription();
};
