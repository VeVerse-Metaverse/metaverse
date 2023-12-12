// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Actors/VRHUD.h"

#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "VR/VRRootWidget.h"
#include "VeShared.h"

AVRHUD::AVRHUD() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.0f;

	MenuWidget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("MenuWidget")));
	MenuWidget->SetGeometryMode(EWidgetGeometryMode::Cylinder);
	MenuWidget->SetCylinderArcAngle(75.0f);
	MenuWidget->SetDrawAtDesiredSize(true);
	MenuWidget->SetDrawSize(FVector2D(1000.0f, 500.0f));
	MenuWidget->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
}

void AVRHUD::SetPawn(APawn* InPawn) {
	Pawn = InPawn;
}

void AVRHUD::SetIsPinned(const bool bInIsPinned) {
	if (bInIsPinned) {
		// Setup the lambda for distance checks.
		if (!DistanceCheckTimerHandle.IsValid()) {
			const FTimerDelegate DistanceCheckTimerDelegate = FTimerDelegate::CreateWeakLambda(this, [=]() {
				if (!Pawn) {
					SetIsActive(false);
					SetIsPinned(false);
					return;
				}

				if (GetHorizontalDistanceTo(Pawn) >= PinDistanceLimit) {
					SetIsActive(false);
					SetIsPinned(false);
				}
			});

			if (const UWorld* const World = GetWorld()) {
				World->GetTimerManager().SetTimer(DistanceCheckTimerHandle, DistanceCheckTimerDelegate, PinDistanceCheckRate, true);
			}
		}
	} else {
		if (DistanceCheckTimerHandle.IsValid()) {
			if (const UWorld* const World = GetWorld()) {
				World->GetTimerManager().ClearTimer(DistanceCheckTimerHandle);
			}

			DistanceCheckTimerHandle.Invalidate();
		}
	}

	bIsPinned = bInIsPinned;
}

bool AVRHUD::GetIsPinned() const {
	return bIsPinned;
}

void AVRHUD::SetIsActive(const bool bInIsActive) {
	bIsActive = bInIsActive;
	SetActorHiddenInGame(!bInIsActive);
}

bool AVRHUD::GetIsActive() const {
	return bIsActive;
}

void AVRHUD::BeginPlay() {
	Super::BeginPlay();
}

void AVRHUD::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!bIsActive) {
		return;
	}

	if (bIsPinned) {
		return;
	}

	if (Pawn) {
		// Current location.
		const FVector MenuLocation = GetActorLocation();

		// Player camera location.
		FVector CameraLocation = Pawn->GetActorLocation();

		// Player camera forward vector.
		FVector CameraForwardVector = Pawn->GetControlRotation().Vector();

		// Try to get correct values from the camera manager.
		if (const auto Controller = Pawn->GetController()) {
			if (const auto PlayerController = Cast<APlayerController>(Controller)) {
				if (const auto CameraManager = PlayerController->PlayerCameraManager) {
					// Try to update camera location using the camera manager.
					CameraLocation = CameraManager->GetCameraLocation();
					// Try to update camera forward vector using the camera manager. 
					CameraForwardVector = CameraManager->GetCameraRotation().Vector();
				}
			}
		}

		// Take normalized vector between the menu and the player camera. 
		FVector Direction = MenuLocation - CameraLocation;
		Direction.Normalize();

		// Interpolate current direction to the target vector.
		FVector InterpolationDelta = FMath::VInterpTo(Direction, CameraForwardVector, DeltaTime, InterpolationSpeed);

		// Multiply delta by the distance.
		InterpolationDelta *= Distance;

		// Calculate current position by adding current camera location to the delta vector.
		const FVector CurrentPosition = InterpolationDelta + FVector(CameraLocation.X, CameraLocation.Y, 0.0f);

		// Calculate target position actor should be moved to.
		const FVector TargetPosition = FVector(CurrentPosition.X, CurrentPosition.Y, CameraLocation.Z - CameraVerticalOffset);

		// Calculate look at rotation.
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(TargetPosition, CameraLocation);

		// Reset pitch and roll as we only rotate menu by the vertical axis.
		TargetRotation.Roll = 0;
		TargetRotation.Pitch = 0;

		// Update the actor location and rotation.
		SetActorLocationAndRotation(TargetPosition, TargetRotation);
	}
}

UVRRootWidget* AVRHUD::GetRootWidget() const {
	if (IsValid(MenuWidget)) {
		return Cast<UVRRootWidget>(MenuWidget->GetWidget());
	}
	return nullptr;
}
