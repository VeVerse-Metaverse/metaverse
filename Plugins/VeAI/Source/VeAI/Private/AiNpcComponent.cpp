// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "AiNpcComponent.h"

#include "AiPerceptibleComponent.h"
#include "Api2AiSubsystem.h"
#include "ComponentUtils.h"
#include "DrawDebugHelpers.h"
#include "JsonObjectConverter.h"
#include "AiPerceptionData.h"
#include "VeApi2.h"
#include "VeShared.h"

void UAiNpcComponent::OnPerceptionSphereBeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&) {
	if (OtherActor == GetOwner()) {
		return;
	}

	if (!IsValid(OtherActor)) {
		return;
	}

#if WITH_EDITORONLY_DATA
	// Draw a debug sphere
	if (const auto* const World = GetWorld(); IsValid(World)) {
		DrawDebugSphere(World, OtherActor->GetActorLocation(), 100.0f, 16, FColor::Green, false, 5.0f);
	}
#endif

	if (UAiPerceptibleComponent* PerceptibleComponent = Cast<UAiPerceptibleComponent>(OtherActor->GetComponentByClass(UAiPerceptibleComponent::StaticClass()))) {
		PerceptibleComponents.AddUnique(PerceptibleComponent);

		if (const auto* const World = GetWorld(); IsValid(World)) {
			// Set the timestamp to the current time.
			if (PerceptibleActorTimestamps.Contains(PerceptibleComponent)) {
				PerceptibleActorTimestamps.Add(PerceptibleComponent, World->GetTimeSeconds());
			}
		}
	}
}

void UAiNpcComponent::OnPerceptionSphereEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32) {
	if (OtherActor == GetOwner()) {
		return;
	}

	if (!IsValid(OtherActor)) {
		return;
	}

	if (UAiPerceptibleComponent* PerceptibleComponent = Cast<UAiPerceptibleComponent>(OtherActor->GetComponentByClass(UAiPerceptibleComponent::StaticClass()))) {
		PerceptibleComponents.Remove(PerceptibleComponent);

		if (const auto* const World = GetWorld(); IsValid(World)) {
			// Remove the perceptible component from the timestamp map if it exists as we no longer detect the actor.
			if (PerceptibleActorTimestamps.Contains(PerceptibleComponent)) {
				PerceptibleActorTimestamps.Remove(PerceptibleComponent);
			}
		}
	}
}

float UAiNpcComponent::CalculateRange(const UAiPerceptibleComponent* PerceptibleComponent) const {
	// Get the AI character.
	const auto* Owner = GetOwner();
	if (!IsValid(Owner)) {
		return 1.0; // Very far away
	}

	// Get the perceptible component owner.
	const auto* OtherOwner = PerceptibleComponent->GetOwner();
	if (!IsValid(OtherOwner)) {
		return 1.0; // Very far away
	}

	// Calculate the range as the distance between the AI character and the perceptible actor.
	const auto AiLocation = Owner->GetActorLocation();
	const auto PerceptibleActorLocation = OtherOwner->GetActorLocation();
	const float Range = FVector::Distance(AiLocation, PerceptibleActorLocation);

	// Normalize the range to a value between 0 and 1.
	const float NormalizedRange = Range / PerceptionSphere->GetScaledSphereRadius();

	return NormalizedRange;
}

FVector UAiNpcComponent::CalculateDirection(const UAiPerceptibleComponent* PerceptibleComponent) const {
	// Get the AI character.
	const auto* Owner = GetOwner();
	if (!IsValid(Owner)) {
		return FVector::ZeroVector; // Failed to calculate direction
	}

	// Get the perceptible component owner.
	const auto* OtherOwner = PerceptibleComponent->GetOwner();
	if (!IsValid(OtherOwner)) {
		return FVector::ZeroVector; // Failed to calculate direction
	}

	// Calculate the direction as a normalized vector pointing from the AI character to the perceptible actor
	const auto AiLocation = Owner->GetActorLocation();
	const auto PerceptibleActorLocation = OtherOwner->GetActorLocation();
	auto Direction = PerceptibleActorLocation - AiLocation;
	Direction.Normalize();
	return Direction;
}

