// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "CogAiPerceptionContext.generated.h"

UCLASS()
class VEAI_API ACogAiPerceptionContext : public AActor {
	GENERATED_BODY()

public:
	ACogAiPerceptionContext();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	TMap<FString, FString> GetPerceptibleProperties() const;

	/**
	 * @brief Perceptible component of the context. Must have properties set: description, location, time, weather, vibe.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UCogAiContextComponent* ContextComponent = nullptr;
};
