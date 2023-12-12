// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AiPerceptionData.h"
#include "Api2AiActionRequest.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "AiNpcComponent.generated.h"

class UAiPerceptibleComponent;
struct FAiPerceptionEvent;
enum class EAiPerception : uint8;

USTRUCT(BlueprintType)
struct VEAI_API FPerceptionEventArray {
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAiPerceptionEvent> Events;
};

USTRUCT(BlueprintType)
struct VEAI_API FPerceiveEvent {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TMap<EAiPerception, FPerceptionEventArray> Perception;
};

USTRUCT(BlueprintType)
struct VEAI_API FPerceiveEventsWrapper {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TArray<FPerceiveEvent> Events;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UAiNpcComponent final : public UActorComponent {
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnPerceptionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnPerceptionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	float CalculateRange(const UAiPerceptibleComponent* PerceptibleComponent) const;

	FVector CalculateDirection(const UAiPerceptibleComponent* PerceptibleComponent) const;

	float CalculateStrength(UAiPerceptibleComponent* PerceptibleComponent);

	float CalculateDuration(const UAiPerceptibleComponent* PerceptibleComponent);
	bool IsObjectWithinFieldOfView(const FVector& CharacterFacingDirection, const FVector& ObjectDirection) const;
	bool IsPerceptionEventRelevant(const FAiPerceptionEvent& PerceptionEvent) const;
	FString ConvertTopPerceptionEventsToJson(const TArray<FAiPerceptionEvent>& TopPerceptionEvents) const;

	UFUNCTION()
	void ProcessPerceivedEntities();

	// Sets default values for this component's properties
	UAiNpcComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief The PerceptionSphere property holds a reference to a USphereComponent that is used to detect nearby actors that have the UAiPerceptibleComponent attached. The PerceptionSphere is used to detect nearby actors that are within the AI character's sensory range, and the detected actors are then processed by the AI neural network to determine the appropriate response or behavior.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TObjectPtr<USphereComponent> PerceptionSphere = nullptr;

	/**
	 * @brief The PerceivedEntities property holds an array of UAiPerceptibleComponent instances that are detected by the PerceptionSphere. The PerceivedEntities array is used to store the detected actors, which are then processed by the AI neural network to determine the appropriate response or behavior.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TArray<TObjectPtr<UAiPerceptibleComponent>> PerceptibleComponents;

	/**
	 * @brief The PerceptibleActorTimestamps property holds a map of UAiPerceptibleComponent instances and their corresponding timestamps. The PerceptibleActorTimestamps map is used to store the detected actors and the time that the AI character detected them. This data is used to determine how long the AI character had been detecting the actor, which is used to determine the appropriate response or behavior.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TMap<UAiPerceptibleComponent*, float> PerceptibleActorTimestamps;

	/**
	 * @brief The ProcessPerceivedEntitiesInterval property holds a float value that determines how often the AI character should process the detected actors.
	 */
	UPROPERTY
	(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float ProcessPerceivedEntitiesInterval = 1.0f;

	/**
	 * @brief The MinVisualDistance property holds a float value that determines the minimum distance that the AI character can see actors.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float MinVisualPerceptionDistance = 10.0f;

	/**
	 * @brief The MaxVisualDistance property holds a float value that determines the maximum distance that the AI character can see actors.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float MaxVisualPerceptionDistance = 10000.0f;

	/**
	 * @brief The MinAudioDistance property holds a float value that determines the minimum distance that the AI character can hear actors.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float MinAudioPerceptionDistance = 10.0f;

	/**
	 * @brief The MaxAudioDistance property holds a float value that determines the maximum distance that the AI character can hear actors.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float MaxAudioPerceptionDistance = 10000.0f;

	/**
	 * @brief The MinPerceptibleDistance property holds a float value that determines the minimum distance that the AI character can detect actors.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float MinOtherPerceptionDistance = 1.0f;

	/**
	 * @brief The MaxPerceptibleDistance property holds a float value that determines the maximum distance that the AI character can detect actors.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float MaxOtherPerceptionDistance = 100.0f;

	/**
	 * @brief How much the duration of the perception affects the strength of the perception. E.g. if the duration is 10 seconds and the duration factor is 10, the strength will be 1.0f.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float PerceptionDurationFactor = 10.0f;

	/**
	 * @brief The MinStrength property holds a float value that determines the minimum strength that the AI character can detect actors.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float MinPerceptionStrength = 0.1f;

	/**
	 * @brief The MinDuration property holds a float value that determines the minimum duration that the AI character can detect actors.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float MinPerceptionDuration = 0.1f;

	/**
	 * @brief The FieldOfView property holds a float value that determines the field of view that the AI character can detect actors.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float FieldOfView = 120.0f;

	/**
	 * @brief The PerceptionEvents property holds an array of FAiPerceptionEvent instances that are detected by the PerceptionSphere. The PerceptionEvents array is used to store the detected actors, which are then processed by the AI neural network to determine the appropriate response or behavior.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TArray<FAiPerceptionEvent> PerceptionEvents;

	/**
	 * @brief History of messages sent to the AI. Keep max of 10 messages.
	 */
	TArray<FApi2AiMessage> Messages;

	/**
	 * @brief The TimerHandle property holds a reference to a FTimerHandle instance that is used to manage the AI character's behavior. It is used to determine when the AI character should perform an action, such as moving to a new location or interacting with another actor by processing the detected actors.
	 */
	FTimerHandle TimerHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FAiNpcData NpcData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TArray<FAiActionDefinition> ActionDefinitions;

	UFUNCTION(BlueprintCallable, Category = "Position")
	FString GetPositionDescriptor(const FVector& Direction);

private:
	FString GetHorizontalDescriptor(float XDifference);
	FString GetVerticalDescriptor(float YDifference);
	FString GetAltitudeDescriptor(float ZDifference);
};