float UAiNpcComponent::CalculateStrength(UAiPerceptibleComponent* PerceptibleComponent) {
	// Get the locations of the AI character and the perceptible actor
	auto AiLocation = GetOwner()->GetActorLocation();
	auto PerceptibleActorLocation = PerceptibleComponent->GetOwner()->GetActorLocation();

	// Calculate the distance between the AI character and the perceptible actor
	float Distance = FVector::Distance(AiLocation, PerceptibleActorLocation);

	float Strength;

	// Calculate the strength based on the type of perception and the distance
	switch (PerceptibleComponent->PerceptionData.Type) {
	case EAiPerception::Visual:
		Strength = FMath::Clamp((MaxVisualPerceptionDistance - Distance) / (MaxVisualPerceptionDistance - MinVisualPerceptionDistance), 0.0f, 1.0f);
		break;
	case EAiPerception::Audio:
		Strength = FMath::Clamp((MaxAudioPerceptionDistance - Distance) / (MaxAudioPerceptionDistance - MinAudioPerceptionDistance), 0.0f, 1.0f);
		break;
	default:
		// For other types of perception return a default strength value
		Strength = 1.0f;
		break;
	}

	return Strength;
}

float UAiNpcComponent::CalculateDuration(const UAiPerceptibleComponent* PerceptibleComponent) {
	// Check if the perceptible component is in the perceptible actor timestamps map
	if (!PerceptibleActorTimestamps.Contains(PerceptibleComponent)) {
		return UE_SMALL_NUMBER;
	}

	// Get the elapsed time since the actor first appeared in perception
	const float FirstAppearanceTimestamp = PerceptibleActorTimestamps[PerceptibleComponent];
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float ElapsedTime = CurrentTime - FirstAppearanceTimestamp;

	// Define a time factor that affects the duration calculation
	const float Duration = FMath::Clamp(ElapsedTime / PerceptionDurationFactor, 0.0f, 1.0f);

	return Duration;
}

bool UAiNpcComponent::IsObjectWithinFieldOfView(const FVector& CharacterFacingDirection, const FVector& ObjectDirection) const {
	// Convert FOV angle (in degrees) to radians
	const float FovRadians = FMath::DegreesToRadians(FieldOfView);

	// Compute the cosine of half of the FOV angle
	const float DotProductThreshold = FMath::Cos(FovRadians * 0.5f);

	// Calculate the dot product of the character's facing direction and the object direction
	const float DotProduct = FVector::DotProduct(CharacterFacingDirection.GetSafeNormal(), ObjectDirection.GetSafeNormal());

	// Compare the dot product with the threshold
	return DotProduct >= DotProductThreshold;
}

bool UAiNpcComponent::IsPerceptionEventRelevant(const FAiPerceptionEvent& PerceptionEvent) const {
	float MaxRange;
	switch (PerceptionEvent.Perception.Type) {
	case EAiPerception::Visual:
		MaxRange = MaxVisualPerceptionDistance;
		break;
	case EAiPerception::Audio:
		MaxRange = MaxAudioPerceptionDistance;
		break;
	default:
		MaxRange = MaxOtherPerceptionDistance;
		break;
	}

	const FVector CharacterFacingDirection = GetOwner()->GetActorForwardVector();

	// Check strength
	if (PerceptionEvent.StrengthValue < MinPerceptionStrength) {
		return false;
	}

	// Check range
	if (PerceptionEvent.RangeValue > MaxRange) {
		return false;
	}

	// Check if the object is within the character's FOV
	if (!IsObjectWithinFieldOfView(CharacterFacingDirection, PerceptionEvent.DirectionValue)) {
		return false;
	}

	// Check duration
	if (PerceptionEvent.DurationValue < MinPerceptionDuration) {
		return false;
	}

	return true;
}

