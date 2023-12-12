// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "Components/InteractiveControlComponent.h"

#include "DrawDebugHelpers.h"
#include "Interaction.h"
#include "InteractionMacros.h"
#include "InteractionNames.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"
#include "Components/InteractiveTargetComponent.h"
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Pawns/InteractiveCharacter.h"
#include "VeShared.h"

UInteractiveControlComponent::UInteractiveControlComponent()
	: Super() {

	SetIsReplicatedByDefault(true);
	UActorComponent::SetAutoActivate(true);

	PrimaryComponentTick.bCanEverTick = true;
#if UE_EDITOR && UE_BUILD_DEBUG
	PrimaryComponentTick.TickInterval = 0.0f;
#else
	// Optimized for production.
	PrimaryComponentTick.TickInterval = 0.125f;
#endif

#if WITH_EDITORONLY_DATA
	bDebugDrawFocusTrace = false;
#endif

#if UE_EDITOR && UE_BUILD_DEBUG
	bDebugDrawFocusTrace = true;
#endif

#if PLATFORM_WINDOWS
	Platform = EInteractionPlatform::Windows;
#elif PLATFORM_ANDROID
	Platform = EInteractionPlatform::Android;
#elif PLATFORM_MAC
	Platform = EInteractionPlatform::Mac;
#elif PLATFORM_IOS
	Platform = EInteractionPlatform::IOS;
#else
	Platform = EInteractionPlatform::Unknown;
#endif
}

#pragma region Component GameFramework
void UInteractiveControlComponent::BeginPlay() {
	Super::BeginPlay();
	if (ACTOR_IS_AUTHORITY(GetOwner())) {
		bIsAuthority = true;
	}

	if (ControlMode == EInteractionControlMode::Unknown) {
		const auto MotionController = GetMotionController();
		if (MotionController != nullptr) {
			ControlMode = EInteractionControlMode::MotionController;
			return;
		}
		UE_LOG(LogInteraction, Error, TEXT("UInteractionControlComponent::BeginPlay() -> Can not automatically determine Control Mode"))
		// todo: Automatically determine Control Mode
	}
}

void UInteractiveControlComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
												 FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto Owner = GetOwner();
	if (!Owner) {
		LogF("no owner");
		return;
	};

	if (!IsValid(GetPlayerController())) {
		return;
	}

	const bool bIsServer = (Owner->HasAuthority() && (Owner->GetNetMode() < NM_Client));
	const bool bIsStandalone = (Owner->GetNetMode() == NM_Standalone);

	// Here we have two dimensions of cases to check: network cases (standalone, server, client) and target cases listed below. Total of 15 possible cases.
	// NewT == null, OldT == null
	// NewT != null, OldT == null
	// NewT == null, OldT != null
	// NewT != null, OldT != null, NewT == OldT
	// NewT != null, OldT != null, NewT != OldT

	// Allow standalone, servers and clients (only if permitted) to do focus checks.
	if (bUseFocus && (bIsStandalone || bIsServer || bProcessClientFocus)) {

		// Do a line trace and find the focused target.
		const auto NewFocusedTarget = ControlMode == EInteractionControlMode::MotionController ? GetMotionControllerFocusedTarget() : GetPawnFocusedTarget();

		// Check if have a new focused target. 
		if (NewFocusedTarget != FocusedTarget) {
			// Check if the previous target is available.
			if (FocusedTarget) {
				if (bIsStandalone) {
					Standalone_FocusEndLocal(FocusedTarget);
				} else if (FocusedTarget->IsClientOnly(EInteractionType::Focus) && !bIsServer) {
					Client_FocusEndLocal(FocusedTarget);
				} else if (!FocusedTarget->IsClientOnly(EInteractionType::Focus) && bIsServer) {
					Server_FocusEndLocal(FocusedTarget);
				}
			}

			// Check if we have a new focused target.			
			if (NewFocusedTarget) {
				if (bIsStandalone) {
					Standalone_FocusBeginLocal(NewFocusedTarget, FocusOrigin, FocusLocation, FocusNormal);
				} else if (NewFocusedTarget->IsClientOnly(EInteractionType::Focus) && !bIsServer) {
					Client_FocusBeginLocal(NewFocusedTarget, FocusOrigin, FocusLocation, FocusNormal);
				} else if (!NewFocusedTarget->IsClientOnly(EInteractionType::Focus) && bIsServer) {
					Server_FocusBeginLocal(NewFocusedTarget, FocusOrigin, FocusLocation, FocusNormal);
				}
			}

			// Update the cached focused target.
			FocusedTarget = NewFocusedTarget;
		}
	}

	// Allow standalone, servers and clients (only if permitted) to do overlap checks.
	if (bUseOverlap && (bIsStandalone || bIsServer || bProcessClientOverlap)) {

		// Do a overlap check and find the best overlapped target among others.
		const auto NewOverlappedTarget = ControlMode == EInteractionControlMode::MotionController ? GetMotionControllerOverlappedTarget() : GetPawnOverlappedTarget();

		// Check if had a new overlapped target.
		if (NewOverlappedTarget != OverlappedTarget) {
			// // Check if the previous target is available.
			// if (OverlappedTarget) {
			// 	if (bIsStandalone) {
			// 		Standalone_OverlapEndLocal(OverlappedTarget);
			// 	} else if (OverlappedTarget->IsClientOnly(EInteractionType::Overlap) && !bIsServer) {
			// 		Client_OverlapEndLocal(OverlappedTarget);
			// 	} else if (!OverlappedTarget->IsClientOnly(EInteractionType::Overlap) && bIsServer) {
			// 		Server_OverlapEndLocal(OverlappedTarget);
			// 	}
			// }

			// // Check if we have a new overlapped target.
			// if (NewOverlappedTarget) {
			// 	if (bIsStandalone) {
			// 		Standalone_OverlapBeginLocal(NewOverlappedTarget, OverlapOrigin, OverlapLocation, OverlapNormal);
			// 	} else if (NewOverlappedTarget->IsClientOnly(EInteractionType::Overlap) && !bIsServer) {
			// 		Client_OverlapBeginLocal(NewOverlappedTarget, OverlapOrigin, OverlapLocation, OverlapNormal);
			// 	} else if (!NewOverlappedTarget->IsClientOnly(EInteractionType::Overlap) && bIsServer) {
			// 		Server_OverlapBeginLocal(NewOverlappedTarget, OverlapOrigin, OverlapLocation, OverlapNormal);
			// 	}
			// }

			// Update the cached overlapped target.
			OverlappedTarget = NewOverlappedTarget;
		}
	}
}

void UInteractiveControlComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractiveControlComponent, RightHandFingerSettings);
	DOREPLIFETIME(UInteractiveControlComponent, LeftHandFingerSettings);
}

void UInteractiveControlComponent::SetMenuMode(const FName Mode) {
	const auto Target = Mode == NAME_None ? nullptr : SelectActivateTarget();
	OnMenuModeChange.Broadcast(MenuMode, Mode, this, Target);
	MenuMode = Mode;
}
#pragma endregion

#pragma region GetTargetHelpers
UInteractiveTargetComponent* UInteractiveControlComponent::GetPawnFocusedTarget() {
	FocusLocation = FVector::ZeroVector;
	FocusNormal = FVector::ZeroVector;
	FocusOrigin = FVector::ZeroVector;

	const auto Owner = GetOwner();
	if (!(Owner)) {
		UE_LOG(LogInteraction, Error, TEXT("CHECK_RETURN(%s) Failed @ %s"), TEXT("Owner"), FUNC_NAME);
		return nullptr;
	};

	// Do not process on simulated proxy pawns.
	if (((Owner->GetLocalRole() == ROLE_SimulatedProxy))) {
		return nullptr;
	}

	const auto OwnerPawn = Cast<APawn>(Owner);
	if (!(OwnerPawn)) {
		UE_LOG(LogInteraction, Error, TEXT("CHECK_RETURN(%s) Failed @ %s"), TEXT("OwnerPawn"), FUNC_NAME);
		return nullptr;
	};

	const auto MyController = OwnerPawn->GetController();
	if (!(MyController)) {
		UE_LOG(LogInteraction, Error, TEXT("CHECK_RETURN(%s) Failed @ %s"), TEXT("MyController"), FUNC_NAME);
		return nullptr;
	};

	const auto World = GetWorld();
	if (!(World)) {
		UE_LOG(LogInteraction, Error, TEXT("CHECK_RETURN(%s) Failed @ %s"), TEXT("World"), FUNC_NAME);
		return nullptr;
	};

	FVector CameraLocation;
	FRotator CameraRotation;

	MyController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	const auto TraceStart = CameraLocation;
	const auto TraceDirection = CameraRotation.Vector();
	const auto TraceEnd = TraceStart + (TraceDirection * MaxFocusDistance);

	FocusOrigin = TraceStart;

	FCollisionQueryParams CollisionQueryParams(NAME_TraceInteractionFocus, true, Owner);
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_InteractionTraceChannel, CollisionQueryParams);

	const auto Actor = Hit.GetActor();

	if (Actor == nullptr) {
		return nullptr;
	}

#if WITH_EDITORONLY_DATA
	if (bDebugDrawFocusTrace) {
		UKismetSystemLibrary::DrawDebugSphere(Owner, Hit.Location, 10.f, 24, FLinearColor::Red);
	}
#endif

	const auto TargetComponent = Actor->FindComponentByClass<UInteractiveTargetComponent>();
	if (!TargetComponent) {
		return nullptr;
	}

	FocusLocation = Hit.ImpactPoint;
	FocusNormal = Hit.ImpactNormal;

	return TargetComponent;
}

UInteractiveTargetComponent* UInteractiveControlComponent::GetPawnOverlappedTarget() {
	OverlapLocation = FVector::ZeroVector;
	OverlapNormal = FVector::ZeroVector;
	OverlapOrigin = FVector::ZeroVector;

	const auto Owner = GetOwner();
	CHECK_RETURN_VALUE(Owner, nullptr);

	if (OverlappingTargets.Num() == 0) {
		return nullptr;
	}

	UInteractiveTargetComponent* ClosestTarget = nullptr;

	// Optimize the 360 degree overlap direction case.
	if (MinOverlapDirection == -1) {
		ClosestTarget = OverlappingTargets[0];
	} else {
		auto MaxDirectionDotProduct = MinOverlapDirection;

		for (auto Target : OverlappingTargets) {

			const auto TargetOwner = Target->GetOwner();
			if (TargetOwner == nullptr) {
				continue;
			}

			// Check if actor is in forward direction of the character.
			auto Direction = TargetOwner->GetActorLocation() - Owner->GetActorLocation();
			Direction.Normalize();

			const auto DirectionDotProduct = FVector::DotProduct(Owner->GetActorForwardVector(), Direction);

			if (DirectionDotProduct > MaxDirectionDotProduct) {
				MaxDirectionDotProduct = DirectionDotProduct;
				ClosestTarget = Target;
			}
		}
	}

	if (!ACTOR_IS_AUTHORITY(Owner)) {
		return ClosestTarget;
	}

	FVector TraceStart;
	FVector BoxExtentStart;
	Owner->GetActorBounds(false, TraceStart, BoxExtentStart);

	FVector TraceEnd;
	FVector BoxExtentEnd;
	const auto TargetOwner = ClosestTarget->GetOwner();
	CHECK_RETURN_VALUE(TargetOwner, ClosestTarget);

	TargetOwner->GetActorBounds(false, TraceEnd, BoxExtentEnd);

	OverlapOrigin = TraceStart;

	const auto World = GetWorld();
	CHECK_RETURN_VALUE(World, ClosestTarget);

	FCollisionQueryParams CollisionQueryParams(NAME_TraceInteractionOverlap, true, Owner);
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_InteractionTraceChannel, CollisionQueryParams);

	const auto Actor = Hit.GetActor();

	if (Actor == nullptr) {
		return ClosestTarget;
	}
	const auto TargetComponent = Actor->FindComponentByClass<UInteractiveTargetComponent>();

	if (TargetComponent == nullptr || TargetComponent != ClosestTarget) {
		return ClosestTarget;
	}

	OverlapLocation = Hit.ImpactPoint;
	OverlapNormal = Hit.ImpactNormal;

	return ClosestTarget;
}

