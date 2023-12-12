// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Inspector/InspectComponent.h"

#include "Inspector/Inspector.h"
#include "VeShared.h"
#include "VeGameFramework.h"
#include "Inspector/IInspectable.h"

UInspectComponent::UInspectComponent() {
	if (!IsRunningDedicatedServer()) {
		PrimaryComponentTick.bCanEverTick = true;
	}
}

void UInspectComponent::BeginPlay() {
	Super::BeginPlay();
}

void UInspectComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// if (Inspector.IsValid()) {
	// 	if (const auto* PC = Cast<APlayerController>(GetOwner()); IsValid(PC)) {
	// 		float DeltaX, DeltaY;
	// 		PC->GetInputMouseDelta(DeltaX, DeltaY);
	//
	// 		if (bOrbiting) {
	// 			if (PC->GetInputAnalogKeyState(EKeys::LeftMouseButton)) {
	// 				Inspector->OrbitUpdate(DeltaX, DeltaY);
	// 			}
	// 		}
	//
	// 		if (bPanning) {
	// 			if (PC->GetInputAnalogKeyState(EKeys::RightMouseButton)) {
	// 				Inspector->PanUpdate(DeltaX, DeltaY);
	// 			}
	// 		}
	// 	}
	// }
}

AActor* UInspectComponent::GetPawnInspectTarget() const {
	const auto PlayerController = GetPlayerController();
	if (!IsValid(PlayerController)) {
		VeLogErrorFunc("no player controller");
		return nullptr;
	}

	const auto World = GetWorld();
	if (!IsValid(World)) {
		VeLogErrorFunc("no world");
		return nullptr;
	}

	FVector CameraLocation;
	FRotator CameraRotation;

	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	const auto TraceStart = CameraLocation;
	const auto TraceDirection = CameraRotation.Vector();
	const auto TraceEnd = TraceStart + TraceDirection * MaxFocusDistance;

	// Do a collision line trace ignoring owner.
	FCollisionQueryParams CollisionQueryParams(NAME_TraceInteractionFocus2, true);
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.AddIgnoredActor(GetPawn());

	TArray<FHitResult> Hits;
	World->LineTraceMultiByChannel(Hits, TraceStart, TraceEnd, ECC_InteractionTraceChannel, CollisionQueryParams);

	for (auto Hit : Hits) {
		auto* Actor = Hit.GetActor();

		if (!IsValid(Actor)) {
			continue;
		}

		if (Cast<IInspectable>(Actor)) {
			return Actor;
		}
		return nullptr;
	}

	return nullptr;
}

void UInspectComponent::OnOrbitBegin() {
	bOrbiting = true;
	if (Inspector.IsValid()) {
		Inspector->OrbitBegin();
	}
}

void UInspectComponent::OrbitUpdate(const float DeltaX, const float DeltaY) {
	if (Inspector.IsValid()) {
		Inspector->OrbitUpdate(DeltaX, DeltaY);
	}
}

void UInspectComponent::OnOrbitEnd() {
	bOrbiting = false;
	if (Inspector.IsValid()) {
		Inspector->OrbitEnd();
	}
}

void UInspectComponent::OnPanBegin() {
	bPanning = true;
	if (Inspector.IsValid()) {
		Inspector->PanBegin();
	}
}

void UInspectComponent::PanUpdate(const float DeltaX, const float DeltaY) {
	if (Inspector.IsValid()) {
		Inspector->PanUpdate(DeltaX, DeltaY);
	}
}

void UInspectComponent::OnPanEnd() {
	bPanning = false;
	if (Inspector.IsValid()) {
		Inspector->PanEnd();
	}
}

AActor* UInspectComponent::BeginInspect() {
	InspectTarget = GetPawnInspectTarget();
	if (InspectTarget.IsValid()) {
		BeginInspectActor(InspectTarget.Get());
		return InspectTarget.Get();
	}
	return nullptr;
}

void UInspectComponent::BeginInspectActor(AActor* InInspectTarget) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	if (IsValid(InInspectTarget)) {
		if (Inspector.IsValid()) {
			if (InInspectTarget == Inspector->GetAttachParentActor()) {
				return;
			}

			Inspector->Destroy();
			Inspector.Reset();
		}

		if (auto* World = GetWorld(); IsValid(World)) {
			if (Inspector = World->SpawnActor<AInspector>(InspectorClass); Inspector.IsValid()) {
				Inspector->SetInspectedActor(InInspectTarget);
				Inspector->ResetView();
				Inspector->CaptureScene();
				OnBeginInspect.Broadcast(InInspectTarget);
			}
		}
	} else {
		if (Inspector.IsValid()) {
			Inspector->Destroy();
			Inspector.Reset();
		}
	}
}

void UInspectComponent::EndInspect() {
	if (IsRunningDedicatedServer()) {
		return;
	}

	OnPanEnd();
	OnOrbitEnd();

	if (Inspector.IsValid()) {
		OnEndInspect.Broadcast();
		Inspector->Destroy();
	}
}

void UInspectComponent::Zoom(const float ZoomValue) const {
	if (Inspector.IsValid()) {
		Inspector->Zoom(ZoomValue);
	}
}

void UInspectComponent::ResetView() const {
	if (Inspector.IsValid()) {
		Inspector->ResetView();
	}
}