void UAiNpcComponent::ProcessPerceivedEntities() {
	TArray<FAiPerceptionEvent> FilteredPerceptionEvents;

	for (auto PerceptibleComponent : PerceptibleComponents) {
		if (IsValid(PerceptibleComponent)) {
			// Calculate the required perception event properties (Strength, Range, Direction)
			const float Strength = CalculateStrength(PerceptibleComponent);
			const float Range = CalculateRange(PerceptibleComponent);
			const auto Direction = CalculateDirection(PerceptibleComponent);
			const float Duration = CalculateDuration(PerceptibleComponent);

			// Create a perception event and add it to the PerceptionEvents array
			FAiPerceptionEvent PerceptionEvent;
			PerceptionEvent.Perception = PerceptibleComponent->PerceptionData;

			const TArray<FString> StrengthLabels = {TEXT("very weak"), TEXT("weak"), TEXT("medium"), TEXT("strong"), TEXT("very strong")};
			const float StrengthIntervals = 0.2f;
			int StrengthIndex = FMath::Clamp(FMath::RoundToInt(Strength / StrengthIntervals) - 1, 0, StrengthLabels.Num() - 1);
			PerceptionEvent.StrengthValue = Strength;
			PerceptionEvent.Strength = StrengthLabels[StrengthIndex];

			const TArray<FString> RangeLabels = {TEXT("very close"), TEXT("close"), TEXT("medium"), TEXT("far"), TEXT("very far")};
			const float RangeIntervals = 0.2f;
			int RangeIndex = FMath::Clamp(FMath::RoundToInt(Range / RangeIntervals) - 1, 0, RangeLabels.Num() - 1);
			PerceptionEvent.Range = RangeLabels[RangeIndex];
			PerceptionEvent.RangeValue = Range;

			PerceptionEvent.Direction = GetPositionDescriptor(Direction);
			PerceptionEvent.DirectionValue = Direction;

			const TArray<FString> DurationLabels = {TEXT("very short"), TEXT("short"), TEXT("medium"), TEXT("long"), TEXT("very long")};
			const float DurationIntervals = 0.2f;
			int DurationIndex = FMath::Clamp(FMath::RoundToInt(Duration / DurationIntervals) - 1, 0, DurationLabels.Num() - 1);
			PerceptionEvent.Duration = DurationLabels[DurationIndex];
			PerceptionEvent.DurationValue = Duration;

			// Check if the perception event is relevant based on the filtering criteria
			if (IsPerceptionEventRelevant(PerceptionEvent)) {
				FilteredPerceptionEvents.Add(PerceptionEvent);
			}
		}
	}

	TMap<EAiPerception, TArray<FAiPerceptionEvent>> PerceptionEventsMap;

	// Add filtered perception events to the map
	for (const auto& PerceptionEvent : FilteredPerceptionEvents) {
		PerceptionEventsMap.FindOrAdd(PerceptionEvent.Perception.Type).Add(PerceptionEvent);
	}

	// Sort the perception events for each perception type
	for (auto& Elem : PerceptionEventsMap) {
		auto& EventsArray = Elem.Value;

		EventsArray.Sort([](const FAiPerceptionEvent& A, const FAiPerceptionEvent& B) {
			// Sort by a combination of factors (strength, range, and duration)
			return A.StrengthValue * A.DurationValue / A.RangeValue > B.StrengthValue * B.DurationValue / B.RangeValue;
		});
	}

	// Take up to 3 most relevant events for each perception type
	TArray<FAiPerceptionEvent> TopPerceptionEvents;
	for (const auto& Elem : PerceptionEventsMap) {
		const auto& EventsArray = Elem.Value;

		for (int32 i = 0; i < FMath::Min(3, EventsArray.Num()); ++i) {
			TopPerceptionEvents.Add(EventsArray[i]);
		}
	}

	// Convert the perception events to a string
	const FString PerceptionEventsJson = ConvertTopPerceptionEventsToJson(TopPerceptionEvents);

	// Process the collected perception events and send them to the API
	GET_MODULE_SUBSYSTEM(Api2AiSubsystem, Api2, Ai);
	const auto Callback = MakeShared<FOnStringRequestCompleted2>();
	Callback->BindWeakLambda(this, [=](const FString& InString, const EApi2ResponseCode InResponseCode, const FString& InError) {
		UE_LOG(LogTemp, Log, TEXT("Response code: %d"), (int32)InResponseCode);
		UE_LOG(LogTemp, Log, TEXT("Response string: %s"), *InString);
		UE_LOG(LogTemp, Log, TEXT("Response error: %s"), *InError);

		// Process the response
		// parse actions and react to them accordingly
	});
	Api2AiSubsystem->SendString(Api::GetApi2RootUrl() / TEXT("ai/perception"), PerceptionEventsJson, Callback);
}