UInteractiveTargetComponent* UInteractiveControlComponent::GetMotionControllerFocusedTarget() {
	FocusLocation = FVector::ZeroVector;
	FocusNormal = FVector::ZeroVector;

	const auto OwningMotionController = GetOwner();
	CHECK_RETURN_VALUE(OwningMotionController, nullptr);
	const auto MotionControllerComponent = Cast<UMotionControllerComponent>(OwningMotionController->GetComponentByClass(UMotionControllerComponent::StaticClass()));
	CHECK_RETURN_VALUE(MotionControllerComponent, nullptr);

	const auto World = GetWorld();
	CHECK_RETURN_VALUE(World, nullptr);

	const auto TraceStart = MotionControllerComponent->GetComponentLocation();
	const auto TraceDirection = MotionControllerComponent->GetForwardVector(); // OwningMotionController->GetActorRotation().Vector();
	const auto TraceEnd = TraceStart + (TraceDirection * MaxFocusDistance);

	FocusOrigin = TraceStart;

	// Do a collision line trace ignoring self.
	FCollisionQueryParams CollisionQueryParams(NAME_TraceInteractionFocus, true, OwningMotionController);
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bTraceComplex = false;
#if UE_EDITOR
	CollisionQueryParams.bDebugQuery = true;
#endif

	FHitResult Hit(ForceInit);
	World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_InteractionTraceChannel, CollisionQueryParams);

#if WITH_EDITORONLY_DATA
	if (bDebugDrawFocusTrace) {
		DrawDebugLine(World, TraceStart, TraceEnd, FColor::Red, false, 0.1f, 0.0f, 0.25f);
	}
#endif
	const auto Actor = Hit.GetActor();

	if (Actor == nullptr) {
		return nullptr;
	}

#if WITH_EDITORONLY_DATA
	if (bDebugDrawFocusTrace) {
		UKismetSystemLibrary::DrawDebugSphere(OwningMotionController, Hit.Location, 7.f, 12, FLinearColor::Red, 0.5f, 0.5f);
	}
#endif

	const auto TargetComponent = Actor->FindComponentByClass<UInteractiveTargetComponent>();

	if (!TargetComponent) {
		return nullptr;
	}

	FocusLocation = Hit.ImpactPoint;
	FocusNormal = Hit.ImpactNormal;

	return TargetComponent;
}

UInteractiveTargetComponent* UInteractiveControlComponent::GetMotionControllerOverlappedTarget() {
	OverlapLocation = FVector::ZeroVector;
	OverlapNormal = FVector::ZeroVector;
	OverlapOrigin = FVector::ZeroVector;

	const auto OwningMotionController = GetOwner();
	CHECK_RETURN_VALUE(OwningMotionController, nullptr);
	const auto MotionControllerComponent = Cast<UMotionControllerComponent>(OwningMotionController->GetComponentByClass(UMotionControllerComponent::StaticClass()));
	CHECK_RETURN_VALUE(MotionControllerComponent, nullptr);

	if (OverlappingTargets.Num() == 0) {
		return nullptr;
	}

	UInteractiveTargetComponent* ClosestTarget = nullptr;

	// Optimize the 360 degree overlap direction case.
	if (MinOverlapDirection == -1) {
		ClosestTarget = OverlappingTargets[0];
	} else {
		auto MaxDirectionDotProduct = MinOverlapDirection;

		for (auto Target : OverlappingTargets) {

			const auto TargetOwner = Target->GetOwner();
			if (!TargetOwner) {
				continue;
			}

			// Check if actor is in forward direction of the motion controller.
			auto Direction = TargetOwner->GetActorLocation() - OwningMotionController->GetActorLocation();
			Direction.Normalize();

			const auto DirectionDotProduct = FVector::DotProduct(OwningMotionController->GetActorForwardVector(), Direction);

			if (DirectionDotProduct >= MaxDirectionDotProduct) {
				MaxDirectionDotProduct = DirectionDotProduct;
				ClosestTarget = Target;
			}
		}
	}

	if (!(OwningMotionController->HasAuthority() && OwningMotionController->GetNetMode() < NM_Client)) {
		return ClosestTarget;
	}

	FVector TraceStart = MotionControllerComponent->GetComponentLocation();

	FVector TraceEnd;
	FVector BoxExtentEnd;
	const auto TargetOwner = ClosestTarget->GetOwner();
	CHECK_RETURN_VALUE(TargetOwner, ClosestTarget);
	TargetOwner->GetActorBounds(false, TraceEnd, BoxExtentEnd);

	OverlapOrigin = TraceStart;

	const auto World = GetWorld();
	CHECK_RETURN_VALUE(World, ClosestTarget);

	// Do a collision line trace ignoring self.
	FCollisionQueryParams CollisionQueryParams(NAME_TraceInteractionOverlap, true, OwningMotionController);
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_InteractionTraceChannel, CollisionQueryParams);

	const auto Actor = Hit.GetActor();

	if (!Actor) {
		return ClosestTarget;
	}

	const auto TargetComponent = Actor->FindComponentByClass<UInteractiveTargetComponent>();

	if (TargetComponent == nullptr || TargetComponent != ClosestTarget) {
		return ClosestTarget;
	}

	OverlapLocation = Hit.ImpactPoint;
	OverlapNormal = Hit.ImpactNormal;

	return ClosestTarget;
}

#pragma endregion

#pragma region Attach
void UInteractiveControlComponent::AttachTargetToMotionController(UInteractiveTargetComponent* InteractiveTarget, UMotionControllerComponent* MotionControllerComponent) {
	// Target handles to attach.
	TArray<UPrimitiveComponent*> TargetHandles;
	// Target hand meshes of handles.
	TArray<USkeletalMeshComponent*> TargetHandMeshes;
	// Target bone transform.
	FTransform* TargetBoneTransform;
	// Finger settings for current hand
	TArray<FInteractiveFingerSettings>* FingerSettings;
	// Target hand finger settings
	FInteractiveFingerSettings* TargetFingerSettings;
	// Control component to attach target.
	USceneComponent* AttachComponent;
	// Socket name to attach defined in the control component.
	FName ControlAttachSocketName;
	// Socket name the target wants to attach overriding the control setting.
	FName TargetAttachSocketName;

	if (MotionControllerComponent->MotionSource == FXRMotionControllerBase::LeftHandSourceId) {
		TargetHandles = InteractiveTarget->LeftHandHandles;
		TargetHandMeshes = InteractiveTarget->LeftHandMeshes;
		TargetBoneTransform = &InteractiveTarget->LeftHandTransform;
		FingerSettings = &InteractiveTarget->LeftHandFingerSettings;
		TargetFingerSettings = &LeftHandFingerSettings;
		AttachComponent = LeftHandAttachComponent ? LeftHandAttachComponent : MotionControllerComponent;
		ControlAttachSocketName = LeftHandAttachSocketName;
		TargetAttachSocketName = InteractiveTarget->LeftHandAttachSocketName;
	} else if (MotionControllerComponent->MotionSource == FXRMotionControllerBase::RightHandSourceId) {
		TargetHandles = InteractiveTarget->RightHandHandles;
		TargetHandMeshes = InteractiveTarget->RightHandMeshes;
		TargetBoneTransform = &InteractiveTarget->RightHandTransform;
		FingerSettings = &InteractiveTarget->RightHandFingerSettings;
		TargetFingerSettings = &RightHandFingerSettings;
		AttachComponent = RightHandAttachComponent ? RightHandAttachComponent : MotionControllerComponent;
		ControlAttachSocketName = RightHandAttachSocketName;
		TargetAttachSocketName = InteractiveTarget->RightHandAttachSocketName;
	} else {
		TargetHandles = InteractiveTarget->RightHandHandles;
		TargetHandMeshes = InteractiveTarget->RightHandMeshes;
		TargetBoneTransform = &InteractiveTarget->RightHandTransform;
		FingerSettings = &InteractiveTarget->RightHandFingerSettings;
		TargetFingerSettings = &RightHandFingerSettings;
		AttachComponent = RightHandAttachComponent ? RightHandAttachComponent : MotionControllerComponent;
		ControlAttachSocketName = RightHandAttachSocketName;
		TargetAttachSocketName = InteractiveTarget->RightHandAttachSocketName;
	}

	if (InteractiveTarget->bUsePhysicsHandles && PhysicsHandle && TargetHandles.Num() > 0) {
		/* Finding the closest handle to the Motion Controller **/
		const auto MotionControllerLocation = MotionControllerComponent->GetComponentLocation();
		TArray<float> HandlesDistance;
		for (const auto Handle : TargetHandles) {
			HandlesDistance.Add(FVector::Dist(Handle->GetComponentLocation(), MotionControllerLocation));
		}
		int32 MinDistanceIndex;
		FGenericPlatformMath::Min(HandlesDistance, &MinDistanceIndex);
		const auto ClosestHandle = TargetHandles[MinDistanceIndex];

		CHECK_RETURN(ClosestHandle);
		*TargetBoneTransform = TargetHandMeshes[MinDistanceIndex]->GetRelativeTransform();
		InteractiveTarget->ResetPhysicsComponent(true, InteractiveTarget->bResetVelocityOnAttach);
		PhysicsHandle->GrabComponentAtLocationWithRotation(InteractiveTarget->PhysicsComponent, NAME_None,
														   FVector(ClosestHandle->GetComponentLocation()),
														   FRotator(ClosestHandle->GetComponentRotation()));
		if (InteractiveTarget->bUseFingers) {
			*TargetFingerSettings = (*FingerSettings)[MinDistanceIndex];
		}
	} else {
		InteractiveTarget->ResetPhysicsComponent(false, InteractiveTarget->bResetVelocityOnAttach);
		InteractiveTarget->SetAttachedToComponent(AttachComponent);
		InteractiveTarget->GetOwner()->AttachToComponent(MotionControllerComponent,
														 FAttachmentTransformRules(InteractiveTarget->DefaultMotionControllerLocationAttachmentRule,
																				   InteractiveTarget->DefaultMotionControllerRotationAttachmentRule,
																				   InteractiveTarget->DefaultMotionControllerScaleAttachmentRule,
																				   InteractiveTarget->DefaultMotionControllerWeldSimulatedBodies),
														 ControlAttachSocketName == TargetAttachSocketName && TargetAttachSocketName != NAME_None
															 ? ControlAttachSocketName
															 : TargetAttachSocketName);
	}
}

void UInteractiveControlComponent::AttachTargetToPawn(UInteractiveTargetComponent* InteractiveTarget) {
	if (InteractiveTarget->bUsePhysicsHandles && PhysicsHandle) {
		CHECK_RETURN(InteractiveTarget->PivotHandle);
		InteractiveTarget->PivotHandTransform = InteractiveTarget->PivotHandMesh->GetRelativeTransform();
		InteractiveTarget->ResetPhysicsComponent(true, InteractiveTarget->bResetVelocityOnAttach);
		PhysicsHandle->GrabComponentAtLocationWithRotation(InteractiveTarget->PhysicsComponent, NAME_None,
														   FVector(InteractiveTarget->PivotHandle->GetComponentLocation()),
														   FRotator(InteractiveTarget->PivotHandle->GetComponentRotation()));

		GrabbedObjectLocation = InteractiveTarget->PivotHandle->GetComponentLocation();
		GrabbedObjectRotation = InteractiveTarget->PivotHandle->GetRelativeRotation().GetInverse();
		GrabbedObjectScale = InteractiveTarget->GetOwner()->GetActorRelativeScale3D();
	} else {
		USceneComponent* AttachComponent = PawnAttachComponent ? PawnAttachComponent : GetPawn()->GetDefaultAttachComponent();
		CHECK_RETURN(AttachComponent);

		InteractiveTarget->SetAttachedToComponent(AttachComponent);
		InteractiveTarget->GetOwner()->AttachToComponent(AttachComponent,
														 FAttachmentTransformRules(InteractiveTarget->PawnLocationAttachmentRule,
																				   InteractiveTarget->PawnRotationAttachmentRule,
																				   InteractiveTarget->PawnScaleAttachmentRule,
																				   InteractiveTarget->PawnWeldSimulatedBodies),
														 PawnAttachSocketName == InteractiveTarget->PawnAttachSocketName && InteractiveTarget->PawnAttachSocketName != NAME_None
															 ? PawnAttachSocketName
															 : InteractiveTarget->PawnAttachSocketName);

		GrabbedObjectLocation = InteractiveTarget->GetOwner()->GetActorLocation();
		GrabbedObjectRotation = InteractiveTarget->GetOwner()->GetActorRotation();
		GrabbedObjectScale = InteractiveTarget->GetOwner()->GetActorRelativeScale3D();
	}
	CHECK_RETURN(InteractiveTarget)
	if (InteractiveTarget->bUseFingers) {
		RightHandFingerSettings = InteractiveTarget->DefaultHandFingerSettings;
	}
}

