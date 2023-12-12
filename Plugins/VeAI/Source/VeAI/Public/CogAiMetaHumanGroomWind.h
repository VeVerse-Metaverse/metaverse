// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CogAiMetaHumanGroomWind.generated.h"

class UBoxComponent;

UCLASS()
class VEAI_API ACogAiMetaHumanGroomWind : public AActor {
	GENERATED_BODY()

	ACogAiMetaHumanGroomWind();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	float CurrentInterpolationTime = 0.0f;
	float TargetInterpolationTime = 0.0;
	float StartWindStrength = 0.0f;
	float TargetWindStrength = 0.0f;

public:
	UFUNCTION(BlueprintCallable, Category = "VeAI")
	FVector GetWindVelocity() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	TObjectPtr<UBoxComponent> BoxComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	FVector WindDirection = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float WindStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float MinWindStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float MaxWindStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float MinInterpolationTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float MaxInterpolationTime = 0.0f;
};