FString UAiNpcComponent::GetPositionDescriptor(const FVector& Direction) {
	const FString Horizontal = GetHorizontalDescriptor(Direction.X);
	const FString Vertical = GetVerticalDescriptor(Direction.Y);
	const FString Altitude = GetAltitudeDescriptor(Direction.Z);

	return FString::Printf(TEXT("%s-%s-%s"), *Horizontal, *Vertical, *Altitude);
}

FString UAiNpcComponent::GetHorizontalDescriptor(float XDifference) {
	if (XDifference > 0) {
		return "right";
	}
	return "left";
}

FString UAiNpcComponent::GetVerticalDescriptor(float YDifference) {
	if (YDifference > 0) {
		return "front";
	}
	return "behind";
}

FString UAiNpcComponent::GetAltitudeDescriptor(float ZDifference) {
	if (ZDifference > 0) {
		return "above";
	}
	return "below";
}

FString UAiNpcComponent::ConvertTopPerceptionEventsToJson(const TArray<FAiPerceptionEvent>& TopPerceptionEvents) const {
	TMap<EAiPerception, FPerceptionEventArray> PerceptionEventsMap;
	for (const auto& Event : TopPerceptionEvents) {
		FPerceptionEventArray& EventArray = PerceptionEventsMap.FindOrAdd(Event.Perception.Type);
		EventArray.Events.Add(Event);
	}

	FPerceiveEvent PerceiveEvent;
	PerceiveEvent.Perception = PerceptionEventsMap;

	FPerceiveEventsWrapper PerceiveEventsWrapper;
	PerceiveEventsWrapper.Events.Add(PerceiveEvent);

	FString OutputJson;
	FJsonObjectConverter::UStructToJsonObjectString<FPerceiveEventsWrapper>(PerceiveEventsWrapper, OutputJson, 0, 0, 0, nullptr, false);

	return OutputJson;
}

UAiNpcComponent::UAiNpcComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f;

	PerceptionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PerceptionSphere"));
	PerceptionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PerceptionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PerceptionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PerceptionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	PerceptionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	PerceptionSphere->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	PerceptionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	PerceptionSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
	PerceptionSphere->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
	PerceptionSphere->OnComponentBeginOverlap.AddDynamic(this, &UAiNpcComponent::OnPerceptionSphereBeginOverlap);
	PerceptionSphere->OnComponentEndOverlap.AddDynamic(this, &UAiNpcComponent::OnPerceptionSphereEndOverlap);
}

void UAiNpcComponent::BeginPlay() {
	Super::BeginPlay();

	if (const auto* const Owner = GetOwner(); IsValid(Owner)) {
		PerceptionSphere->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}

	if (const auto* const World = GetWorld(); IsValid(World)) {
		World->GetTimerManager().SetTimer(TimerHandle, this, &UAiNpcComponent::ProcessPerceivedEntities, ProcessPerceivedEntitiesInterval, true);
	}
}

void UAiNpcComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (const auto* const World = GetWorld(); IsValid(World)) {
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void UAiNpcComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