void UInteractiveControlComponent::AttachTarget(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);

	if (ControlMode == EInteractionControlMode::MotionController) {
		const auto MotionController = GetMotionController();

		UMotionControllerComponent* MotionControllerComponent = Cast<UMotionControllerComponent>(MotionController->GetComponentByClass(UMotionControllerComponent::StaticClass()));
		CHECK_RETURN(MotionControllerComponent);
		InteractiveTarget->ChangeOwnerScale(true, true);
		AttachTargetToMotionController(InteractiveTarget, MotionControllerComponent);
	} else {
		InteractiveTarget->ChangeOwnerScale(true, false);
		AttachTargetToPawn(InteractiveTarget);
	}

	InteractiveTarget->AttachToControl(this);

	UseTarget = InteractiveTarget;
}

void UInteractiveControlComponent::DetachTargetFromMotionController(UInteractiveTargetComponent* InteractiveTarget) {
	const auto DetachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);

	const auto MotionController = GetMotionController();
	UMotionControllerComponent* MotionControllerComponent = Cast<UMotionControllerComponent>(MotionController->GetComponentByClass(UMotionControllerComponent::StaticClass()));
	if (InteractiveTarget->bUsePhysicsHandles && PhysicsHandle && PhysicsHandle->GrabbedComponent != nullptr) {
		PhysicsHandle->ReleaseComponent();
	} else {
		if (MotionControllerComponent == nullptr) {
			UE_LOG(LogInteraction, Warning, TEXT("Motion controller component is nullptr but ControlMode == MotionController."));
			InteractiveTarget->SetAttachedToComponent(nullptr);
			InteractiveTarget->GetOwner()->DetachFromActor(DetachRules);
			return;
		}

		InteractiveTarget->SetAttachedToComponent(nullptr);
		InteractiveTarget->GetOwner()->DetachAllSceneComponents(MotionControllerComponent, DetachRules);
	}

	InteractiveTarget->ResetPhysicsComponent(true, InteractiveTarget->bResetVelocityOnDetach);
}

void UInteractiveControlComponent::DetachTargetFromPawn(UInteractiveTargetComponent* InteractiveTarget) const {
	const auto DetachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);

	if (PhysicsHandle && PhysicsHandle->GrabbedComponent != nullptr) {
		PhysicsHandle->ReleaseComponent();
	} else {
		if (PawnAttachComponent != nullptr) {
			InteractiveTarget->SetAttachedToComponent(nullptr);
			InteractiveTarget->GetOwner()->DetachAllSceneComponents(PawnAttachComponent, DetachRules);
		} else {
			InteractiveTarget->SetAttachedToComponent(nullptr);
			InteractiveTarget->GetOwner()->DetachFromActor(DetachRules);
		}
	}

	InteractiveTarget->ResetPhysicsComponent(InteractiveTarget->bUpdatePhysicsState, InteractiveTarget->bResetVelocityOnDetach);
}

void UInteractiveControlComponent::DetachTarget(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);

	UseTarget = nullptr;

	if (ControlMode == EInteractionControlMode::MotionController) {
		InteractiveTarget->ChangeOwnerScale(false, true);
		DetachTargetFromMotionController(InteractiveTarget);
	} else {
		InteractiveTarget->ChangeOwnerScale(false, false);
		DetachTargetFromPawn(InteractiveTarget);
	}

	InteractiveTarget->DetachFromControl(this);
}

void UInteractiveControlComponent::OnTargetDestroyed(UInteractiveTargetComponent* InteractiveTargetComponent) {
	if (InteractiveTargetComponent) {
		if (FocusedTarget == InteractiveTargetComponent) {
			FocusedTarget = nullptr;
		}

		if (OverlappedTarget == InteractiveTargetComponent) {
			OverlappedTarget = nullptr;
		}

		if (ActivateTarget == InteractiveTargetComponent) {
			ActivateTarget = nullptr;
		}

		if (GrabTarget == InteractiveTargetComponent) {
			GrabTarget = nullptr;
		}

		if (UseTarget == InteractiveTargetComponent) {
			UseTarget = nullptr;
		}
	}

	OnGrabEnd.Broadcast(this, InteractiveTargetComponent);
	Client_OnTargetDestroyed(InteractiveTargetComponent);
}

void UInteractiveControlComponent::Client_OnTargetDestroyed_Implementation(UInteractiveTargetComponent* InteractiveTargetComponent) {
	if (InteractiveTargetComponent) {
		if (FocusedTarget == InteractiveTargetComponent) {
			FocusedTarget = nullptr;
		}

		if (OverlappedTarget == InteractiveTargetComponent) {
			OverlappedTarget = nullptr;
		}

		if (ActivateTarget == InteractiveTargetComponent) {
			ActivateTarget = nullptr;
		}

		if (GrabTarget == InteractiveTargetComponent) {
			GrabTarget = nullptr;
		}

		if (UseTarget == InteractiveTargetComponent) {
			UseTarget = nullptr;
		}
	}

	OnGrabEnd.Broadcast(this, InteractiveTargetComponent);
}

bool UInteractiveControlComponent::IsDedicatedServer() const {
	const auto World = GetWorld();
	CHECK_RETURN_VALUE(World, false);

	const auto GameInstance = World->GetGameInstance();
	CHECK_RETURN_VALUE(GameInstance, false);

	return GameInstance->IsDedicatedServerInstance();
}

void UInteractiveControlComponent::Standalone_AttachLocal(UInteractiveTargetComponent* InteractiveTarget) {
	if (InteractiveTarget == nullptr) {
		InteractiveTarget = SelectActivateTarget();
	}
	CHECK_RETURN(InteractiveTarget);

	AttachTarget(InteractiveTarget);

	InteractiveTarget->Standalone_Attach(this);
}

void UInteractiveControlComponent::Server_AttachLocal(UInteractiveTargetComponent* InteractiveTarget) {
	if (InteractiveTarget == nullptr) {
		InteractiveTarget = SelectActivateTarget();
	}
	CHECK_RETURN(InteractiveTarget);

	AttachTarget(InteractiveTarget);

	InteractiveTarget->Server_Attach(this);

	if (InteractiveTarget->IsReplicating(EInteractionType::Grab)) {
		if (InteractiveTarget->IsMulticasting(EInteractionType::Grab)) {
			Multicast_AttachRemote(InteractiveTarget);
		} else {
			Client_AttachRemote(InteractiveTarget);
		}
	}
}

void UInteractiveControlComponent::Client_AttachRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);

	Client_AttachLocal(InteractiveTarget);
}

void UInteractiveControlComponent::Client_AttachLocal(UInteractiveTargetComponent* InteractiveTarget) {
	if (InteractiveTarget == nullptr) {
		InteractiveTarget = SelectActivateTarget();
	}
	CHECK_RETURN(InteractiveTarget);

	AttachTarget(InteractiveTarget);

	InteractiveTarget->Client_Attach(this);
}

void UInteractiveControlComponent::Multicast_AttachRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);

	// Do not notify server with multicast calls as it has own server-only call.
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	CHECK_RETURN(InteractiveTarget);

	Client_AttachLocal(InteractiveTarget);
}

void UInteractiveControlComponent::Standalone_DetachLocal(UInteractiveTargetComponent* InteractiveTarget) {
	if (InteractiveTarget == nullptr) {
		InteractiveTarget = SelectUseTarget();
	}
	CHECK_RETURN(InteractiveTarget);

	if (InteractiveTarget->AttachedTo && InteractiveTarget->AttachedTo->GetOwner() != GetOwner()) {
		return;
	}

	DetachTarget(InteractiveTarget);

	InteractiveTarget->Standalone_Detach(this);
}

void UInteractiveControlComponent::Server_DetachLocal(UInteractiveTargetComponent* InteractiveTarget) {
	if (InteractiveTarget == nullptr) {
		InteractiveTarget = SelectUseTarget();
	}
	CHECK_RETURN(InteractiveTarget);

	// Do not detach if we already do not own the object, e.g. it was reattached to other hand.
	if (InteractiveTarget->AttachedTo && InteractiveTarget->AttachedTo->GetOwner() != GetOwner()) {
		return;
	}

	DetachTarget(InteractiveTarget);

	InteractiveTarget->Server_Detach(this);

	if (InteractiveTarget->IsReplicating(EInteractionType::Grab)) {
		if (InteractiveTarget->IsMulticasting(EInteractionType::Grab)) {
			Multicast_DetachRemote(InteractiveTarget);
		} else {
			Client_DetachRemote(InteractiveTarget);
		}
	}
}

void UInteractiveControlComponent::Client_DetachRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);

	DetachTarget(InteractiveTarget);
	Client_DetachLocal(InteractiveTarget);
}

void UInteractiveControlComponent::Client_DetachLocal(UInteractiveTargetComponent* InteractiveTarget) {
	if (InteractiveTarget == nullptr) {
		InteractiveTarget = SelectUseTarget();
	}
	CHECK_RETURN(InteractiveTarget);

	if (InteractiveTarget->AttachedTo && InteractiveTarget->AttachedTo->GetOwner() != GetOwner()) {
		return;
	}

	DetachTarget(InteractiveTarget);

	InteractiveTarget->Client_Detach(this);
}

void UInteractiveControlComponent::Multicast_DetachRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);

	// Do not notify server with multicast calls as it has own server-only call.
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	CHECK_RETURN(InteractiveTarget);

	DetachTarget(InteractiveTarget);
	Client_DetachLocal(InteractiveTarget);
}
#pragma endregion

#pragma region Control Owner Helpers
AActor* UInteractiveControlComponent::GetMotionController() const {
	if (ControlMode != EInteractionControlMode::MotionController) {
		return nullptr;
	}

	return GetOwner();
}

APawn* UInteractiveControlComponent::GetPawn() const {
	if (ControlMode == EInteractionControlMode::MotionController) {
		const auto OwningController = GetOwner();
		if (OwningController == nullptr) {
			return nullptr;
		}
		return Cast<APawn>(OwningController->GetOwner());
	}
	return Cast<APawn>(GetOwner());
}

APlayerController* UInteractiveControlComponent::GetPlayerController() const {
	const auto Pawn = GetPawn();

	if (Pawn) {
		return Cast<APlayerController>(Pawn->GetController());
	}

	return nullptr;
}

#pragma endregion

