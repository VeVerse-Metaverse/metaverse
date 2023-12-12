// Fill out your copyright notice in the Description page of Project Settings.
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GizmoBase.h"
#include "RotationGizmo.generated.h"

/**
 *
 */
UCLASS()
class VEGAMEFRAMEWORK_API ARotationGizmo : public AGizmoBase
{
	GENERATED_BODY()

public:

	ARotationGizmo();

	virtual ETransformType GetGizmoType() const final { return ETransformType::Rotation; }

	// Returns a Snapped Transform based on how much has been accumulated, the Delta Transform and Snapping Value
	virtual FTransform GetSnappedTransform(FTransform& outCurrentAccumulatedTransform
		, const FTransform& DeltaTransform
		, ETransformationDomain Domain
		, float SnappingValue) const override;

protected:

	//Rotation has a special way of Handling the Scene Scaling and that is, that its AXis need to face the Camera as well!
	virtual FVector CalculateGizmoSceneScale(const FVector& ReferenceLocation, const FVector& ReferenceLookDirection
		, float FieldOfView) override;

	virtual FTransform GetDeltaTransform(const FVector& LookingVector
		, const FVector& RayStartPoint
		, const FVector& RayEndPoint
		,  ETransformationDomain Domain) override;

private:

	FVector PreviousRotationViewScale;
};
