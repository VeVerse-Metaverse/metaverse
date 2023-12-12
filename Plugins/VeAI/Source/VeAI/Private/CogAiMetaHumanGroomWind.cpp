// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "CogAiMetaHumanGroomWind.h"

#include "Components/BoxComponent.h"

ACogAiMetaHumanGroomWind::ACogAiMetaHumanGroomWind() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
}

void ACogAiMetaHumanGroomWind::BeginPlay() {
	Super::BeginPlay();

	TargetInterpolationTime = FMath::RandRange(MinInterpolationTime, MaxInterpolationTime);
	StartWindStrength = FMath::RandRange(MinWindStrength, MaxWindStrength);
	TargetWindStrength = FMath::RandRange(MinWindStrength, MaxWindStrength);
}

void ACogAiMetaHumanGroomWind::Tick(const float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	CurrentInterpolationTime += DeltaSeconds;

	if (CurrentInterpolationTime >= TargetInterpolationTime) {
		TargetInterpolationTime = FMath::RandRange(MinInterpolationTime, MaxInterpolationTime);
		CurrentInterpolationTime = 0.0f;

		StartWindStrength = WindStrength;
		TargetWindStrength = FMath::RandRange(MinWindStrength, MaxWindStrength);
	}

	const float InterpolationAlpha = CurrentInterpolationTime / TargetInterpolationTime;
	WindStrength = FMath::Lerp(StartWindStrength, TargetWindStrength, InterpolationAlpha);
}

FVector ACogAiMetaHumanGroomWind::GetWindVelocity() const {
	return WindDirection * WindStrength;
}