#pragma region Overlap Bindings
void UInteractiveControlComponent::OnOverlapBegin(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
												  UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/,
												  bool /*bFromSweep*/, const FHitResult& /*SweepResult*/) {
	const auto Target = OtherActor->FindComponentByClass<UInteractiveTargetComponent>();
	if (Target == nullptr) {
		return;
	}

	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	const bool bIsServer = ACTOR_IS_AUTHORITY(Owner);
	const bool bIsStandalone = ACTOR_IS_STANDALONE(Owner);

	if (bUseOverlap && (bIsStandalone || bIsServer || bProcessClientOverlap)) {
		// Check if we have a new overlapped target.
		if (Target) {
			if (bIsStandalone) {
				Standalone_OverlapBeginLocal(Target, OverlapOrigin, OverlapLocation, OverlapNormal);
			} else if (Target->IsClientOnly(EInteractionType::Overlap) && !bIsServer) {
				Client_OverlapBeginLocal(Target, OverlapOrigin, OverlapLocation, OverlapNormal);
			} else if (!Target->IsClientOnly(EInteractionType::Overlap) && bIsServer) {
				Server_OverlapBeginLocal(Target, OverlapOrigin, OverlapLocation, OverlapNormal);
			}
		}

		OverlappingTargets.Add(Target);
	}
}

void UInteractiveControlComponent::OnOverlapEnd(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
												UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/) {
	const auto Target = OtherActor->FindComponentByClass<UInteractiveTargetComponent>();
	if (Target == nullptr) {
		return;
	}
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	const bool bIsServer = ACTOR_IS_AUTHORITY(Owner);
	const bool bIsStandalone = ACTOR_IS_STANDALONE(Owner);

	if (bUseOverlap && (bIsStandalone || bIsServer || bProcessClientOverlap)) {
		if (Target) {
			if (bIsStandalone) {
				Standalone_OverlapEndLocal(Target);
			} else if (Target->IsClientOnly(EInteractionType::Overlap) && !bIsServer) {
				Client_OverlapEndLocal(Target);
			} else if (!Target->IsClientOnly(EInteractionType::Overlap) && bIsServer) {
				Server_OverlapEndLocal(Target);
			}
		}
	}

	OverlappingTargets.Remove(Target);
}
#pragma endregion

#pragma region InputClientBindings
void UInteractiveControlComponent::OnInputBegin(const EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl) {
	if (ProxyControl) {
		if (ProxyControl->ControlMode == EInteractionControlMode::MotionController) {
			OnMotionControllerInputBegin(Action, ProxyControl);
		} else if (ProxyControl->ControlMode == EInteractionControlMode::PawnMobile) {
			OnMobileInputBegin(Action, ProxyControl);
		} else {
			OnPawnInputBegin(Action, ProxyControl);
		}
	} else {
		if (ControlMode == EInteractionControlMode::MotionController) {
			OnMotionControllerInputBegin(Action);
		} else if (ControlMode == EInteractionControlMode::PawnMobile) {
			OnMobileInputBegin(Action);
		} else {
			OnPawnInputBegin(Action);
		}
	}
}

void UInteractiveControlComponent::OnPawnInputBegin(const EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl) {
	const auto Owner = GetPawn();
	CHECK_RETURN(Owner);

	const auto TargetControl = ProxyControl ? ProxyControl : this;
	const auto SelectedActivateTarget = TargetControl->SelectActivateTarget();
	const auto SelectedGrabTarget = TargetControl->SelectGrabTarget();
	const auto SelectedUseTarget = TargetControl->SelectUseTarget();

	uint8 UseActionIndex;

	const bool bIsStandalone = ACTOR_IS_STANDALONE(Owner);

	switch (Action) {
	case EInteractionInputActionType::ActionActivate:
		// Do not call activate event at the use target.
		if (SelectedActivateTarget != nullptr && SelectedActivateTarget != SelectedUseTarget) {
			if (bIsStandalone) {
				TargetControl->Standalone_ActivateBeginLocal(SelectedActivateTarget, ActivateOrigin, ActivateLocation, ActivateNormal);
			} else if (SelectedActivateTarget->IsReplicating(EInteractionType::Activate)) {
				Server_ActivateBeginRemote(ProxyControl);
			} else {
				TargetControl->Client_ActivateBeginLocal(SelectedActivateTarget, ActivateOrigin, ActivateLocation, ActivateNormal);
			}
		}
		return;
	case EInteractionInputActionType::ActionGrab:
		// Do not allow the pawn to grab a new target if it already has another target attached.
		if (SelectedGrabTarget != nullptr && SelectedUseTarget == nullptr) {
			if (bIsStandalone) {
				TargetControl->Standalone_GrabBeginLocal(SelectedGrabTarget, GrabOrigin, GrabLocation, GrabNormal);
			} else if (SelectedGrabTarget->IsReplicating(EInteractionType::Grab)) {
				Server_GrabBeginRemote(ProxyControl);
			} else {
				TargetControl->Client_GrabBeginLocal(SelectedGrabTarget, GrabOrigin, GrabLocation, GrabNormal);
			}
		}
		return;
	case EInteractionInputActionType::ActionUse:
		UseActionIndex = 0;
		break;
	case EInteractionInputActionType::ActionUse1:
		UseActionIndex = 1;
		break;
	case EInteractionInputActionType::ActionUse2:
		UseActionIndex = 2;
		break;
	case EInteractionInputActionType::ActionUse3:
		UseActionIndex = 3;
		break;
	default:
		return;
	}

	if (SelectedUseTarget != nullptr) {
		if (bIsStandalone) {
			TargetControl->Standalone_UseBeginLocal(SelectedUseTarget, UseActionIndex);
		} else if (SelectedUseTarget->IsReplicating(EInteractionType::Use)) {
			Server_UseBeginRemote(ProxyControl, UseActionIndex);
		} else {
			TargetControl->Client_UseBeginLocal(SelectedUseTarget, UseActionIndex);
		}
	}
}

void UInteractiveControlComponent::OnMotionControllerInputBegin(EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl) {
	const auto TargetControl = ProxyControl ? ProxyControl : this;
	const auto SelectedActivateTarget = TargetControl->SelectActivateTarget();
	const auto SelectedGrabTarget = TargetControl->SelectGrabTarget();
	const auto SelectedUseTarget = TargetControl->SelectUseTarget();

	uint8 UseActionIndex;

	const auto Owner = GetPawn();
	CHECK_RETURN(Owner);

	const bool bIsStandalone = ACTOR_IS_STANDALONE(Owner);

	switch (Action) {
	case EInteractionInputActionType::ActionActivate:
		// Do not allow the motion controller to activate a new target if it already has another target attached.
		if (SelectedActivateTarget != nullptr && SelectedUseTarget == nullptr) {
			if (bIsStandalone) {
				TargetControl->Standalone_ActivateBeginLocal(SelectedActivateTarget, ActivateOrigin, ActivateLocation, ActivateNormal);
			} else if (SelectedActivateTarget->IsReplicating(EInteractionType::Activate)) {
				Server_ActivateBeginRemote(ProxyControl);
			} else {
				TargetControl->Client_ActivateBeginLocal(SelectedActivateTarget, ActivateOrigin, ActivateLocation, ActivateNormal);
			}
		}
		return;
	case EInteractionInputActionType::ActionGrab:
		// Do not allow the motion controller to grab a new target if it already has another target attached.
		if (SelectedGrabTarget != nullptr && SelectedUseTarget == nullptr) {
			if (bIsStandalone) {
				TargetControl->Standalone_GrabBeginLocal(SelectedGrabTarget, GrabOrigin, GrabLocation, GrabNormal);
			} else if (SelectedGrabTarget->IsReplicating(EInteractionType::Grab)) {
				Server_GrabBeginRemote(ProxyControl);
			} else {
				TargetControl->Client_GrabBeginLocal(SelectedGrabTarget, GrabOrigin, GrabLocation, GrabNormal);
			}
		}
		return;
	case EInteractionInputActionType::ActionUse:
		UseActionIndex = 0;
		break;
	case EInteractionInputActionType::ActionUse1:
		UseActionIndex = 1;
		break;
	case EInteractionInputActionType::ActionUse2:
		UseActionIndex = 2;
		break;
	case EInteractionInputActionType::ActionUse3:
		UseActionIndex = 3;
		break;
	default:
		return;
	}

	if (SelectedUseTarget != nullptr) {
		if (bIsStandalone) {
			TargetControl->Standalone_UseBeginLocal(SelectedUseTarget, UseActionIndex);
		} else if (SelectedUseTarget->IsReplicating(EInteractionType::Use)) {
			Server_UseBeginRemote(ProxyControl, UseActionIndex);
		} else {
			TargetControl->Client_UseBeginLocal(SelectedUseTarget, UseActionIndex);
		}
	}
}

void UInteractiveControlComponent::OnMobileInputBegin(const EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl) {
	OnPawnInputBegin(Action, ProxyControl);
}

void UInteractiveControlComponent::OnUserInputBegin(const FName InInput, const FKey InKey, UInteractiveControlComponent* InProxyControl) {
	if (InProxyControl) {
		if (InProxyControl->ControlMode == EInteractionControlMode::MotionController) {
			OnMotionControllerUserInputBegin(InInput, InKey, InProxyControl);
		} else if (InProxyControl->ControlMode == EInteractionControlMode::PawnMobile) {
			OnMobileUserInputBegin(InInput, InKey, InProxyControl);
		} else {
			OnPawnUserInputBegin(InInput, InKey, InProxyControl);
		}
	} else {
		if (ControlMode == EInteractionControlMode::MotionController) {
			OnMotionControllerUserInputBegin(InInput, InKey);
		} else if (ControlMode == EInteractionControlMode::PawnMobile) {
			OnMobileUserInputBegin(InInput, InKey);
		} else {
			OnPawnUserInputBegin(InInput, InKey);
		}
	}
}

void UInteractiveControlComponent::OnMotionControllerUserInputBegin(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl) {
	const auto TargetControl = InProxyControl ? InProxyControl : this;
	const auto SelectedActivateTarget = TargetControl->SelectActivateTarget();

	const auto Owner = GetPawn();
	CHECK_RETURN(Owner);

	const bool bIsStandalone = ACTOR_IS_STANDALONE(Owner);

	if (IsValid(SelectedActivateTarget)) {
		if (bIsStandalone) {
			TargetControl->Standalone_UserInputBeginLocal(SelectedActivateTarget, InName, InKey, ActivateOrigin, ActivateLocation, ActivateNormal);
		} else if (SelectedActivateTarget->IsReplicating(EInteractionType::User)) {
			Server_UserInputBeginRemote(InName, InKey, InProxyControl);
		} else {
			TargetControl->Client_UserInputBeginLocal(SelectedActivateTarget, InName, InKey, ActivateOrigin, ActivateLocation, ActivateNormal);
		}
	}
}

void UInteractiveControlComponent::OnMobileUserInputBegin(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl) {
	OnPawnUserInputBegin(InName, InKey, InProxyControl);
}


void UInteractiveControlComponent::OnPawnUserInputBegin(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl) {
	const auto Owner = GetPawn();
	CHECK_RETURN(Owner);

	const auto TargetControl = InProxyControl ? InProxyControl : this;
	const auto SelectedActivateTarget = TargetControl->SelectActivateTarget();
	const bool bIsStandalone = ACTOR_IS_STANDALONE(Owner);

	if (IsValid(SelectedActivateTarget)) {
		if (bIsStandalone) {
			TargetControl->Standalone_UserInputBeginLocal(SelectedActivateTarget, InName, InKey, ActivateOrigin, ActivateLocation, ActivateNormal);
		} else if (SelectedActivateTarget->IsReplicating(EInteractionType::User)) {
			Server_UserInputBeginRemote(InName, InKey, InProxyControl);
		} else {
			TargetControl->Client_UserInputBeginLocal(SelectedActivateTarget, InName, InKey, ActivateOrigin, ActivateLocation, ActivateNormal);
		}
	}
}

void UInteractiveControlComponent::Standalone_UserInputBeginLocal(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey, const FVector& InOrigin,
																  const FVector& InLocation, const FVector& InNormal) {
	CHECK_RETURN(InProxyControl);

	OnUserInputBeginDelegate.Broadcast(this, InProxyControl);

	InProxyControl->Standalone_UserInputBegin(this, InName, InKey, InOrigin, InLocation, InNormal);

}

