// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "Pawns/InteractiveMotionController.h"


#include "DrawDebugHelpers.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Interaction.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "XRMotionControllerBase.h"
#include "Components/ArrowComponent.h"
#include "Components/InteractiveControlComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Enums/InteractionType.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "VeShared.h"

// Sets default values
AInteractiveMotionController::AInteractiveMotionController() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	OverlapTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapTrigger"));
	TeleportArcDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TeleportArcDirection"));
	TeleportArcSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportArcSpline"));
	TeleportArcEndPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportArcEndPoint"));
	TeleportTargetPositionMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportTargetPositionMarker"));
	TeleportTargetDirectionMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportTargetDirectionMarker"));

	RootComponent = RootSceneComponent;

	MotionController->SetupAttachment(RootSceneComponent);
	MotionController->SetRelativeLocation(FVector(0));
	MotionController->SetCollisionProfileName(TEXT("BlockAll"));

	TeleportArcDirection->SetupAttachment(MotionController);
	TeleportArcDirection->SetRelativeLocation(FVector(0));
	TeleportArcDirection->ArrowSize = 0.2f;

	TeleportArcSpline->SetupAttachment(MotionController);
	TeleportArcSpline->SetRelativeLocation(FVector(0));

	OverlapTrigger->SetupAttachment(MotionController);
	OverlapTrigger->SetRelativeLocation(FVector(0));
	OverlapTrigger->SetSphereRadius(10.f);
	OverlapTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	OverlapTrigger->SetGenerateOverlapEvents(true);

	TeleportArcEndPoint->SetupAttachment(RootComponent);
	TeleportArcEndPoint->SetUsingAbsoluteLocation(true);
	TeleportArcEndPoint->SetUsingAbsoluteRotation(true);
	TeleportArcEndPoint->SetUsingAbsoluteScale(true);
	TeleportArcEndPoint->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.15f));
	TeleportArcEndPoint->SetGenerateOverlapEvents(false);
	TeleportArcEndPoint->SetCollisionProfileName(TEXT("NoCollision"));
	TeleportArcEndPoint->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_ArcEndPoint(
		TEXT("StaticMesh'/VeInteraction/Interaction/VR/Meshes/SM_TeleportArcEndPoint.SM_TeleportArcEndPoint'"), LOAD_NoWarn);
	static ConstructorHelpers::FObjectFinder<UMaterial> M_ArcEndPoint(
		TEXT("Material'/VeInteraction/Interaction/VR/Materials/M_TeleportArcEndPoint.M_TeleportArcEndPoint'"), LOAD_NoWarn);

	if (SM_ArcEndPoint.Succeeded()) {
		TeleportArcEndPoint->SetStaticMesh(SM_ArcEndPoint.Object);
		if (M_ArcEndPoint.Succeeded()) {
			TeleportArcEndPoint->SetMaterial(0, M_ArcEndPoint.Object);
		}
	}

	TeleportTargetPositionMarker->SetupAttachment(RootComponent);
	TeleportTargetPositionMarker->SetUsingAbsoluteLocation(true);
	TeleportTargetPositionMarker->SetUsingAbsoluteRotation(true);
	TeleportTargetPositionMarker->SetUsingAbsoluteScale(true);
	TeleportTargetPositionMarker->SetRelativeScale3D(FVector(0.75f, 0.75f, 1.f));
	TeleportTargetPositionMarker->SetGenerateOverlapEvents(false);
	TeleportTargetPositionMarker->SetCollisionProfileName(TEXT("NoCollision"));
	TeleportTargetPositionMarker->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_TeleportTargetPosition(
		TEXT("StaticMesh'/VeInteraction/Interaction/VR/Meshes/SM_TeleportTargetPosition.SM_TeleportTargetPosition'"), LOAD_NoWarn | LOAD_Quiet);
	static ConstructorHelpers::FObjectFinder<UMaterial> M_TeleportTargetPosition(
		TEXT("Material'/VeInteraction/Interaction/VR/Materials/M_TeleportTargetPosition.M_TeleportTargetPosition'"), LOAD_NoWarn | LOAD_Quiet);

	if (SM_TeleportTargetPosition.Succeeded()) {
		TeleportTargetPositionMarker->SetStaticMesh(SM_TeleportTargetPosition.Object);
		if (M_TeleportTargetPosition.Succeeded()) {
			TeleportTargetPositionMarker->SetMaterial(0, M_TeleportTargetPosition.Object);
		}
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_TeleportTargetDirection(
		TEXT("StaticMesh'/VeInteraction/Interaction/VR/Meshes/SM_TeleportTargetDirection.SM_TeleportTargetDirection'"), LOAD_NoWarn | LOAD_Quiet);
	static ConstructorHelpers::FObjectFinder<UMaterial> M_TeleportTargetDirection(
		TEXT("Material'/VeInteraction/Interaction/VR/Materials/M_TeleportTargetDirection.M_TeleportTargetDirection'"), LOAD_NoWarn | LOAD_Quiet);

	if (SM_TeleportTargetDirection.Succeeded()) {
		TeleportTargetDirectionMarker->SetStaticMesh(SM_TeleportTargetDirection.Object);
		if (M_TeleportTargetDirection.Succeeded()) {
			TeleportTargetDirectionMarker->SetMaterial(0, M_TeleportTargetDirection.Object);
		}
	}

	TeleportTargetDirectionMarker->SetupAttachment(TeleportTargetPositionMarker);
	TeleportTargetDirectionMarker->SetGenerateOverlapEvents(false);
	TeleportTargetDirectionMarker->SetCollisionProfileName(TEXT("NoCollision"));
	TeleportTargetDirectionMarker->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_TeleportArcSpline(
		TEXT("StaticMesh'/VeInteraction/Interaction/VR/Meshes/SM_TeleportArcSpline.SM_TeleportArcSpline'"), LOAD_NoWarn | LOAD_Quiet);
	static ConstructorHelpers::FObjectFinder<UMaterial> M_TeleportArcSpline(
		TEXT("Material'/VeInteraction/Interaction/VR/Materials/M_TeleportArcSpline.M_TeleportArcSpline'"), LOAD_NoWarn | LOAD_Quiet);

	if (SM_TeleportArcSpline.Succeeded()) {
		TeleportArcSplineMesh = SM_TeleportArcSpline.Object;
		if (M_TeleportArcSpline.Succeeded()) {
			TeleportArcSplineMaterial = M_TeleportArcSpline.Object;
			TeleportArcSpline->SetMaterial(0, TeleportArcSplineMaterial);
		}
	}

	MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	bIsTeleportActive = false;
	SplineMeshes.Empty();
	bIsLastFrameValidTeleportDestination = false;
	TeleportDestination = FVector(0);
	bIsValidTeleportDestination = false;
	TeleportRotation = FRotator(0);
	TeleportLaunchVelocity = 900.0f;
	InitialControllerRotation = FRotator(0);

	InteractiveControl = CreateDefaultSubobject<UInteractiveControlComponent>(TEXT("InteractiveControl"));
	InteractiveControl->SetIsReplicated(true);
	InteractiveControl->SetControlMode(EInteractionControlMode::MotionController);

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>("PhysicsHandle");
	InteractiveControl->PhysicsHandle = PhysicsHandle;
}

