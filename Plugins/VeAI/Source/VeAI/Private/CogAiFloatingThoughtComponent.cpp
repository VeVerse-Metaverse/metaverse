// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiFloatingThoughtComponent.h"
#include "Components/CapsuleComponent.h"

UCogAiFloatingThoughtComponent::UCogAiFloatingThoughtComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UCogAiFloatingThoughtComponent::SpawnFloatingThoughts_Implementation(const FString& Thoughts, const FString& Emotion) {
	const auto* Owner = GetOwner();

	if (!IsValid(Owner) || !Owner->HasAuthority()) {
		return;
	}

	auto* const World = GetWorld();

	if (!IsValid(World)) {
		return;
	}

	if (FloatingThoughtClass && IsValid(GetWorld())) {
		FVector Location = Owner->GetActorLocation();
		Location.X += FMath::RandRange(-FloatingThoughtsSpawnRadius, FloatingThoughtsSpawnRadius);
		Location.Y += FMath::RandRange(-FloatingThoughtsSpawnRadius, FloatingThoughtsSpawnRadius);

		if (const auto* CapsuleComponent = Owner->FindComponentByClass<UCapsuleComponent>(); IsValid(CapsuleComponent)) {
			const float Height = CapsuleComponent->GetScaledCapsuleHalfHeight();
			Location.Z += Height;
		}

		if (auto* FloatingThoughts = World->SpawnActor<ACogAiFloatingThought>(FloatingThoughtClass, Location, FRotator::ZeroRotator); IsValid(FloatingThoughts)) {
			FloatingThoughts->SetThoughtAndEmotion(Thoughts, Emotion);
		}
	}
}