void UInteractiveControlComponent::Server_UserInputBeginRemote_Implementation(const FName& Name, const FKey& Key, UInteractiveControlComponent* ProxyControl) {

	if (!IsValid(ProxyControl)) {
		Server_UserInputBeginLocal(Name, Key);
		return;
	}

	ProxyControl->Server_UserInputBeginLocal(Name, Key);
}

void UInteractiveControlComponent::Multicast_UserInputBeginRemote_Implementation(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey,
																  const FVector_NetQuantize& InOrigin, const FVector_NetQuantize& InLocation,
																  const FVector_NetQuantizeNormal& InNormal) {
	CHECK_RETURN(InProxyControl);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_UserInputBeginLocal(InProxyControl, InName, InKey, InOrigin, InLocation, InNormal);
}

void UInteractiveControlComponent::Client_UserInputBeginRemote_Implementation(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey,
															   const FVector_NetQuantize& InOrigin, const FVector_NetQuantize& InLocation,
															   const FVector_NetQuantizeNormal& InNormal) {
	CHECK_RETURN(InProxyControl);
	Client_UserInputBeginLocal(InProxyControl, InName, InKey, InOrigin, InLocation, InNormal);
}

void UInteractiveControlComponent::Server_UserInputBeginLocal(const FName& InName, const FKey& InKey) {
	const auto InteractiveTarget = SelectActivateTarget();
	CHECK_RETURN(InteractiveTarget);

	OnUserInputBeginDelegate.Broadcast(this, InteractiveTarget);

	// Local notify the interactive target at the server.
	InteractiveTarget->Server_UserInputBegin(this, InName, InKey, ActivateOrigin, ActivateLocation, ActivateNormal);

	if (InteractiveTarget->IsReplicating(EInteractionType::User)) {
		if (InteractiveTarget->IsMulticasting(EInteractionType::User)) {
			Multicast_UserInputBeginRemote(InteractiveTarget, InName, InKey, ActivateOrigin, ActivateLocation, ActivateNormal);
		} else {
			Client_UserInputBeginRemote(InteractiveTarget, InName, InKey, ActivateOrigin, ActivateLocation, ActivateNormal);
		}
	}
}

void UInteractiveControlComponent::Client_UserInputBeginLocal(UInteractiveTargetComponent* InteractiveTarget, const FName& Name, const FKey& Key,
															  const FVector_NetQuantize InOrigin,
															  const FVector_NetQuantize InLocation, const FVector_NetQuantizeNormal InNormal) {
	CHECK_RETURN(InteractiveTarget);

	OnUserInputBeginDelegate.Broadcast(this, InteractiveTarget);

	InteractiveTarget->Client_UserInputBegin(this, Name, Key, InOrigin, InLocation, InNormal);
}

void UInteractiveControlComponent::OnUserInputEnd(const FName InInput, const FKey InKey, UInteractiveControlComponent* ProxyControl) {
	if (ProxyControl) {
		if (ProxyControl->ControlMode == EInteractionControlMode::MotionController) {
			OnMotionControllerUserInputEnd(InInput, InKey, ProxyControl);
		} else if (ProxyControl->ControlMode == EInteractionControlMode::PawnMobile) {
			OnMobileUserInputEnd(InInput, InKey, ProxyControl);
		} else {
			OnPawnUserInputEnd(InInput, InKey, ProxyControl);
		}
	} else {
		if (ControlMode == EInteractionControlMode::MotionController) {
			OnMotionControllerUserInputEnd(InInput, InKey);
		} else if (ControlMode == EInteractionControlMode::PawnMobile) {
			OnMobileUserInputEnd(InInput, InKey);
		} else {
			OnPawnUserInputEnd(InInput, InKey);
		}
	}
}

void UInteractiveControlComponent::OnMotionControllerUserInputEnd(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl) {
	const auto Owner = GetPawn();
	CHECK_RETURN(Owner);

	const auto TargetControl = InProxyControl ? InProxyControl : this;
	const auto SelectedActivateTarget = TargetControl->SelectActivateTarget();

	const bool bIsStandalone = ACTOR_IS_STANDALONE(Owner);

	if (IsValid(SelectedActivateTarget)) {
		if (bIsStandalone) {
			TargetControl->Standalone_UserInputEndLocal(SelectedActivateTarget, InName, InKey);
		} else if (SelectedActivateTarget->IsReplicating(EInteractionType::User)) {
			Server_UserInputEndRemote(InName, InKey, InProxyControl);
		} else {
			TargetControl->Client_UserInputEndLocal(SelectedActivateTarget, InName, InKey);
		}
	}
}

void UInteractiveControlComponent::OnMobileUserInputEnd(const FName& Name, const FKey& Key, UInteractiveControlComponent* ProxyControl) {
	OnPawnUserInputEnd(Name, Key, ProxyControl);
}

void UInteractiveControlComponent::Standalone_UserInputEndLocal(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey) {
	CHECK_RETURN(InProxyControl);

	OnUserInputEndDelegate.Broadcast(this, InProxyControl);

	InProxyControl->Standalone_UserInputEnd(this, InName, InKey);
	ActivateTarget = nullptr;
}

void UInteractiveControlComponent::Client_UserInputEndLocal(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey) {
	if (InProxyControl == nullptr) {
		CHECK_RETURN(ActivateTarget);
		if (ActivateTarget->IsClientOnly(EInteractionType::User)) {
			ActivateTarget->Client_UserInputEnd(this, InName, InKey);
		}
	} else {
		InProxyControl->Client_UserInputEnd(this, InName, InKey);
	}

	OnUserInputEndDelegate.Broadcast(this, InProxyControl);

	ActivateTarget = nullptr;
}

void UInteractiveControlComponent::Multicast_UserInputEndRemote_Implementation(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey) {
	CHECK_RETURN(InProxyControl);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_UserInputEndLocal(InProxyControl, InName, InKey);
}

void UInteractiveControlComponent::Client_UserInputEndRemote_Implementation(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey) {
	CHECK_RETURN(InProxyControl);
	Client_UserInputEndLocal(InProxyControl, InName, InKey);
}

void UInteractiveControlComponent::Server_UserInputEndLocal(const FName& InName, const FKey& InKey) {
	CHECK_RETURN(ActivateTarget);

	OnUserInputEndDelegate.Broadcast(this, ActivateTarget);

	ActivateTarget->Server_UserInputEnd(this, InName, InKey);
	if (ActivateTarget->IsReplicating(EInteractionType::User)) {
		if (ActivateTarget->IsMulticasting(EInteractionType::User)) {
			Multicast_UserInputEndRemote(ActivateTarget, InName, InKey);
		} else {
			Client_UserInputEndRemote(ActivateTarget, InName, InKey);
		}
	}
	ActivateTarget = nullptr;
}

void UInteractiveControlComponent::Server_UserInputEndRemote_Implementation(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl) {
	if (InProxyControl == nullptr) {
		Server_UserInputEndLocal(InName, InKey);
		return;
	}
	InProxyControl->Server_UserInputEndLocal(InName, InKey);
}

void UInteractiveControlComponent::OnPawnUserInputEnd(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl) {
	const auto Owner = GetPawn();
	CHECK_RETURN(Owner);

	const auto TargetControl = InProxyControl ? InProxyControl : this;
	const auto SelectedActivateTarget = TargetControl->SelectActivateTarget();

	const bool bIsStandalone = ACTOR_IS_STANDALONE(Owner);

	if (IsValid(SelectedActivateTarget)) {
		if (bIsStandalone) {
			TargetControl->Standalone_UserInputEndLocal(SelectedActivateTarget, InName, InKey);
		} else if (SelectedActivateTarget->IsReplicating(EInteractionType::User)) {
			Server_UserInputEndRemote(InName, InKey, InProxyControl);
		} else {
			TargetControl->Client_UserInputEndLocal(SelectedActivateTarget, InName, InKey);
		}
	}
}

void UInteractiveControlComponent::OnInputEnd(const EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl) {
	if (ProxyControl) {
		if (ProxyControl->ControlMode == EInteractionControlMode::MotionController) {
			OnMotionControllerInputEnd(Action, ProxyControl);
		} else if (ProxyControl->ControlMode == EInteractionControlMode::PawnMobile) {
			OnMobileInputEnd(Action, ProxyControl);
		} else {
			OnPawnInputEnd(Action, ProxyControl);
		}
	} else {
		if (ControlMode == EInteractionControlMode::MotionController) {
			OnMotionControllerInputEnd(Action);
		} else if (ControlMode == EInteractionControlMode::PawnMobile) {
			OnMobileInputEnd(Action);
		} else {
			OnPawnInputEnd(Action);
		}
	}
}

void UInteractiveControlComponent::OnPawnInputEnd(const EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl) {
	const auto TargetControl = ProxyControl ? ProxyControl : this;
	const auto MyActivateTarget = TargetControl->SelectActivateTarget();
	const auto MyUseTarget = TargetControl->SelectUseTarget();

	uint8 UseActionIndex;

	const auto Owner = GetPawn();
	CHECK_RETURN(Owner);

	const bool bIsStandalone = ACTOR_IS_STANDALONE(Owner);

	switch (Action) {
	case EInteractionInputActionType::ActionActivate:
		if (MyActivateTarget != nullptr && MyActivateTarget != MyUseTarget) {
			if (bIsStandalone) {
				TargetControl->Standalone_ActivateEndLocal(MyActivateTarget);
			} else if (MyActivateTarget->IsReplicating(EInteractionType::Activate)) {
				Server_ActivateEndRemote(ProxyControl);
			} else {
				TargetControl->Client_ActivateEndLocal(MyActivateTarget);
			}
		}
		return;
	case EInteractionInputActionType::ActionGrab:
		if (MyActivateTarget != nullptr) {
			if (bIsStandalone) {
				TargetControl->Standalone_GrabEndLocal(MyActivateTarget);
			} else if (MyActivateTarget->IsReplicating(EInteractionType::Grab)) {
				Server_GrabEndRemote(ProxyControl);
			} else {
				TargetControl->Client_GrabEndLocal(MyActivateTarget);
			}
		} else if (MyUseTarget != nullptr && bEnableUseTargetGrabEndEvents) {
			if (bIsStandalone) {
				TargetControl->Standalone_GrabEndLocal(MyUseTarget);
			} else if (MyUseTarget->IsReplicating(EInteractionType::Grab)) {
				Server_GrabEndRemote(ProxyControl);
			} else {
				TargetControl->Client_GrabEndLocal(MyUseTarget);
			}
		}
		return;
	case EInteractionInputActionType::ActionUse:
		UseActionIndex = 0;
		break;
	case EInteractionInputActionType::ActionUse1:
		UseActionIndex = 1;
		break;
	case EInteractionInputActionType::ActionUse2:
		UseActionIndex = 2;
		break;
	case EInteractionInputActionType::ActionUse3:
		UseActionIndex = 3;
		break;
	default:
		return;
	}

	if (UseActionIndex < 0xFF && MyUseTarget != nullptr) {
		if (bIsStandalone) {
			TargetControl->Standalone_UseEndLocal(MyActivateTarget);
		} else if (MyUseTarget->IsReplicating(EInteractionType::Use)) {
			Server_UseEndRemote(ProxyControl, UseActionIndex);
		} else {
			TargetControl->Client_UseEndLocal(MyUseTarget, UseActionIndex);
		}
	}
}

