// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Components/InteractiveControlComponent.h"
#include "GameFramework/Actor.h"
#include "InteractiveMotionController.generated.h"

UCLASS(Blueprintable, BlueprintType)
class VEINTERACTION_API AInteractiveMotionController : public AActor {
	GENERATED_BODY()

public:
	AInteractiveMotionController();

#pragma region Components
	/** Root scene component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* RootSceneComponent;

	/** Motion controller component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UMotionControllerComponent* MotionController;
	FORCEINLINE class UMotionControllerComponent* GetMotionController() const { return MotionController; }

	/** Used to start point for teleport casts. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UArrowComponent* TeleportArcDirection;

	/** Teleport arc spline. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USplineComponent* TeleportArcSpline;

	/** Teleport arc spline mesh segments. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class USplineMeshComponent*> SplineMeshes;

	/** Mesh used for teleport arc segments. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMesh* TeleportArcSplineMesh;

	/** Material used for teleport arc segments. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UMaterial* TeleportArcSplineMaterial;

	/** Teleport arc end point, sphere in the center of the cylinder. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* TeleportArcEndPoint;

	/** Teleport arc position marker, cylinder arrow. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* TeleportTargetPositionMarker;

	/** Teleport arc direction marker, cylinder. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* TeleportTargetDirectionMarker;

	/** Controller overlap trigger. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* OverlapTrigger;

	/** Physics handle for object attachment. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPhysicsHandleComponent* PhysicsHandle;
	
	/** Interactive control component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UInteractiveControlComponent* InteractiveControl;
#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator InitialControllerRotation;
	FORCEINLINE FRotator GetInitialControllerRotation() const { return InitialControllerRotation; }

	/** Flag indicating that teleport visuals are active. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsTeleportActive;
	FORCEINLINE bool GetIsTeleportActive() const { return bIsTeleportActive; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsLastFrameValidTeleportDestination;

	/** Do we have a valid teleport destination at the navmesh. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsValidTeleportDestination;
	FORCEINLINE bool GetIsValidTeleportDestination() const { return bIsValidTeleportDestination; }

	/** Teleport destination location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector TeleportDestination;

	/** Teleport destination rotation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator TeleportRotation;

	/** Teleport arc trace launch velocity. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TeleportLaunchVelocity;

	/** Motion controller haptic feedback effect. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UHapticFeedbackEffect_Base* VRHapticFeedbackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugDrawSphere;

	/** Motion controller motion source. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MotionSource;

	/** Motion source setter. */
	UFUNCTION(BlueprintSetter)
	void SetMotionSource(FName InMotionSource);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintGetter)
	FTransform GetMotionControllerTransform() const;

	UFUNCTION(BlueprintSetter)
	void SetMotionControllerTransform(FTransform InTransform) const;

	UFUNCTION(BlueprintSetter)
	void SetTeleportRotation(FRotator InRotation);

	UFUNCTION(BlueprintCallable)
    void HapticFeedback(float Intensity);

	/** Call to initiate teleportation enabling components visibility. */
	UFUNCTION(BlueprintCallable)
    void EnableTeleportationVisuals();

	/** Call to finish teleportation disabling components visibility. */
	UFUNCTION(BlueprintCallable)
    void DisableTeleportationVisuals();
	
	/** Returns the teleport destination point location and rotation. */
	UFUNCTION(BlueprintCallable)
    void GetTeleportDestination(FVector& OutLocation, FRotator& OutRotation) const;

protected:
	/** Trace teleport destination by predicting projectile movement in the arc. */
	bool TraceTeleportDestination(TArray<FVector>& TracePoints, FVector& NavMeshLocation, FVector& TraceLocation);

	/** Updates the teleport arc end point. */
	void UpdateTeleportArcEndPoint(bool bHaveValidLocation, FVector EndPointLocation) const;

	/** Updates the teleport arc spline points and meshes. */
	void UpdateTeleportArcSpline(bool bHaveValidLocation, TArray<FVector> SplinePoints);

	/** Clears the teleport arc spline points and meshes. */
	void ClearTeleportArcSpline();

};