// Called when the game starts or when spawned
void AInteractiveMotionController::BeginPlay() {
	Super::BeginPlay();

	SetReplicates(true);
	SetReplicatingMovement(true);

	if (OverlapTrigger && InteractiveControl) {
		OverlapTrigger->OnComponentBeginOverlap.AddDynamic(InteractiveControl, &UInteractiveControlComponent::OnOverlapBegin);
		OverlapTrigger->OnComponentEndOverlap.AddDynamic(InteractiveControl, &UInteractiveControlComponent::OnOverlapEnd);
	}

	if (TeleportTargetDirectionMarker) {
		TeleportTargetDirectionMarker->SetVisibility(false, true);
	}
}

// Called every frame
void AInteractiveMotionController::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

	ClearTeleportArcSpline();

	if (bIsTeleportActive) {
		TArray<FVector> TracePoints;
		FVector NavMeshLocation;
		FVector TraceLocation;
		bIsValidTeleportDestination = TraceTeleportDestination(TracePoints, NavMeshLocation, TraceLocation);

		TeleportTargetPositionMarker->SetVisibility(bIsValidTeleportDestination, true);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TArray<AActor*> IgnoreActors;
		FHitResult HitResult;

		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		IgnoreActors.Add(this);

		const auto TraceStartLocation = NavMeshLocation + FVector(0.f, 0.f, 50.f);
		const auto TraceEndLocation = NavMeshLocation + FVector(0.f, 0.f, -50.f);

		// Trace down to find a valid location for player to stand at (original NavMesh location is offset upwards, so we must find the actual floor)
		UKismetSystemLibrary::LineTraceSingleForObjects(this,
														TraceStartLocation,
														TraceEndLocation,
														ObjectTypes,
														false,
														IgnoreActors,
														EDrawDebugTrace::None,
														HitResult,
														true,
														FLinearColor::Red);

		const auto NewLocation = UKismetMathLibrary::SelectVector(HitResult.ImpactPoint, NavMeshLocation, HitResult.bBlockingHit);
		TeleportTargetPositionMarker->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

		if ((bIsValidTeleportDestination && !bIsLastFrameValidTeleportDestination) || (!bIsValidTeleportDestination && bIsLastFrameValidTeleportDestination)) {
			HapticFeedback(0.25f);
		}

		bIsLastFrameValidTeleportDestination = bIsValidTeleportDestination;

		UpdateTeleportArcSpline(bIsValidTeleportDestination, TracePoints);
		UpdateTeleportArcEndPoint(bIsValidTeleportDestination, TraceLocation);
	}
	if (bDebugDrawSphere) {
		const auto World = GetWorld();
		if (World != nullptr) {
			DrawDebugSphere(World, GetMotionController()->GetComponentToWorld().GetLocation(), 5.0f, 12.0, FColor::Red, false, 0.f, 0.0f, 0.1);
		}
	}
}