void UInteractiveControlComponent::OnMotionControllerInputEnd(const EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl) {
	const auto TargetControl = ProxyControl ? ProxyControl : this;
	const auto MyActivateTarget = TargetControl->SelectActivateTarget();
	const auto MyUseTarget = TargetControl->SelectUseTarget();

	uint8 UseActionIndex;

	const auto Owner = GetPawn();
	CHECK_RETURN(Owner);

	const bool bIsStandalone = ACTOR_IS_STANDALONE(Owner);

	switch (Action) {
	case EInteractionInputActionType::ActionActivate:
		if (MyActivateTarget != nullptr && MyActivateTarget != MyUseTarget) {
			if (bIsStandalone) {
				TargetControl->Standalone_ActivateEndLocal(MyActivateTarget);
			} else if (MyActivateTarget->IsReplicating(EInteractionType::Activate)) {
				Server_ActivateEndRemote(ProxyControl);
			} else {
				TargetControl->Client_ActivateEndLocal(MyActivateTarget);
			}
		}
		return;
	case EInteractionInputActionType::ActionGrab:
		if (MyActivateTarget != nullptr) {
			if (bIsStandalone) {
				TargetControl->Standalone_GrabEndLocal(MyActivateTarget);
			} else if (MyActivateTarget->IsReplicating(EInteractionType::Grab)) {
				Server_GrabEndRemote(ProxyControl);
			} else {
				TargetControl->Client_GrabEndLocal(MyActivateTarget);
			}
		} else if (MyUseTarget != nullptr && bEnableUseTargetGrabEndEvents) {
			if (bIsStandalone) {
				TargetControl->Standalone_GrabEndLocal(MyUseTarget);
			} else if (MyUseTarget->IsReplicating(EInteractionType::Grab)) {
				Server_GrabEndRemote(ProxyControl);
			} else {
				TargetControl->Client_GrabEndLocal(MyUseTarget);
			}
		}
		return;
	case EInteractionInputActionType::ActionUse:
		UseActionIndex = 0;
		break;
	case EInteractionInputActionType::ActionUse1:
		UseActionIndex = 1;
		break;
	case EInteractionInputActionType::ActionUse2:
		UseActionIndex = 2;
		break;
	case EInteractionInputActionType::ActionUse3:
		UseActionIndex = 3;
		break;
	default:
		return;
	}

	if (UseActionIndex < 0xFF && MyUseTarget != nullptr) {
		if (bIsStandalone) {
			TargetControl->Standalone_UseEndLocal(MyUseTarget, UseActionIndex);
		} else if (MyUseTarget->IsReplicating(EInteractionType::Use)) {
			Server_UseEndRemote(ProxyControl, UseActionIndex);
		} else {
			TargetControl->Client_UseEndLocal(MyUseTarget, UseActionIndex);
		}
	}
}

void UInteractiveControlComponent::OnMobileInputEnd(const EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl) {
	OnPawnInputEnd(Action, ProxyControl);
}

void UInteractiveControlComponent::OnCustomInputEnd(FKey InKey, UInteractiveControlComponent* ProxyControl) {
	// todo: implement
}

void UInteractiveControlComponent::OnInputAxis(const EInteractionInputAxisType Axis, const float Value, UInteractiveControlComponent* ProxyControl) {
	switch (Axis) {
	case EInteractionInputAxisType::AxisX:
		if (FMath::IsNearlyEqual(AxisZeroValue, Value)) {
			return;
		}
		break;
	case EInteractionInputAxisType::AxisY:
		if (FMath::IsNearlyEqual(AxisZeroValue, Value)) {
			return;
		}
		break;
	case EInteractionInputAxisType::AxisZ:
		if (FMath::IsNearlyEqual(AxisZeroValue, Value)) {
			return;
		}
		break;
	default:
		return;
	}

	if (ProxyControl) {
		if (ProxyControl->ControlMode == EInteractionControlMode::MotionController) {
			OnMotionControllerInputAxis(Axis, Value, ProxyControl);
		} else if (ProxyControl->ControlMode == EInteractionControlMode::PawnMobile) {
			OnMobileInputAxis(Axis, Value, ProxyControl);
		} else {
			OnPawnInputAxis(Axis, Value, ProxyControl);
		}
	} else {
		if (ControlMode == EInteractionControlMode::MotionController) {
			OnMotionControllerInputAxis(Axis, Value);
		} else if (ControlMode == EInteractionControlMode::PawnMobile) {
			OnMobileInputAxis(Axis, Value);
		} else {
			OnPawnInputAxis(Axis, Value);
		}
	}
}

void UInteractiveControlComponent::OnPawnInputAxis(const EInteractionInputAxisType Axis, float Value, UInteractiveControlComponent* ProxyControl) {
	UInteractiveTargetComponent* MyUseTarget = ProxyControl ? ProxyControl->SelectUseTarget() : SelectUseTarget();

	if (MyUseTarget != nullptr) {
		if (MyUseTarget->IsReplicating(EInteractionType::Use)) {
			Server_UseAxisRemote(Axis, Value, ProxyControl);
		} else {
			Client_UseAxisLocal(MyUseTarget, Axis, Value);
		}
		return;
	}

	UInteractiveTargetComponent* MyActivateTarget = ProxyControl ? ProxyControl->SelectActivateTarget() : SelectActivateTarget();
	if (MyActivateTarget != nullptr) {
		if (MyActivateTarget->IsReplicating(EInteractionType::Activate)) {
			Server_ActivateAxisRemote(Axis, Value, ProxyControl);
		} else {
			Client_ActivateAxisLocal(MyActivateTarget, Axis, Value);
		}
	}
}

void UInteractiveControlComponent::OnMotionControllerInputAxis(const EInteractionInputAxisType Axis, const float Value, UInteractiveControlComponent* ProxyControl) {
	OnPawnInputAxis(Axis, Value, ProxyControl);
}

void UInteractiveControlComponent::OnMobileInputAxis(const EInteractionInputAxisType Axis, const float Value, UInteractiveControlComponent* ProxyControl) {
	OnPawnInputAxis(Axis, Value, ProxyControl);
}
#pragma endregion

#pragma region Custom RPC

void UInteractiveControlComponent::Standalone_CustomEventLocal(UInteractiveTargetComponent* InteractiveTarget, const FString& Topic, const TArray<uint8>& Parameters) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Standalone_CustomEvent(this, Topic, Parameters);
}

void UInteractiveControlComponent::Server_CustomEventLocal(UInteractiveTargetComponent* InteractiveTarget, const FString& Topic, const TArray<uint8>& Parameters) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Server_CustomEvent(this, Topic, Parameters);
	if (InteractiveTarget->IsReplicating(EInteractionType::Events)) {
		// Should we multicast to clients.
		if (InteractiveTarget->IsMulticasting(EInteractionType::Events)) {
			Multicast_CustomEventRemote(InteractiveTarget, Topic, Parameters);
		} else {
			Client_CustomEventRemote(InteractiveTarget, Topic, Parameters);
		}
	}
}

void UInteractiveControlComponent::Client_CustomEventLocal(UInteractiveTargetComponent* InteractiveTarget, const FString& Topic, const TArray<uint8>& Parameters) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Client_CustomEvent(this, Topic, Parameters);
}

void UInteractiveControlComponent::Server_CustomEventRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget, const FString& Topic, const TArray<uint8>& Parameters,
																		   UInteractiveControlComponent* ProxyControl) {
	// Check if we should process the event as ourselves, or leave it to the proxy component at the server side.
	if (ProxyControl == nullptr) {
		Server_CustomEventLocal(InteractiveTarget, Topic, Parameters);
		return;
	}
	ProxyControl->Server_CustomEventLocal(InteractiveTarget, Topic, Parameters);
}

void UInteractiveControlComponent::Client_CustomEventRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget, const FString& Topic, const TArray<uint8>& Parameters) {
	CHECK_RETURN(InteractiveTarget);
	Client_CustomEventLocal(InteractiveTarget, Topic, Parameters);
}

void UInteractiveControlComponent::Multicast_CustomEventRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget, const FString& Topic,
																			  const TArray<uint8>& Parameters) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_CustomEventLocal(InteractiveTarget, Topic, Parameters);
}

#pragma endregion

#pragma region FocusBegin

void UInteractiveControlComponent::Standalone_FocusBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
															  const FVector_NetQuantize InFocusOrigin,
															  const FVector_NetQuantize InFocusLocation,
															  const FVector_NetQuantizeNormal InFocusNormal) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Standalone_FocusBegin(this, InFocusOrigin, InFocusLocation, InFocusNormal);
}

void UInteractiveControlComponent::Server_FocusBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
														  const FVector_NetQuantize InFocusOrigin,
														  const FVector_NetQuantize InFocusLocation,
														  const FVector_NetQuantizeNormal InFocusNormal) {
	CHECK_RETURN(InteractiveTarget);

	// Local notify the interactive target at the server.
	InteractiveTarget->Server_FocusBegin(this, InFocusOrigin, InFocusLocation, InFocusNormal);

	// Should we replicate to clients.
	if (InteractiveTarget->IsReplicating(EInteractionType::Focus)) {
		// Should we multicast to clients.
		if (InteractiveTarget->IsMulticasting(EInteractionType::Focus)) {
			Multicast_FocusBeginRemote(InteractiveTarget, InFocusOrigin, InFocusLocation, InFocusNormal);
		} else {
			Client_FocusBeginRemote(InteractiveTarget, InFocusOrigin, InFocusLocation, InFocusNormal);
		}
	}
}

void UInteractiveControlComponent::Client_FocusBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
														  const FVector_NetQuantize InFocusOrigin,
														  const FVector_NetQuantize InFocusLocation,
														  const FVector_NetQuantizeNormal InFocusNormal) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Client_FocusBegin(this, InFocusOrigin, InFocusLocation, InFocusNormal);
}

void UInteractiveControlComponent::Client_FocusBeginRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																		  const FVector_NetQuantize InFocusOrigin,
																		  const FVector_NetQuantize InFocusLocation,
																		  const FVector_NetQuantizeNormal InFocusNormal) {
	CHECK_RETURN(InteractiveTarget);
	Client_FocusBeginLocal(InteractiveTarget, InFocusOrigin, InFocusLocation, InFocusNormal);
}

void UInteractiveControlComponent::Multicast_FocusBeginRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																			 const FVector_NetQuantize InFocusOrigin,
																			 const FVector_NetQuantize InFocusLocation,
																			 const FVector_NetQuantizeNormal InFocusNormal) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_FocusBeginLocal(InteractiveTarget, InFocusOrigin, InFocusLocation, InFocusNormal);
}

#pragma endregion

#pragma region FocusEnd

void UInteractiveControlComponent::Standalone_FocusEndLocal(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Standalone_FocusEnd(this);
}

void UInteractiveControlComponent::Server_FocusEndLocal(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);

	// Local notify the interactive actor at the server.
	InteractiveTarget->Server_FocusEnd(this);

	// Should we replicate to clients.
	if (InteractiveTarget->IsReplicating(EInteractionType::Focus)) {
		// Should we multicast to clients.
		if (InteractiveTarget->IsMulticasting(EInteractionType::Focus)) {
			Multicast_FocusEndRemote(InteractiveTarget);
		} else {
			Client_FocusEndRemote(InteractiveTarget);
		}
	}
}

void UInteractiveControlComponent::Client_FocusEndLocal(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Client_FocusEnd(this);
}

void UInteractiveControlComponent::Client_FocusEndRemote_Implementation(UInteractiveTargetComponent* Target) {
	CHECK_RETURN(Target);
	Client_FocusEndLocal(Target);
}

void UInteractiveControlComponent::Multicast_FocusEndRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_FocusEndLocal(InteractiveTarget);
}

#pragma endregion

#pragma region Overlap Begin
void UInteractiveControlComponent::Standalone_OverlapBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
																const FVector_NetQuantize InFocusOrigin,
																const FVector_NetQuantize InFocusLocation,
																const FVector_NetQuantizeNormal InFocusNormal) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Standalone_OverlapBegin(this, InFocusOrigin, InFocusLocation, InFocusNormal);
}

