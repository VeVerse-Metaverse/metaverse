// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "CoreMinimal.h"
#include "InspectInertiaManager.h"
#include "GameFramework/Actor.h"
#include "Inspector.generated.h"

class UArrowComponent;
class UDirectionalLightComponent;
class USpotLightComponent;

UCLASS(BlueprintType, Blueprintable)
class VEGAMEFRAMEWORK_API AInspector final : public AActor {
	GENERATED_BODY()

public:
	AInspector();

#pragma region Orbit

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInspectInertiaManager> OrbitXInertia;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInspectInertiaManager> OrbitYInertia;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInspectInertiaManager> PanXInertia;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInspectInertiaManager> PanYInertia;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInspectInertiaManager> ZoomInertia;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector OrbitAngles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Yaw = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Pitch = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OrbitSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PanSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LookDistance = FVector(100.0f, 0.0f, 0.0f);

	/** Call when starting orbit around the inspected actor, e.g. on mouse down. */
	UFUNCTION(BlueprintCallable)
	void OrbitBegin();

	/** Call each frame while orbiting, e.g. on mouse move. */
	UFUNCTION(BlueprintCallable)
	void OrbitUpdate(float DeltaX, float DeltaY);

	/** Call when finished orbiting around the inspected actor, e.g. on mouse up. */
	UFUNCTION(BlueprintCallable)
	void OrbitEnd();

	FVector GetInspectedActorOrigin() const;

	UFUNCTION(BlueprintCallable)
	void PanBegin();

	UFUNCTION(BlueprintCallable)
	void PanUpdate(float DeltaX, float DeltaY);

	UFUNCTION(BlueprintCallable)
	void PanEnd();

#pragma endregion

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ZoomSpeed = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> InspectedActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector DefaultLocation = {500.0f, 0.0f, 0.0f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator DefaultRotation = {0.0f, 180.0f, 0.0f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector PanOffset = {0.0f, 0.0f, 0.0f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector FocusPoint = {0.0f, 0.0f, 0.0f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsPanning = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UArrowComponent> OrbitXComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UArrowComponent> OrbitYComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UDirectionalLightComponent> AmbientLightComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USpotLightComponent> DrawLightComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USpotLightComponent> FillLightComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USpotLightComponent> BackLightComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<UPrimitiveComponent*, FLightingChannels> InspectedActorLightingChannels;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetupInspectedActorLightingChannels();
	void ResetInspectedActorLightingChannels();

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ResetView();

	UFUNCTION(BlueprintNativeEvent, meta=(DisplayName="Pan"))
	bool BP_Pan();
	bool BP_Pan_Implementation() { return false; }

	UFUNCTION(BlueprintCallable)
	void SetInspectedActor(AActor* InInspectedActor);

	UFUNCTION(BlueprintCallable)
	void CaptureScene() const;

	void Zoom(float ZoomValue) const;
};
