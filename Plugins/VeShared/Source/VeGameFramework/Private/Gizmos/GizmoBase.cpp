// Copyright 2020 Juan Marcelo Portillo. All Rights Reserved.
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "Gizmos/GizmoBase.h"
#include "Components/SceneComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "VeGameFramework.h"

// Sets default values
AGizmoBase::AGizmoBase() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// PrimaryActorTick.TickInterval = 0.167f; // 10 Hz

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ScalingSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scaling"));
	ScalingSceneComponent->SetupAttachment(RootComponent);

	X_AxisBox = CreateDefaultSubobject<UBoxComponent>(TEXT("X Axis Box"));
	Y_AxisBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Y Axis Box"));
	Z_AxisBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Z Axis Box"));

	X_AxisBox->SetupAttachment(ScalingSceneComponent);
	Y_AxisBox->SetupAttachment(ScalingSceneComponent);
	Z_AxisBox->SetupAttachment(ScalingSceneComponent);

	RegisterDomainComponent(X_AxisBox, ETransformationDomain::AxisX);
	RegisterDomainComponent(Y_AxisBox, ETransformationDomain::AxisY);
	RegisterDomainComponent(Z_AxisBox, ETransformationDomain::AxisZ);

	GizmoSceneScaleFactor = 0.05f;
	CameraArcRadius = 150.f;

	PreviousRayStartPoint = FVector::ZeroVector;
	PreviousRayEndPoint = FVector::ZeroVector;

	bTransformInProgress = false;
	bIsPrevRayValid = false;
}

void AGizmoBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	//ToDo: There seems to be an issue where the Root Scene doesn't Attach properly on the first 'go' on Unreal 4.26
	// if (RootComponent) {
		// RootComponent->AttachToComponent(RootComponent->GetAttachParent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	// }
}

void AGizmoBase::UpdateGizmoSpace(ETransformSpace SpaceType) {
	switch (SpaceType) {
	case ETransformSpace::Local:
		SetActorRelativeRotation(FQuat(ForceInit));
		break;
	case ETransformSpace::World:
		SetActorRotation(FQuat(ForceInit), ETeleportType::TeleportPhysics);
		break;
	}
}

//Base Gizmo does not affect anything and returns No Delta Transform.
// This func is overriden by each Transform Gizmo

FTransform AGizmoBase::GetDeltaTransform(const FVector& LookingVector, const FVector& RayStartPoint, const FVector& RayEndPoint, ETransformationDomain Domain) {
	FTransform DeltaTransform;
	DeltaTransform.SetScale3D(FVector::ZeroVector);
	return DeltaTransform;
}

void AGizmoBase::ScaleGizmo(const FVector& ReferenceLocation, const FVector& ReferenceLookDirection, const float FieldOfView) {
	const FVector Scale = CalculateGizmoSceneScale(ReferenceLocation, ReferenceLookDirection, FieldOfView);
	if (ScalingSceneComponent) {
		ScalingSceneComponent->SetWorldScale3D(Scale);
	}
}

FTransform AGizmoBase::GetSnappedTransform(FTransform& outCurrentAccumulatedTransform
	, const FTransform& DeltaTransform
	, ETransformationDomain Domain
	, float SnappingValue) const {
	return DeltaTransform;
}

ETransformationDomain AGizmoBase::GetTransformationAxes(USceneComponent* ComponentHit) const {
	if (!ComponentHit) return ETransformationDomain::None;

	if (UShapeComponent* ShapeComponent = Cast<UShapeComponent>(ComponentHit)) {
		if (const ETransformationDomain* pDomain = DomainMap.Find(ShapeComponent))
			return *pDomain;
	} else
		UE_LOG(LogTemp, Warning, TEXT("Failed to Get Domain! Component Hit is not a Shape Component. %s"), *ComponentHit->GetName());

	return ETransformationDomain::None;
}

FVector AGizmoBase::CalculateGizmoSceneScale(const FVector& ReferenceLocation, const FVector& ReferenceLookDirection, float FieldOfView) {
	FVector deltaLocation = (GetActorLocation() - ReferenceLocation);
	float distance = deltaLocation.ProjectOnTo(ReferenceLookDirection).Size();
	float scaleView = (distance * FMath::Sin(FMath::DegreesToRadians(FieldOfView))) / CameraArcRadius;
	scaleView *= GizmoSceneScaleFactor;
	return FVector(scaleView);
}

bool AGizmoBase::AreRaysValid() const {
	return bIsPrevRayValid;
}

void AGizmoBase::UpdateRays(const FVector& RayStart, const FVector& RayEnd) {
	PreviousRayStartPoint = RayStart;
	PreviousRayEndPoint = RayEnd;
	bIsPrevRayValid = true;
}

void AGizmoBase::RegisterDomainComponent(USceneComponent* Component
	, ETransformationDomain Domain) {
	if (!Component) return;

	if (UShapeComponent* ShapeComponent = Cast<UShapeComponent>(Component)) {
		DomainMap.Add(ShapeComponent, Domain);
	} else {
		VeLogErrorFunc("Failed to Register Component! Component is not a Shape Component %s", *Component->GetName());
	}
}

void AGizmoBase::SetTransformProgressState(bool bInProgress, ETransformationDomain CurrentDomain) {
	VeLogFunc("transform progress state: %d vs %d", bTransformInProgress, bInProgress);
	if (bInProgress != bTransformInProgress) {
		bIsPrevRayValid = false; //set this so that we don't get an invalid delta value
		bTransformInProgress = bInProgress;
		OnGizmoStateChanged.Broadcast(GetGizmoType(), bTransformInProgress, CurrentDomain);
		OnGizmoStateChangedBP(GetGizmoType(), bTransformInProgress, CurrentDomain);
	}
}