void AInteractiveMotionController::SetMotionSource(const FName InMotionSource) {
	CHECK_RETURN(MotionController);
	MotionSource = InMotionSource;
	MotionController->SetTrackingMotionSource(InMotionSource);
	MotionController->SetTrackingSource(MotionController->GetTrackingSource());
}

FTransform AInteractiveMotionController::GetMotionControllerTransform() const {
	if (MotionController) {
		return MotionController->GetComponentTransform();
	}
	return FTransform::Identity;
}

void AInteractiveMotionController::SetMotionControllerTransform(const FTransform InTransform) const {
	if (MotionController) {
		MotionController->SetWorldTransform(InTransform);
	}
}

void AInteractiveMotionController::SetTeleportRotation(const FRotator InRotation) {
	TeleportRotation = InRotation;
}

bool AInteractiveMotionController::TraceTeleportDestination(TArray<FVector>& TracePoints, FVector& NavMeshLocation, FVector& TraceLocation) {
	const auto StartPosition = TeleportArcDirection->GetComponentLocation();
	const auto LaunchVelocity = TeleportArcDirection->GetForwardVector() * TeleportLaunchVelocity;

	FPredictProjectilePathParams PredictProjectilePathParams(0.f, StartPosition, LaunchVelocity, 2.0f, UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	FPredictProjectilePathResult PredictProjectilePathResult;

	// Simulate throwing a projectile (including gravity) to find a teleport location.
	const bool bSuccessPredictProjectilePath = UGameplayStatics::PredictProjectilePath(this, PredictProjectilePathParams, PredictProjectilePathResult);
	if (!bSuccessPredictProjectilePath) {
		return false;
	}

	UNavigationSystemV1* NavigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(this);
	FVector Point = PredictProjectilePathResult.HitResult.Location;
	FNavLocation ProjectedLocation;
	FVector ProjectNavExtents = FVector(500.f);

	// Project our hit against the NavMesh to find a place for player to stand.
	const bool bSuccessProjectToNav = NavigationSystemV1->ProjectPointToNavigation(Point, ProjectedLocation, ProjectNavExtents);
	if (!bSuccessProjectToNav) {
		return false;
	}

	TracePoints.Empty();

	for (auto PathPoint : PredictProjectilePathResult.PathData) {
		TracePoints.Add(PathPoint.Location);
	}

	TraceLocation = Point;
	NavMeshLocation = ProjectedLocation.Location;

	return true;
}

void AInteractiveMotionController::UpdateTeleportArcSpline(const bool bHaveValidLocation, TArray<FVector> SplinePoints) {
	if (!bHaveValidLocation) {
		const auto ArcLocation = TeleportArcDirection->GetComponentLocation();
		const auto ArcForward = TeleportArcDirection->GetForwardVector();

		SplinePoints.Empty();
		SplinePoints.Add(ArcLocation);

		// Draw just a small part of the arc as we can't build a full arc spline to the destination location.
		const auto NextPointLocation = ArcLocation + ArcForward * 20.0f;
		SplinePoints.Add(NextPointLocation);
	}

	// Build a spline from all trace points. 
	for (auto Point : SplinePoints) {
		TeleportArcSpline->AddSplinePoint(Point, ESplineCoordinateSpace::Local);
	}

	const auto SplinePointCount = TeleportArcSpline->GetNumberOfSplinePoints();
	const auto LastSplinePointIndex = SplinePoints.Num() - 1;
	// Minus two because we use i and i+1 in calculation.
	const auto LastSafeSplinePointIndex = SplinePointCount - 2;

	TeleportArcSpline->SetSplinePointType(LastSplinePointIndex, ESplinePointType::CurveClamped);

	// Create all spline mesh fragment components. This generates tangents we can use to build a smoothly curved spline mesh.
	for (int i = 0; i <= LastSafeSplinePointIndex; i++) {
		const auto SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), FName(*FString::Printf(TEXT("TeleportArcSpline %d"), i)));
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->SetStaticMesh(TeleportArcSplineMesh);
		SplineMeshComponent->SetMaterial(0, TeleportArcSplineMaterial);
		SplineMeshComponent->SetRelativeLocation(FVector(0.f));
		SplineMeshComponent->SetRelativeRotation(FRotator(0.f));
		SplineMeshComponent->SetRelativeScale3D(FVector(1.f));
		SplineMeshComponent->SetForwardAxis(ESplineMeshAxis::X);
		SplineMeshComponent->SetStartScale(FVector2D(4.f));
		SplineMeshComponent->SetEndScale(FVector2D(4.f));
		SplineMeshComponent->SetBoundaryMax(1.0f);
		SplineMeshComponent->RegisterComponent();

		SplineMeshes.Add(SplineMeshComponent);

		const auto StartPosition = SplinePoints[i];
		const auto StartTangent = TeleportArcSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const auto EndPosition = SplinePoints[i + 1];
		const auto EndTangent = TeleportArcSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		SplineMeshComponent->SetStartAndEnd(StartPosition, StartTangent, EndPosition, EndTangent);
	}
}

