// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiFloatingThought.h"

#include "CogAiFloatingThoughtWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

ACogAiFloatingThought::ACogAiFloatingThought()
	: Super() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	FloatingThoughtWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("FloatingEmotionWidget");
	FloatingThoughtWidgetComponent->SetupAttachment(RootComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	AudioComponent->SetupAttachment(RootComponent);
}

void ACogAiFloatingThought::SetThoughtAndEmotion(const FString& InThoughts, const FString& InEmotionName) {
	if (auto* Widget = FloatingThoughtWidgetComponent->GetWidget(); IsValid(Widget)) {
		if (const auto* FloatingThoughtWidget = Cast<UCogAiFloatingThoughtWidget>(Widget)) {
			FloatingThoughtWidget->SetThoughtAndEmotion(InThoughts, InEmotionName);
		}
	}
}

void ACogAiFloatingThought::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	if (!IsValid(FloatingThoughtWidgetComponent)) {
		FloatingThoughtWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("FloatingEmotionWidget");
		FloatingThoughtWidgetComponent->SetupAttachment(RootComponent);
	}
}

void ACogAiFloatingThought::BeginPlay() {
	Super::BeginPlay();

	if (FloatingThoughtWidgetClass && IsValid(FloatingThoughtWidgetComponent)) {
		FloatingThoughtWidgetComponent->SetWidgetClass(FloatingThoughtWidgetClass);
	}

	if (IsValid(AudioComponent) && IsValid(Sound)) {
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}

	InitialLocation = GetActorLocation();
}

void ACogAiFloatingThought::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	// Update the location.
	FVector NewLocation = GetActorLocation();
	NewLocation.Z += FlyUpSpeed * DeltaTime;
	SetActorLocation(NewLocation);

	float DistanceAlpha = 1.0f;

	const auto* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PC)) {
		const auto* Pawn = PC->GetPawn();
		if (IsValid(Pawn)) {
			if (const auto Distance = FVector::Dist(Pawn->GetActorLocation(), GetActorLocation()); Distance > MaxVisibilityDistance) {
				DistanceAlpha = 0.0f;
			} else if (Distance < MinVisibilityDistance) {
				DistanceAlpha = 1.0f;
			} else {
				const float DistanceInRange = FMath::Clamp(Distance - MinVisibilityDistance, 0.0f, MaxVisibilityDistance - MinVisibilityDistance);
				DistanceAlpha = 1.0f - FMath::SmoothStep(0.0f, 1.0f, DistanceInRange / (MaxVisibilityDistance - MinVisibilityDistance));
			}
		}
	}

	// Fade out gradually.
	if (ElapsedTime >= LifeTime - FadeOutDuration) {
		const float Alpha = 1.0f - (ElapsedTime - (LifeTime - FadeOutDuration)) / FadeOutDuration;
		if (IsValid(FloatingThoughtWidgetComponent)) {
			if (auto* Widget = FloatingThoughtWidgetComponent->GetWidget(); IsValid(Widget)) {
				Widget->SetRenderOpacity(Alpha * DistanceAlpha);
			}
		}
	} else {
		if (IsValid(FloatingThoughtWidgetComponent)) {
			if (auto* Widget = FloatingThoughtWidgetComponent->GetWidget(); IsValid(Widget)) {
				Widget->SetRenderOpacity(DistanceAlpha);
			}
		}
	}

	// Check if the world is valid.
	const auto* const World = GetWorld();
	if (!IsValid(World)) {
		return;
	}

	// Rotate to face the local player's camera.
	auto PlayerLocation = FVector::ZeroVector;
	const auto* PlayerController = World->GetFirstPlayerController();

	const UCameraComponent* ActiveCameraComponent = nullptr;

	// Try to infer the player's camera location from the view target.
	if (IsValid(PlayerController) && IsValid(PlayerController->GetViewTarget())) {
		ActiveCameraComponent = PlayerController->GetViewTarget()->FindComponentByClass<UCameraComponent>();
		if (IsValid(ActiveCameraComponent)) {
			PlayerLocation = ActiveCameraComponent->GetComponentLocation();
		}
	}

	// Try to infer the player's camera location from the camera manager.
	if (IsValid(PlayerController) && (!IsValid(ActiveCameraComponent) || PlayerLocation == FVector::ZeroVector)) {
		if (const auto CameraManager = PlayerController->PlayerCameraManager) {
			PlayerLocation = CameraManager->GetCameraLocation();
		}
	}

	// Fall back to the player's location if we couldn't find a camera.
	if (IsValid(PlayerController)) {
		if (const APawn* PlayerPawn = PlayerController->GetPawn(); PlayerLocation == FVector::ZeroVector && IsValid(PlayerPawn)) {
			PlayerLocation = PlayerPawn->GetActorLocation();
		}
	}

	// Update the rotation to face the player.
	const FVector MyActorLocation = GetActorLocation();
	const FRotator NewRotation = FRotationMatrix::MakeFromX((PlayerLocation - MyActorLocation).GetSafeNormal()).Rotator();
	SetActorRotation(NewRotation);

	// Destroy the actor when it's time.
	if (ElapsedTime >= LifeTime) {
		Destroy();
	}
}