void UInteractiveControlComponent::Server_OverlapBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
															const FVector_NetQuantize InFocusOrigin,
															const FVector_NetQuantize InFocusLocation,
															const FVector_NetQuantizeNormal InFocusNormal) {
	CHECK_RETURN(InteractiveTarget);

	// Local notify the interactive actor at the server.
	InteractiveTarget->Server_OverlapBegin(this, InFocusOrigin, InFocusLocation, InFocusNormal);

	// Should we replicate to clients.
	if (InteractiveTarget->IsReplicating(EInteractionType::Overlap)) {
		// Should we multicast to clients.
		if (InteractiveTarget->IsMulticasting(EInteractionType::Overlap)) {
			Multicast_OverlapBeginRemote(InteractiveTarget, InFocusOrigin, InFocusLocation, InFocusNormal);
		} else {
			Client_OverlapBeginRemote(InteractiveTarget, InFocusOrigin, InFocusLocation, InFocusNormal);
		}
	}
}

void UInteractiveControlComponent::Client_OverlapBeginLocal(UInteractiveTargetComponent* Target,
															const FVector_NetQuantize InFocusOrigin,
															const FVector_NetQuantize InFocusLocation,
															const FVector_NetQuantizeNormal InFocusNormal) {
	CHECK_RETURN(Target);
	Target->Client_OverlapBegin(this, InFocusOrigin, InFocusLocation, InFocusNormal);
}

void UInteractiveControlComponent::Client_OverlapBeginRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																			const FVector_NetQuantize InFocusOrigin,
																			const FVector_NetQuantize InFocusLocation,
																			const FVector_NetQuantizeNormal InFocusNormal) {
	CHECK_RETURN(InteractiveTarget);
	Client_OverlapBeginLocal(InteractiveTarget, InFocusOrigin, InFocusLocation, InFocusNormal);
}

void UInteractiveControlComponent::Multicast_OverlapBeginRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																			   const FVector_NetQuantize InFocusOrigin,
																			   const FVector_NetQuantize InFocusLocation,
																			   const FVector_NetQuantizeNormal InFocusNormal) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_OverlapBeginLocal(InteractiveTarget, InFocusOrigin, InFocusLocation, InFocusNormal);
}

#pragma endregion

#pragma region Overlap End

void UInteractiveControlComponent::Standalone_OverlapEndLocal(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Standalone_OverlapEnd(this);
}

void UInteractiveControlComponent::Server_OverlapEndLocal(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);

	// Local notify the interactive actor at the server.
	InteractiveTarget->Server_OverlapEnd(this);

	// Should we replicate to clients.
	if (InteractiveTarget->IsReplicating(EInteractionType::Overlap)) {
		// Should we multicast to clients.
		if (InteractiveTarget->IsMulticasting(EInteractionType::Overlap)) {
			Multicast_OverlapEndRemote(InteractiveTarget);
		} else {
			Client_OverlapEndRemote(InteractiveTarget);
		}
	}
}

void UInteractiveControlComponent::Client_OverlapEndLocal(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Client_OverlapEnd(this);
}

void UInteractiveControlComponent::Client_OverlapEndRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	Client_OverlapEndLocal(InteractiveTarget);
}

void UInteractiveControlComponent::Multicast_OverlapEndRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_OverlapEndLocal(InteractiveTarget);
}

#pragma endregion

#pragma region Activate Select
UInteractiveTargetComponent* UInteractiveControlComponent::SelectActivateTarget() {
	// Try to get a focused target.
	if (bUseFocus && FocusedTarget != nullptr) {
		ActivateLocation = FocusLocation;
		ActivateOrigin = FocusOrigin;
		ActivateNormal = FocusNormal;
		return ActivateTarget = FocusedTarget;
	}

	// Try to get a overlapped target.
	if (bUseOverlap && OverlappedTarget != nullptr) {
		ActivateLocation = OverlapLocation;
		ActivateOrigin = OverlapOrigin;
		ActivateNormal = OverlapNormal;
		return ActivateTarget = OverlappedTarget;
	}

	ActivateLocation = FVector::ZeroVector;
	ActivateOrigin = FVector::ZeroVector;
	ActivateNormal = FVector::ForwardVector; // Not sure if we can use zero for the normal so use forward.

	return ActivateTarget = nullptr;
}

UInteractiveTargetComponent* UInteractiveControlComponent::SelectGrabTarget() {
	if (GrabTarget != nullptr) {
		return GrabTarget;
	}

	if (ActivateTarget != nullptr) {
		return GrabTarget = ActivateTarget;
	}

	// Try to get a focused target.
	if (bUseFocus && FocusedTarget != nullptr) {
		GrabLocation = FocusLocation;
		GrabOrigin = FocusOrigin;
		GrabNormal = FocusNormal;
		return GrabTarget = FocusedTarget;
	}

	// Try to get a overlapped target.
	if (bUseOverlap && OverlappedTarget != nullptr) {
		GrabLocation = OverlapLocation;
		GrabOrigin = OverlapOrigin;
		GrabNormal = OverlapNormal;
		return GrabTarget = OverlappedTarget;
	}

	GrabLocation = FVector::ZeroVector;
	GrabOrigin = FVector::ZeroVector;
	GrabNormal = FVector::ForwardVector; // Not sure if we can use zero for the normal so use forward.

	return GrabTarget = nullptr;
}
#pragma endregion

#pragma region Activate Begin

void UInteractiveControlComponent::Standalone_ActivateBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
																 const FVector_NetQuantize InOrigin,
																 const FVector_NetQuantize InLocation,
																 const FVector_NetQuantizeNormal InNormal) {
	CHECK_RETURN(InteractiveTarget);

	OnActivateBegin.Broadcast(this, InteractiveTarget);

	InteractiveTarget->Standalone_ActivateBegin(this, InOrigin, InLocation, InNormal);
}

void UInteractiveControlComponent::Server_ActivateBeginLocal() {
	const auto InteractiveTarget = SelectActivateTarget();
	CHECK_RETURN(InteractiveTarget);

	OnActivateBegin.Broadcast(this, InteractiveTarget);

	// Local notify the interactive target at the server.
	InteractiveTarget->Server_ActivateBegin(this, ActivateOrigin, ActivateLocation, ActivateNormal);

	if (InteractiveTarget->IsReplicating(EInteractionType::Activate)) {
		if (InteractiveTarget->IsMulticasting(EInteractionType::Activate)) {
			Multicast_ActivateBeginRemote(InteractiveTarget, ActivateOrigin, ActivateLocation, ActivateNormal);
		} else {
			Client_ActivateBeginRemote(InteractiveTarget, ActivateOrigin, ActivateLocation, ActivateNormal);
		}
	}
}

void UInteractiveControlComponent::Client_ActivateBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
															 const FVector_NetQuantize InOrigin,
															 const FVector_NetQuantize InLocation,
															 const FVector_NetQuantizeNormal InNormal) {
	CHECK_RETURN(InteractiveTarget);

	OnActivateBegin.Broadcast(this, InteractiveTarget);

	InteractiveTarget->Client_ActivateBegin(this, InOrigin, InLocation, InNormal);
}

void UInteractiveControlComponent::Server_ActivateBeginRemote_Implementation(UInteractiveControlComponent* ProxyControl) {
	// Check if we should process the event as ourselves, or leave it to the proxy component at the server side.
	if (ProxyControl == nullptr) {
		Server_ActivateBeginLocal();
		return;
	}
	ProxyControl->Server_ActivateBeginLocal();
}

void UInteractiveControlComponent::Client_ActivateBeginRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																			 const FVector_NetQuantize InOrigin,
																			 const FVector_NetQuantize InLocation,
																			 const FVector_NetQuantizeNormal InNormal) {
	CHECK_RETURN(InteractiveTarget);
	Client_ActivateBeginLocal(InteractiveTarget, InOrigin, InLocation, InNormal);
}

void UInteractiveControlComponent::Multicast_ActivateBeginRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																				const FVector_NetQuantize InOrigin,
																				const FVector_NetQuantize InLocation,
																				const FVector_NetQuantizeNormal InNormal) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_ActivateBeginLocal(InteractiveTarget, InOrigin, InLocation, InNormal);
}
#pragma endregion

#pragma region Activate End
void UInteractiveControlComponent::Standalone_ActivateEndLocal(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);

	OnActivateEnd.Broadcast(this, InteractiveTarget);

	InteractiveTarget->Standalone_ActivateEnd(this);
	ActivateTarget = nullptr;
}

void UInteractiveControlComponent::Server_ActivateEndLocal() {
	CHECK_RETURN(ActivateTarget);

	OnActivateEnd.Broadcast(this, ActivateTarget);

	ActivateTarget->Server_ActivateEnd(this);
	if (ActivateTarget->IsReplicating(EInteractionType::Activate)) {
		if (ActivateTarget->IsMulticasting(EInteractionType::Activate)) {
			Multicast_ActivateEndRemote(ActivateTarget);
		} else {
			Client_ActivateEndRemote(ActivateTarget);
		}
	}
	ActivateTarget = nullptr;
}

void UInteractiveControlComponent::Client_ActivateEndLocal(UInteractiveTargetComponent* InteractiveTarget) {
	if (InteractiveTarget == nullptr) {
		CHECK_RETURN(ActivateTarget);
		if (ActivateTarget->IsClientOnly(EInteractionType::Activate)) {
			ActivateTarget->Client_ActivateEnd(this);
		}
	} else {
		InteractiveTarget->Client_ActivateEnd(this);
	}

	OnActivateEnd.Broadcast(this, InteractiveTarget);

	ActivateTarget = nullptr;
}

void UInteractiveControlComponent::Server_ActivateEndRemote_Implementation(UInteractiveControlComponent* ProxyControl) {
	if (ProxyControl == nullptr) {
		Server_ActivateEndLocal();
		return;
	}
	ProxyControl->Server_ActivateEndLocal();
}

void UInteractiveControlComponent::Client_ActivateEndRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	Client_ActivateEndLocal(InteractiveTarget);
}

void UInteractiveControlComponent::Multicast_ActivateEndRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_ActivateEndLocal(InteractiveTarget);
}
#pragma endregion

#pragma region Activate Axis
void UInteractiveControlComponent::Standalone_ActivateAxisLocal(UInteractiveTargetComponent* InteractiveTarget,
																const EInteractionInputAxisType Axis, const float Value) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Standalone_ActivateAxis(this, Axis, Value);
}

void UInteractiveControlComponent::Server_ActivateAxisLocal(const EInteractionInputAxisType Axis, const float Value) {
	UInteractiveTargetComponent* InteractiveTarget = SelectActivateTarget();

	CHECK_RETURN(InteractiveTarget);

	// Local notify the interactive target at the server.
	InteractiveTarget->Server_ActivateAxis(this, Axis, Value);

	if (InteractiveTarget->IsReplicating(EInteractionType::Activate)) {
		if (InteractiveTarget->IsMulticasting(EInteractionType::Activate)) {
			Multicast_ActivateAxisRemote(InteractiveTarget, Axis, Value);
		} else {
			Client_ActivateAxisRemote(InteractiveTarget, Axis, Value);
		}
	}
}

void UInteractiveControlComponent::Client_ActivateAxisLocal(UInteractiveTargetComponent* InteractiveTarget,
															const EInteractionInputAxisType Axis, const float Value) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Client_ActivateAxis(this, Axis, Value);
}

void UInteractiveControlComponent::Server_ActivateAxisRemote_Implementation(const EInteractionInputAxisType Axis, const float Value,
																			UInteractiveControlComponent* ProxyControl) {
	if (ProxyControl == nullptr) {
		Server_ActivateAxisLocal(Axis, Value);
	} else {
		ProxyControl->Server_ActivateAxisLocal(Axis, Value);
	}
}

void UInteractiveControlComponent::Client_ActivateAxisRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																			const EInteractionInputAxisType Axis, const float Value) {
	CHECK_RETURN(InteractiveTarget);
	Client_ActivateAxisLocal(InteractiveTarget, Axis, Value);
}

void UInteractiveControlComponent::Multicast_ActivateAxisRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																			   const EInteractionInputAxisType Axis, const float Value) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}

	Client_ActivateAxisLocal(InteractiveTarget, Axis, Value);
}
#pragma endregion

#pragma region Grab Begin

