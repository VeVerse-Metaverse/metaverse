// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "TestVRMenu.h"

#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "VeShared.h"


// Sets default values
ATestVRMenu::ATestVRMenu() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup menu widget.
	MenuWidget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("MenuWidget")));
	MenuWidget->SetGeometryMode(EWidgetGeometryMode::Cylinder);
	MenuWidget->SetCylinderArcAngle(75.0f);
	MenuWidget->SetDrawAtDesiredSize(true);
	MenuWidget->SetDrawSize(FVector2D(512.0f, 256.0f));
}

// Called when the game starts or when spawned
void ATestVRMenu::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ATestVRMenu::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

	if (OwningPawn) {
		// Current location.
		const FVector MenuLocation = GetActorLocation();

		// Player camera location.
		FVector CameraLocation = OwningPawn->GetActorLocation();

		// Player camera forward vector.
		FVector CameraForwardVector = OwningPawn->GetControlRotation().Vector();

		// Try to get correct values from the camera manager.
		if (const auto Controller = OwningPawn->GetController()) {
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
		const FVector TargetPosition = FVector(CurrentPosition.X, CurrentPosition.Y, CameraLocation.Z);

		// Calculate look at rotation.
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(TargetPosition, CameraLocation);

		// Reset pitch and roll as we only rotate menu by the vertical axis.
		TargetRotation.Roll = 0;
		TargetRotation.Pitch = 0;

		// Update the actor location and rotation.
		SetActorLocationAndRotation(TargetPosition, TargetRotation);
	}
}

void ATestVRMenu::SetPawn(APawn* InPawn) {
	OwningPawn = InPawn;
}
