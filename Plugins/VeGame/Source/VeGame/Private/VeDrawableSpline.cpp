// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeDrawableSpline.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "VeShared.h"


FVeDrawableSplinePoint::FVeDrawableSplinePoint(): Location(FVector::ZeroVector), Scale(0.f) {}

// Sets default values
AVeDrawableSpline::AVeDrawableSpline() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_TeleportArcSpline(
		TEXT("StaticMesh'/VeInteraction/Interaction/VR/Meshes/SM_TeleportArcSpline.SM_TeleportArcSpline'"), LOAD_NoWarn | LOAD_Quiet);
	static ConstructorHelpers::FObjectFinder<UMaterial> M_TeleportArcSpline(
		TEXT("Material'/VeInteraction/Interaction/VR/Materials/M_TeleportArcSpline.M_TeleportArcSpline'"), LOAD_NoWarn | LOAD_Quiet);

	if (SM_TeleportArcSpline.Succeeded()) {
		SplineSegmentMesh = SM_TeleportArcSpline.Object;
		if (M_TeleportArcSpline.Succeeded()) {
			SplineSegmentMaterial = M_TeleportArcSpline.Object;
			Spline->SetMaterial(0, SplineSegmentMaterial);
		}
	}

	SplineMeshes.Empty();

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootSceneComponent;

	Spline->SetupAttachment(RootSceneComponent);
	Spline->SetRelativeLocation(FVector(0));
}

// Called when the game starts or when spawned
void AVeDrawableSpline::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void AVeDrawableSpline::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AVeDrawableSpline::StartDrawing() {
	ClearSpline();
}

void AVeDrawableSpline::FinishDrawing() {
	ClearSpline();
	UpdateSpline();
}

void AVeDrawableSpline::RemoveFirstPoint(bool bDestroyIfEmpty) {
	ClearSpline();

	if (SplinePoints.Num() > 0) {
		SplinePoints.RemoveAt(0);
		UpdateSpline();
	} else {
		if (bDestroyIfEmpty) {
			Destroy();
		}
	}
}

void AVeDrawableSpline::AddSplinePoint(FVeDrawableSplinePoint InSplinePoint) {
	SplinePoints.Add(InSplinePoint);
	ClearSpline();
	UpdateSpline();
}

void AVeDrawableSpline::UpdateSpline() {
	for (FVeDrawableSplinePoint Point : SplinePoints) {
		Spline->AddSplinePoint(Point.Location, ESplineCoordinateSpace::Local);
	}

	const int32 SplinePointCount = Spline->GetNumberOfSplinePoints();
	const int32 LastSplinePointIndex = SplinePoints.Num() - 1;
	// Minus two because we use i and i+1 in calculation.
	const int32 LastSafeSplinePointIndex = SplinePointCount - 2;

	if (LastSplinePointIndex <= 0) {
		return;
	}

	Spline->SetSplinePointType(LastSplinePointIndex, ESplinePointType::CurveClamped);

	for (int32 i = 0; i <= LastSafeSplinePointIndex - 1; i++) {
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(),
																					FName(*FString::Printf(TEXT("TeleportArcSpline %d"), i)));
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->SetStaticMesh(SplineSegmentMesh);
		SplineMeshComponent->SetMaterial(0, SplineSegmentMaterial);
		SplineMeshComponent->SetRelativeLocation(FVector(0.f));
		SplineMeshComponent->SetRelativeRotation(FRotator(0.f));
		SplineMeshComponent->SetRelativeScale3D(SplineSegmentScale);
		SplineMeshComponent->SetForwardAxis(ESplineMeshAxis::X);
		SplineMeshComponent->SetStartScale(SplineSegmentSize * SplinePoints[i].Scale);
		SplineMeshComponent->SetEndScale(SplineSegmentSize * SplinePoints[i+1].Scale);
		SplineMeshComponent->SetBoundaryMax(1.0f);
		SplineMeshComponent->RegisterComponent();

		SplineMeshes.Add(SplineMeshComponent);

		const auto StartPosition = SplinePoints[i].Location;
		const auto StartTangent = Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const auto EndPosition = SplinePoints[i + 1].Location;
		const auto EndTangent = Spline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		SplineMeshComponent->SetStartAndEnd(StartPosition, StartTangent, EndPosition, EndTangent);

		// LogScreenLogF("P: %.3f, %.3f, %.3f", StartPosition.X, StartPosition.Y, StartPosition.Z);
	}
}

void AVeDrawableSpline::ClearSpline() {
	for (USplineMeshComponent* SplineMesh : SplineMeshes) {
		SplineMesh->DestroyComponent();
	}
	SplineMeshes.Empty();
	Spline->ClearSplinePoints();
}

FVector AVeDrawableSpline::GetFirstPointLocation() {
	if (SplinePoints.Num() > 0) {
		return SplinePoints[0].Location;
	}
	return FVector::ZeroVector;
}

FVector AVeDrawableSpline::GetLastPointLocation() {
	if (SplinePoints.Num() > 0) {
		const int32 LastSplinePointIndex = SplinePoints.Num() - 1;
		return SplinePoints[LastSplinePointIndex].Location;
	}
	return FVector::ZeroVector;
}
