// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "InteractionNames.h"
#include "GameFramework/Pawn.h"
#include "InteractiveVRPawn.generated.h"

class USphereComponent;
class UInteractiveControlComponent;
UCLASS()
class VEINTERACTION_API AInteractiveVRPawn : public APawn {
	GENERATED_BODY()

public:
	AInteractiveVRPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Intercast)
	UInteractiveControlComponent* InteractiveControl;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category=Intercast)
	USphereComponent* CollisionComponent;

#pragma region Game Framework
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion

#pragma region VR Components
public:
	/** VR camera attachment point, controlled by HMD. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* VRCameraOrigin;

	/** VR camera to render scene. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* VRCamera;

	/** Returns cached player camera manager. */
	UFUNCTION(BlueprintGetter)
	class APlayerCameraManager* GetPlayerCameraManager();

protected:
	/** Cached player camera manager. */
	UPROPERTY()
	class APlayerCameraManager* PlayerCameraManager;
#pragma endregion

#pragma region Overlap Proxy
	UFUNCTION()
	void OnPawnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
							const FHitResult& SweepResult);

	UFUNCTION()
	void OnPawnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion

#pragma region Motion Controllers
private:
	UFUNCTION()
	void InitializeMotionControllersSource();

	/** Flag that shows if Motion Controllers are initialized */
	UPROPERTY()
	bool bIsMotionControllersInitialized;

	/** Delay after that it is necessary to update Motion Controllers source */
	UPROPERTY()
	float MotionControllersInitializingDelay;

public:
	/** Left hand motion controller. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	class AInteractiveMotionController* LeftMotionController;

	/** Right hand motion controller. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	class AInteractiveMotionController* RightMotionController;

	/** Class to use to spawn the left motion controller. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AInteractiveMotionController> LeftMotionControllerClass;

	/** Class to use to spawn the right motion controller. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AInteractiveMotionController> RightMotionControllerClass;
#pragma endregion

#pragma region Network Motion Controller Transform Updates
	/** Replicated left motion controller transform. */
	UPROPERTY(ReplicatedUsing=OnRep_LeftMotionControllerTransform)
	FTransform LeftMotionControllerTransform;

	/** Updates left motion controller transform at clients. */
	UFUNCTION()
	void OnRep_LeftMotionControllerTransform() const;

	/** Replicated right motion controller transform. */
	UPROPERTY(ReplicatedUsing=OnRep_RightMotionControllerTransform)
	FTransform RightMotionControllerTransform;

	/** Updates left motion controller transform at clients. */
	UFUNCTION()
	void OnRep_RightMotionControllerTransform() const;

	/** Handles sending motion controller transform data to the server. */
	FTimerHandle MotionControllerTransformUpdateTimerHandle;

	/** How often to send the updated motion controller transforms to the server. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MotionControllerTransformUpdateRate = 0.1f;

	/** Sends current motion controller transforms to the server. */
	UFUNCTION()
	void Client_SendMotionControllerTransforms();

	/** Updates the current motion controller transforms at the server. */
	UFUNCTION(Server, Reliable)
	void Server_UpdateMotionControllerTransforms(FTransform InLeftHandTransform, FTransform InRightHandTransform);

#pragma endregion

#pragma region Teleportation
	/** Is in the teleportation state (faded out and not yet faded in). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsTeleporting;

	/** Fade out duration before teleportation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TeleportFadeOutDuration;

	/** Fade in duration after teleportation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TeleportFadeInDuration;

	/** Color to fade out to and fade in from. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor TeleportFadeColor;

	/** Handles teleportation fade out and teleportation execution. */
	FTimerHandle TeleportFadeTimerHandle;

	/** Dead zone in the middle of the thumb pad or thumb stick that will block rotation during teleportation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ThumbDeadZone;

	/** Determines the rotation to teleport the pawn with, will keep the rotation or get the input rotation depending on the thumb dead zone value. */
	UFUNCTION()
	FRotator GetRotationFromInput(float UpAxis, float RightAxis) const;

	/** Called before teleportation begin. */
	UFUNCTION(BlueprintImplementableEvent)
	void OnBeforeTeleport();

	/** Called when teleportation finished. */
	UFUNCTION(BlueprintImplementableEvent)
	void OnAfterTeleport();

	/** Initiates the teleportation process. */
	UFUNCTION()
	void InitiateTeleportation(class AInteractiveMotionController* MotionController);

	/** Request actual teleportation. */
	UFUNCTION()
	void FinishTeleportation(class AInteractiveMotionController* MotionController);

	/** Do the teleportation at the server. */
	UFUNCTION(Server, Reliable)
	void Server_Teleport(FVector NewLocation, FRotator NewRotation);

protected:
	/** Fade out before teleportation. */
	void TeleportFadeOut();

	/** Fade in after teleportation. */
	void TeleportFadeIn();

#pragma endregion

#pragma region Input Bindings

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ActivateBinding = TEXT("InteractActivate"); // NAME_InteractActivate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UseBinding = NAME_InteractUse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName GrabBinding = NAME_InteractGrab;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName InputAxisXBinding = NAME_InteractAxisX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName InputAxisYBinding = NAME_InteractAxisY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName InputAxisZBinding = NAME_InteractAxisZ;

#pragma endregion

#pragma region InputProxy
	UFUNCTION()
	virtual void OnInputActionActivateBegin();

	UFUNCTION()
	virtual void OnInputActionActivateEnd();

	UFUNCTION()
	virtual void OnInputActionUseBegin();

	UFUNCTION()
	virtual void OnInputActionUseEnd();

	UFUNCTION()
	virtual void OnInputActionGrabBegin();

	UFUNCTION()
	virtual void OnInputActionGrabEnd();

	UFUNCTION()
	virtual void OnInputAxisX(float Value);

	UFUNCTION()
	virtual void OnInputAxisY(float Value);

	UFUNCTION()
	virtual void OnInputAxisZ(float Value);

	UFUNCTION()
	void OnInputActionResetVR();

	UFUNCTION()
	void OnInputActionTeleportRightBegin();
	UFUNCTION()
	void OnInputActionTeleportRightEnd();
	UFUNCTION()
	void OnInputActionTeleportLeftBegin();
	UFUNCTION()
	void OnInputActionTeleportLeftEnd();

	UFUNCTION()
	void OnInputActionActivateRightBegin();
	UFUNCTION()
	void OnInputActionActivateRightEnd();
	UFUNCTION()
	void OnInputActionActivateLeftBegin();
	UFUNCTION()
	void OnInputActionActivateLeftEnd();

	UFUNCTION()
	void OnInputActionGrabRightBegin();
	UFUNCTION()
	void OnInputActionGrabRightEnd();
	UFUNCTION()
	void OnInputActionGrabLeftBegin();
	UFUNCTION()
	void OnInputActionGrabLeftEnd();

	UFUNCTION()
	void OnInputActionUseRightBegin();
	UFUNCTION()
	void OnInputActionUseRightEnd();
	UFUNCTION()
	void OnInputActionUseLeftBegin();
	UFUNCTION()
	void OnInputActionUseLeftEnd();

	UFUNCTION()
	void OnInputAxisXRight(float Value);
	UFUNCTION()
	void OnInputAxisXLeft(float Value);

	UFUNCTION()
	void OnInputAxisYRight(float Value);
	UFUNCTION()
	void OnInputAxisYLeft(float Value);

	UFUNCTION()
	void OnInputAxisZRight(float Value);
	UFUNCTION()
	void OnInputAxisZLeft(float Value);
#pragma endregion

};
