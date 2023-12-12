// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VeAiSubject.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UVeAiSimpleFsmSubject : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class VEAI_API IVeAiSimpleFsmSubject {
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VeAiSubject")
	void Say(const FString& InText);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VeAiSubject")
	void Emote(const FString& InEmotion, const FString& InMetadata);
};
