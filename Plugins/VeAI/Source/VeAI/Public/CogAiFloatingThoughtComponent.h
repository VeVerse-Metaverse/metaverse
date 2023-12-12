// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CogAiFloatingThought.h"
#include "Components/ActorComponent.h"
#include "CogAiFloatingThoughtComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UCogAiFloatingThoughtComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UCogAiFloatingThoughtComponent();

	/** @brief Enables or disables the floating thoughts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bUseFloatingThoughts = false;

	/** @brief The radius in which the floating thoughts will be spawned. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float FloatingThoughtsSpawnRadius = 100.0f;

	/** @brief The class used for floating thoughts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TSubclassOf<ACogAiFloatingThought> FloatingThoughtClass = ACogAiFloatingThought::StaticClass();

	/** @brief Spawns the floating thoughts. */
	UFUNCTION(BlueprintNativeEvent, Category="AI")
	void SpawnFloatingThoughts(const FString& Thoughts, const FString& Emotion = TEXT(""));

};
