// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GizmoBase.h"
#include "TranslationGizmo.generated.h"

/**
 *
 */
UCLASS()
class VEGAMEFRAMEWORK_API ATranslationGizmo : public AGizmoBase
{
	GENERATED_BODY()

public:

	ATranslationGizmo();

	virtual ETransformType GetGizmoType() const final { return ETransformType::Translation; }

	virtual FTransform GetDeltaTransform(const FVector& LookingVector
		, const FVector& RayStartPoint
		, const FVector& RayEndPoint,  ETransformationDomain Domain) override;

	// Returns a Snapped Transform based on how much has been accumulated, the Delta Transform and Snapping Value
	virtual FTransform GetSnappedTransform(FTransform& outCurrentAccumulatedTransform
		, const FTransform& DeltaTransform
		, ETransformationDomain Domain
		, float SnappingValue) const override;

protected:

	// The Hit Box for the XY-Plane Translation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gizmo")
	class UBoxComponent* XY_PlaneBox;

	// The Hit Box for the YZ-Plane Translation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gizmo")
	class UBoxComponent* YZ_PlaneBox;

	// The Hit Box for the	XZ-Plane Translation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gizmo")
	class UBoxComponent* XZ_PlaneBox;

	// The Hit Box for the	XYZ Free Translation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gizmo")
	class USphereComponent* XYZ_Sphere;
};