void AInteractiveMotionController::UpdateTeleportArcEndPoint(const bool bHaveValidLocation, const FVector EndPointLocation) const {
	const bool bIsArcEndPointVisible = bHaveValidLocation && bIsTeleportActive;
	TeleportArcEndPoint->SetVisibility(bIsArcEndPointVisible);
	TeleportArcEndPoint->SetWorldLocation(EndPointLocation, false, nullptr, ETeleportType::TeleportPhysics);

	FRotator DeviceRotation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);

	// Combine the two rotations to get an accurate preview of where player will look to after a teleport.
	const auto DirectionMarkerRotation = UKismetMathLibrary::ComposeRotators(TeleportRotation, FRotator(0.f, DeviceRotation.Yaw, 0.f));

	TeleportTargetDirectionMarker->SetWorldRotation(DirectionMarkerRotation);
}

void AInteractiveMotionController::ClearTeleportArcSpline() {
	for (auto Spline : SplineMeshes) {
		Spline->DestroyComponent();
	}
	SplineMeshes.Empty();
	TeleportArcSpline->ClearSplinePoints();
}

void AInteractiveMotionController::GetTeleportDestination(FVector& OutLocation, FRotator& OutRotation) const {
	FRotator DeviceRotation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);

	// Ignore relative height difference.
	const FVector RotatedVector = TeleportRotation.RotateVector(FVector(DevicePosition.X, DevicePosition.Y, 0.f));

	// Subtract HMD origin (Camera) to get correct Pawn destination for teleportation.
	OutLocation = TeleportTargetPositionMarker->GetComponentLocation() - RotatedVector;
	OutRotation = TeleportRotation;
}

void AInteractiveMotionController::EnableTeleportationVisuals() {
	// Set the flag, rest of the teleportation is handled during Tick event.
	bIsTeleportActive = true;
	TeleportTargetPositionMarker->SetVisibility(true, true);

	// Store Rotation to later compare Roll value to support wrist-based orientation of the motion controller.
	if (MotionController != nullptr) {
		InitialControllerRotation = MotionController->GetComponentRotation();
	}
}

void AInteractiveMotionController::DisableTeleportationVisuals() {
	if (bIsTeleportActive) {
		bIsTeleportActive = false;
		TeleportTargetPositionMarker->SetVisibility(false, true);
		TeleportArcEndPoint->SetVisibility(false);
	}
}

void AInteractiveMotionController::HapticFeedback(const float Intensity) {
	const APawn* Pawn = Cast<APawn>(GetOwner());
	CHECK_RETURN(Pawn);

	APlayerController* Controller = Cast<APlayerController>(Pawn->GetController());
	CHECK_RETURN(Controller);

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
		EControllerHand ControllerHand;
		FXRMotionControllerBase::GetHandEnumForSourceName(MotionSource, ControllerHand);
		Controller->PlayHapticEffect(VRHapticFeedbackEffect, ControllerHand, Intensity);
	}
}