void UInteractiveControlComponent::Standalone_GrabBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
															 const FVector_NetQuantize InOrigin,
															 const FVector_NetQuantize InLocation,
															 const FVector_NetQuantizeNormal InNormal) {
	CHECK_RETURN(InteractiveTarget);
	OnGrabBegin.Broadcast(this, InteractiveTarget);

	InteractiveTarget->Standalone_GrabBegin(this, InOrigin, InLocation, InNormal);
}

void UInteractiveControlComponent::Server_GrabBeginLocal() {
	const auto InteractiveTarget = SelectGrabTarget();
	CHECK_RETURN(InteractiveTarget);

	// Local notify the interactive target at the server.
	InteractiveTarget->Server_GrabBegin(this, GrabOrigin, GrabLocation, GrabNormal);

	OnGrabBegin.Broadcast(this, InteractiveTarget);

	if (InteractiveTarget->IsReplicating(EInteractionType::Grab)) {
		if (InteractiveTarget->IsMulticasting(EInteractionType::Grab)) {
			Multicast_GrabBeginRemote(InteractiveTarget, GrabOrigin, GrabLocation, GrabNormal);
		} else {
			Client_GrabBeginRemote(InteractiveTarget, GrabOrigin, GrabLocation, GrabNormal);
		}
	}
}

void UInteractiveControlComponent::Client_GrabBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
														 const FVector_NetQuantize InOrigin,
														 const FVector_NetQuantize InLocation,
														 const FVector_NetQuantizeNormal InNormal) {
	CHECK_RETURN(InteractiveTarget);
	OnGrabBegin.Broadcast(this, InteractiveTarget);

	InteractiveTarget->Client_GrabBegin(this, InOrigin, InLocation, InNormal);
}

void UInteractiveControlComponent::Server_GrabBeginRemote_Implementation(UInteractiveControlComponent* ProxyControl) {
	// Check if we should process the event as ourselves, or leave it to the proxy component at the server side.
	if (ProxyControl == nullptr) {
		Server_GrabBeginLocal();
		return;
	}
	ProxyControl->Server_GrabBeginLocal();
}

void UInteractiveControlComponent::Client_GrabBeginRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																		 const FVector_NetQuantize InOrigin,
																		 const FVector_NetQuantize InLocation,
																		 const FVector_NetQuantizeNormal InNormal) {
	CHECK_RETURN(InteractiveTarget);
	Client_GrabBeginLocal(InteractiveTarget, InOrigin, InLocation, InNormal);
}

void UInteractiveControlComponent::Multicast_GrabBeginRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																			const FVector_NetQuantize InOrigin,
																			const FVector_NetQuantize InLocation,
																			const FVector_NetQuantizeNormal InNormal) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_GrabBeginLocal(InteractiveTarget, InOrigin, InLocation, InNormal);
}
#pragma endregion

#pragma region Grab End
void UInteractiveControlComponent::Standalone_GrabEndLocal(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Standalone_GrabEnd(this);

	OnGrabEnd.Broadcast(this, InteractiveTarget);

	if (ActivateTarget == GrabTarget) {
		ActivateTarget = nullptr;
	}
	GrabTarget = nullptr;
	UseTarget = nullptr;
}

void UInteractiveControlComponent::Server_GrabEndLocal() {
	CHECK_RETURN(GrabTarget);
	GrabTarget->Server_GrabEnd(this);

	OnGrabEnd.Broadcast(this, GrabTarget);

	if (GrabTarget->IsReplicating(EInteractionType::Grab)) {
		if (GrabTarget->IsMulticasting(EInteractionType::Grab)) {
			Multicast_GrabEndRemote(GrabTarget);
		} else {
			Client_GrabEndRemote(GrabTarget);
		}
	}
	if (ActivateTarget == GrabTarget) {
		ActivateTarget = nullptr;
	}
	GrabTarget = nullptr;
	UseTarget = nullptr;
}

void UInteractiveControlComponent::Client_GrabEndLocal(UInteractiveTargetComponent* InteractiveTarget) {
	if (InteractiveTarget == nullptr) {
		CHECK_RETURN(GrabTarget);
		if (GrabTarget->IsClientOnly(EInteractionType::Grab)) {
			GrabTarget->Client_GrabEnd(this);
		}
	} else {
		InteractiveTarget->Client_GrabEnd(this);
	}

	OnGrabEnd.Broadcast(this, InteractiveTarget);

	if (ActivateTarget == GrabTarget) {
		ActivateTarget = nullptr;
	}
	GrabTarget = nullptr;
	UseTarget = nullptr;
}

void UInteractiveControlComponent::Server_GrabEndRemote_Implementation(UInteractiveControlComponent* ProxyControl) {
	if (ProxyControl == nullptr) {
		Server_GrabEndLocal();
		return;
	}
	ProxyControl->Server_GrabEndLocal();
}

void UInteractiveControlComponent::Client_GrabEndRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	Client_GrabEndLocal(InteractiveTarget);
}

void UInteractiveControlComponent::Multicast_GrabEndRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_GrabEndLocal(InteractiveTarget);
}
#pragma endregion

#pragma region Use Select

UInteractiveTargetComponent* UInteractiveControlComponent::SelectUseTarget() const {
	// Todo: in future we could have different attached targets at different slots, etc. that can be used by different events.
	return UseTarget;
}

#pragma endregion

#pragma region Use Begin

void UInteractiveControlComponent::Standalone_UseBeginLocal(UInteractiveTargetComponent* InteractiveTarget, const uint8 UseIndex) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Standalone_UseBegin(this, UseIndex);
}

void UInteractiveControlComponent::Server_UseBeginLocal(const uint8 UseIndex) {
	// Notice that the server selects the interactive target, client has no authority to do that.
	const auto InteractiveTarget = SelectUseTarget();
	CHECK_RETURN(InteractiveTarget);

	// Local notify the target at the server.
	InteractiveTarget->Server_UseBegin(this, UseIndex);

	if (InteractiveTarget->IsReplicating(EInteractionType::Use)) {
		if (InteractiveTarget->IsMulticasting(EInteractionType::Use)) {
			Multicast_UseBeginRemote(InteractiveTarget, UseIndex);
		} else {
			Client_UseBeginRemote(InteractiveTarget, UseIndex);
		}
	}
}

void UInteractiveControlComponent::Client_UseBeginLocal(UInteractiveTargetComponent* InteractiveTarget, const uint8 UseIndex) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Client_UseBegin(this, UseIndex);
}

void UInteractiveControlComponent::Server_UseBeginRemote_Implementation(UInteractiveControlComponent* ProxyControl, const uint8 UseIndex) {
	if (ProxyControl == nullptr) {
		Server_UseBeginLocal();
		return;
	}
	ProxyControl->Server_UseBeginLocal();
}

void UInteractiveControlComponent::Client_UseBeginRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget, const uint8 UseIndex) {
	CHECK_RETURN(InteractiveTarget == nullptr);
	Client_UseBeginLocal(InteractiveTarget, UseIndex);
}

void UInteractiveControlComponent::Multicast_UseBeginRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget, const uint8 UseIndex) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_UseBeginLocal(InteractiveTarget, UseIndex);
}

#pragma endregion

#pragma region Use End

void UInteractiveControlComponent::Standalone_UseEndLocal(UInteractiveTargetComponent* InteractiveTarget, const uint8 UseIndex) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Standalone_UseEnd(this, UseIndex);
	// UseTarget = nullptr;
}

void UInteractiveControlComponent::Server_UseEndLocal(uint8 UseIndex) {
	// // Notice that the server selects the interactive target, client has no authority to do that.
	// const auto InteractiveTarget = SelectUseTarget();
	// if (InteractiveTarget == nullptr || InteractiveTarget == ActivateTarget && ActivateTarget->AttachedToControl != this) {
	// 	return;
	// }
	CHECK_RETURN(UseTarget);
	UseTarget->Server_UseEnd(this, UseIndex);
	if (UseTarget->IsReplicating(EInteractionType::Use)) {
		if (UseTarget->IsMulticasting(EInteractionType::Use)) {
			Multicast_UseEndRemote(UseTarget, UseIndex);
		} else {
			Client_UseEndRemote(UseTarget, UseIndex);
		}
	}
	// UseTarget = nullptr;
}

void UInteractiveControlComponent::Client_UseEndLocal(UInteractiveTargetComponent* InteractiveTarget, uint8 UseIndex) {
	if (InteractiveTarget == nullptr) {
		CHECK_RETURN(UseTarget);
		if (UseTarget->IsClientOnly(EInteractionType::Use)) {
			UseTarget->Client_UseEnd(this);
		}
	} else {
		InteractiveTarget->Client_UseEnd(this);
	}
	// UseTarget = nullptr;
}

void UInteractiveControlComponent::Server_UseEndRemote_Implementation(UInteractiveControlComponent* ProxyControl, const uint8 UseIndex) {
	if (ProxyControl == nullptr) {
		Server_UseEndLocal(UseIndex);
		return;
	}
	ProxyControl->Server_UseEndLocal(UseIndex);
}

void UInteractiveControlComponent::Client_UseEndRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget, const uint8 UseIndex) {
	CHECK_RETURN(InteractiveTarget);
	Client_UseEndLocal(InteractiveTarget, UseIndex);
}

void UInteractiveControlComponent::Multicast_UseEndRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget, const uint8 UseIndex) {
	CHECK_RETURN(InteractiveTarget);
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	if (!ACTOR_IS_COSMETIC(Owner)) {
		return;
	}
	Client_UseEndLocal(InteractiveTarget, UseIndex);
}

#pragma endregion

#pragma region Use Axis
void UInteractiveControlComponent::Standalone_UseAxisLocal(UInteractiveTargetComponent* InteractiveTarget,
														   const EInteractionInputAxisType Axis, const float Value) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Standalone_UseAxis(this, Axis, Value);
}

void UInteractiveControlComponent::Server_UseAxisLocal(const EInteractionInputAxisType Axis, const float Value) {
	UInteractiveTargetComponent* InteractiveTarget = SelectUseTarget();

	CHECK_RETURN(InteractiveTarget);

	// Local notify the interactive target at the server.
	InteractiveTarget->Server_UseAxis(this, Axis, Value);

	if (InteractiveTarget->IsReplicating(EInteractionType::Activate)) {
		if (InteractiveTarget->IsMulticasting(EInteractionType::Activate)) {
			Multicast_UseAxisRemote(InteractiveTarget, Axis, Value);
		} else {
			Client_UseAxisRemote(InteractiveTarget, Axis, Value);
		}
	}
}

void UInteractiveControlComponent::Client_UseAxisLocal(UInteractiveTargetComponent* InteractiveTarget,
													   const EInteractionInputAxisType Axis, const float Value) {
	CHECK_RETURN(InteractiveTarget);
	InteractiveTarget->Client_UseAxis(this, Axis, Value);
}

void UInteractiveControlComponent::Server_UseAxisRemote_Implementation(const EInteractionInputAxisType Axis, const float Value,
																	   UInteractiveControlComponent* ProxyControl) {
	if (ProxyControl == nullptr) {
		Server_UseAxisLocal(Axis, Value);
	} else {
		ProxyControl->Server_UseAxisLocal(Axis, Value);
	}
}

void UInteractiveControlComponent::Client_UseAxisRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																	   const EInteractionInputAxisType Axis, const float Value) {
	CHECK_RETURN(InteractiveTarget);
	Client_UseAxisLocal(InteractiveTarget, Axis, Value);
}

void UInteractiveControlComponent::Multicast_UseAxisRemote_Implementation(UInteractiveTargetComponent* InteractiveTarget,
																		  const EInteractionInputAxisType Axis, const float Value) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(ACTOR_IS_COSMETIC(Owner)); // Do not notify server with multicast calls as it has own server-only call.
	CHECK_RETURN(InteractiveTarget);
	Client_UseAxisLocal(InteractiveTarget, Axis, Value);
}
#pragma endregion
