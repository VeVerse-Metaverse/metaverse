// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "InteractiveCharacter.h"
#include "GameFramework/Character.h"

#include "InteractiveVRCharacter.generated.h"

/** Base class for interactive VR characters. Spawns motion controllers, controls teleportation. */
UCLASS(Blueprintable, BlueprintType)
class VEINTERACTION_API AInteractiveVRCharacter : public AInteractiveCharacter {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AInteractiveVRCharacter(const FObjectInitializer& ObjectInitializer);

#pragma region Game Framework
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion

#pragma region VR Components
	/** VR camera attachment point, controlled by HMD. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* VRCameraOrigin;

	/** VR camera to render scene. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* VRCamera;

	/** Set to true when we have active tracking from the HMD. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bVRCameraTrackingActive = true;

protected:
	UPROPERTY()
	class APlayerCameraManager* PlayerCameraManager;

public:
	/** Returns cached player camera manager. */
	UFUNCTION(BlueprintGetter)
	class APlayerCameraManager* GetPlayerCameraManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VRTemplate|Variables", meta = (AllowPrivateAccess = "true"))
	float DefaultPlayerHeight = 0.01f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRTemplate|Variables", meta = (AllowPrivateAccess = "true"))
	bool bUseControllerRollToRotate = false;
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

	UPROPERTY()
	float TeleportLeftDirectionAxisX;
	UPROPERTY()
	float TeleportLeftDirectionAxisY;
	UFUNCTION()
	void UpdateTeleportLeftRotation();

	UPROPERTY()
	float TeleportRightDirectionAxisX;
	UPROPERTY()
	float TeleportRightDirectionAxisY;
	UFUNCTION()
	void UpdateTeleportRightRotation();
	
	/** Find teleport rotation from input rotation depending on the thumb dead zone value. */
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

protected:
	/** Fade out before teleportation. */
	void TeleportFadeOut();

	/** Fade in after teleportation. */
	void TeleportFadeIn();

#pragma endregion

#pragma region Network Motion Controller Transform Updates
	/** Replicated camera transform. */
	UPROPERTY(ReplicatedUsing=OnRep_CameraTransform)
	FTransform CameraTransform;

	/** Updates camera transform at clients. */
	UFUNCTION()
	void OnRep_CameraTransform() const;

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
	void Client_SendTransforms();

	/** Updates the current motion controller transforms at the server. */
	UFUNCTION(Server, Reliable)
	void Server_UpdateTransforms(FTransform InCameraTransform, FTransform InLeftHandTransform, FTransform InRightHandTransform);

	/** Updates the current motion controller transforms at the server. */
	UFUNCTION(Server, Reliable)
	void Server_UpdateTrackingStatus(bool bTrackingActive);

	/** Do the teleportation at the server. */
	UFUNCTION(Server, Reliable)
	void Server_Teleport(FVector NewLocation, FRotator NewRotation);
#pragma endregion

#pragma region Input Proxy
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

	UFUNCTION()
	void SetTeleportRightDirectionX(float Value);
	UFUNCTION()
	void SetTeleportRightDirectionY(float Value);

	UFUNCTION()
	void SetTeleportLeftDirectionX(float Value);
	UFUNCTION()
    void SetTeleportLeftDirectionY(float Value);	
#pragma endregion

};
