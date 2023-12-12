// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VeDrawableSpline.generated.h"

USTRUCT(BlueprintType)
struct FVeDrawableSplinePoint {
	GENERATED_BODY()
	
	FVeDrawableSplinePoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Scale;
};

UCLASS(BlueprintType)
class VEGAME_API AVeDrawableSpline : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVeDrawableSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Spline. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USplineComponent* Spline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVeDrawableSplinePoint> SplinePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SplineSegmentSize = FVector2D(1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SplineSegmentScale = FVector(1.0f, 1.0f, 1.0f);

	/** Spline mesh segments. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USplineMeshComponent*> SplineMeshes;

	/** Mesh used for spline segments. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMesh* SplineSegmentMesh;

	/** Material used for teleport arc segments. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterial* SplineSegmentMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RootSceneComponent;

	/** Starts a new spline. */
	UFUNCTION(BlueprintCallable)
	void StartDrawing();

	/** Finishes the spline. */
	UFUNCTION(BlueprintCallable)
	void FinishDrawing();

	UFUNCTION(BlueprintCallable)
	void RemoveFirstPoint(bool bDestroyIfEmpty);

	/** Adds a new spline point. */
	UFUNCTION(BlueprintCallable)
	void AddSplinePoint(FVeDrawableSplinePoint InSplinePoint);

	/** Updates the spline points and meshes. */
	UFUNCTION(BlueprintCallable)
	void UpdateSpline();

	/** Clears the spline points and meshes. */
	UFUNCTION(BlueprintCallable)
	void ClearSpline();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetFirstPointLocation();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetLastPointLocation();
};
