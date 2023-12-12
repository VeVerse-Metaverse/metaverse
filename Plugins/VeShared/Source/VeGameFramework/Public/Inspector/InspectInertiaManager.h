// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InspectInertiaManager.generated.h"

USTRUCT(BlueprintType)
struct FInspectInertiaSample {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Delta;

	FInspectInertiaSample()
		: Time(0.0), Delta(0.0f) {}

	explicit FInspectInertiaSample(const float InDelta, const double InTime)
		: Time(InTime), Delta(InDelta) { }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEGAMEFRAMEWORK_API UInspectInertiaManager final : public UActorComponent {
	GENERATED_BODY()

public:
	UInspectInertiaManager();

	/** Percentage of velocity loss per second. */
	UPROPERTY(EditAnywhere)
	float FrictionCoefficient = 2.0f;

	/** Constant amount of velocity lost per second. */
	UPROPERTY(EditAnywhere)
	float StaticVelocityDrag = 100.0f;

	/** Samples older than this will be discarded. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.05f, ClampMax=3.0f))
	float SampleTimeout = 0.1f;

	/** Current velocity. */
	UPROPERTY(VisibleAnywhere)
	float Velocity = 0.0f;

	UPROPERTY()
	TArray<FInspectInertiaSample> Samples;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void AddSample(float Delta, double CurrentTime);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void UpdateVelocity(const float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void ClearVelocity();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, BlueprintPure)
	float GetVelocity() const { return Velocity; }
};
