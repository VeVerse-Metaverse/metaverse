// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "Pawns/InteractiveVRCharacter.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Interaction.h"
#include "InteractionMacros.h"
#include "MotionControllerComponent.h"
#include "TimerManager.h"
#include "VeShared.h"
#include "XRMotionControllerBase.h"
#include "Camera/CameraComponent.h"
// #include "Components/InteractiveControlComponent.h"
#include "Components/InteractionSubjectComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Pawns/InteractiveMotionController.h"
#include "InteractionNames.h"


#pragma region Game Framework
AInteractiveVRCharacter::AInteractiveVRCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRCameraOrigin = CreateDefaultSubobject<USceneComponent>(NAME_VROrigin);
	VRCamera = CreateDefaultSubobject<UCameraComponent>(NAME_Camera);

	VRCameraOrigin->SetupAttachment(RootComponent);
	VRCamera->SetupAttachment(VRCameraOrigin);

	TeleportFadeOutDuration = 0.1f;
	TeleportFadeInDuration = 0.2f;
	TeleportFadeColor = FColor::Black;

	bIsTeleporting = false;

	ThumbDeadZone = 0.7f;

	const auto MovementComponent = ACharacter::GetMovementComponent();
	ensure(MovementComponent);
	if (MovementComponent) {
		MovementComponent->SetActive(false);
	}

	// ensure(InteractiveControl);
	// if (InteractiveControl) {
		// InteractiveControl->SetControlMode(EInteractionControlMode::PawnVR);
	// }

	if (InteractionSubjectComponent) {
		InteractionSubjectComponent->SetControlMode(EInteractionControlMode::PawnVR);
	}

	LeftMotionControllerClass = AInteractiveMotionController::StaticClass();
	RightMotionControllerClass = AInteractiveMotionController::StaticClass();

	// For once updating Motion Controllers source
	bIsMotionControllersInitialized = false;
	MotionControllersInitializingDelay = 0.05f;
}

void AInteractiveVRCharacter::BeginPlay() {
	Super::BeginPlay();

	SetReplicates(true);
	// VRCamera->SetIsReplicated(false);
	// VRCameraOrigin->SetIsReplicated(false);

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	const auto SpawnTransform = FTransform::Identity;
	const FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	const auto World = GetWorld();
	CHECK_RETURN(World);

	if (ACTOR_IS_AUTHORITY(this) || ACTOR_IS_STANDALONE(this)) {
		CHECK_RETURN(LeftMotionControllerClass);
		CHECK_RETURN(RightMotionControllerClass);

		if (!LeftMotionController) {
			LeftMotionController = World->SpawnActorDeferred<AInteractiveMotionController>(LeftMotionControllerClass, SpawnTransform, this, nullptr,
																						   ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			if (LeftMotionController) {
				LeftMotionController->SetMotionSource(FXRMotionControllerBase::LeftHandSourceId);
				LeftMotionController->FinishSpawning(SpawnTransform);
				LeftMotionController->SetOwner(this);
				LeftMotionController->AttachToComponent(VRCameraOrigin, AttachRules);
			}
		}

		if (!RightMotionController) {
			RightMotionController = World->SpawnActorDeferred<AInteractiveMotionController>(RightMotionControllerClass, SpawnTransform, this, nullptr,
																							ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			if (RightMotionController) {
				RightMotionController->SetMotionSource(FXRMotionControllerBase::RightHandSourceId);
				RightMotionController->FinishSpawning(SpawnTransform);
				RightMotionController->SetOwner(this);
				RightMotionController->AttachToComponent(VRCameraOrigin, AttachRules);
			}
		}
	}

	// Start the update timer to send updated motion controller transforms to the server.
	if (IsLocallyControlled() && !ACTOR_IS_STANDALONE(this)) {
		FTimerDelegate MotionControllerTransformUpdateTimerDelegate;
		MotionControllerTransformUpdateTimerDelegate.BindUObject(this, &AInteractiveVRCharacter::Client_SendTransforms);
		World->GetTimerManager().SetTimer(MotionControllerTransformUpdateTimerHandle, MotionControllerTransformUpdateTimerDelegate, MotionControllerTransformUpdateRate, true);
	}
}

void AInteractiveVRCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (ACTOR_IS_AUTHORITY(this) || ACTOR_IS_STANDALONE(this)) {
		if (RightMotionController != nullptr) {
			RightMotionController->Destroy(true);
		}

		if (LeftMotionController != nullptr) {
			LeftMotionController->Destroy(true);
		}
	}

	if (IsLocallyControlled() && !ACTOR_IS_STANDALONE(this)) {
		if (MotionControllerTransformUpdateTimerHandle.IsValid()) {
			GetWorld()->GetTimerManager().ClearTimer(MotionControllerTransformUpdateTimerHandle);
		}
	}
}

void AInteractiveVRCharacter::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

	if (InputComponent == nullptr) {
		return;
	}

	if (IsLocallyControlled()) {
		UpdateTeleportLeftRotation();
		UpdateTeleportRightRotation();

		// todo: Optimize Motion Controllers Source with timer delegate. 
		// Once updating Motion Controllers source
		if (!bIsMotionControllersInitialized) {
			MotionControllersInitializingDelay -= DeltaTime;
			if (MotionControllersInitializingDelay <= 0.0f) {
				InitializeMotionControllersSource();
			}
		}
	}
}

void AInteractiveVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(NAME_InteractResetVR, IE_Pressed, this, &AInteractiveVRCharacter::OnInputActionResetVR);

	PlayerInputComponent->BindAction(NAME_InteractTeleportLeft, IE_Pressed, this, &AInteractiveVRCharacter::OnInputActionTeleportLeftBegin);
	PlayerInputComponent->BindAction(NAME_InteractTeleportLeft, IE_Released, this, &AInteractiveVRCharacter::OnInputActionTeleportLeftEnd);
	PlayerInputComponent->BindAction(NAME_InteractTeleportRight, IE_Pressed, this, &AInteractiveVRCharacter::OnInputActionTeleportRightBegin);
	PlayerInputComponent->BindAction(NAME_InteractTeleportRight, IE_Released, this, &AInteractiveVRCharacter::OnInputActionTeleportRightEnd);

	PlayerInputComponent->BindAction(NAME_InteractActivateLeft, IE_Pressed, this, &AInteractiveVRCharacter::OnInputActionActivateLeftBegin);
	PlayerInputComponent->BindAction(NAME_InteractActivateLeft, IE_Released, this, &AInteractiveVRCharacter::OnInputActionActivateLeftEnd);
	PlayerInputComponent->BindAction(NAME_InteractActivateRight, IE_Pressed, this, &AInteractiveVRCharacter::OnInputActionActivateRightBegin);
	PlayerInputComponent->BindAction(NAME_InteractActivateRight, IE_Released, this, &AInteractiveVRCharacter::OnInputActionActivateRightEnd);

	PlayerInputComponent->BindAction(NAME_InteractGrabLeft, IE_Pressed, this, &AInteractiveVRCharacter::OnInputActionGrabLeftBegin);
	PlayerInputComponent->BindAction(NAME_InteractGrabLeft, IE_Released, this, &AInteractiveVRCharacter::OnInputActionGrabLeftEnd);
	PlayerInputComponent->BindAction(NAME_InteractGrabRight, IE_Pressed, this, &AInteractiveVRCharacter::OnInputActionGrabRightBegin);
	PlayerInputComponent->BindAction(NAME_InteractGrabRight, IE_Released, this, &AInteractiveVRCharacter::OnInputActionGrabRightEnd);

	PlayerInputComponent->BindAction(NAME_InteractUseLeft, IE_Pressed, this, &AInteractiveVRCharacter::OnInputActionUseLeftBegin);
	PlayerInputComponent->BindAction(NAME_InteractUseLeft, IE_Released, this, &AInteractiveVRCharacter::OnInputActionUseLeftEnd);
	PlayerInputComponent->BindAction(NAME_InteractUseRight, IE_Pressed, this, &AInteractiveVRCharacter::OnInputActionUseRightBegin);
	PlayerInputComponent->BindAction(NAME_InteractUseRight, IE_Released, this, &AInteractiveVRCharacter::OnInputActionUseRightEnd);

	PlayerInputComponent->BindAxis(NAME_InteractTeleportLeftDirectionAxisX, this, &AInteractiveVRCharacter::SetTeleportLeftDirectionX);
	PlayerInputComponent->BindAxis(NAME_InteractTeleportLeftDirectionAxisY, this, &AInteractiveVRCharacter::SetTeleportLeftDirectionY);
	PlayerInputComponent->BindAxis(NAME_InteractTeleportRightDirectionAxisX, this, &AInteractiveVRCharacter::SetTeleportRightDirectionX);
	PlayerInputComponent->BindAxis(NAME_InteractTeleportRightDirectionAxisY, this, &AInteractiveVRCharacter::SetTeleportRightDirectionY);
}

void AInteractiveVRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AInteractiveVRCharacter, CameraTransform, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AInteractiveVRCharacter, LeftMotionControllerTransform, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AInteractiveVRCharacter, RightMotionControllerTransform, COND_SkipOwner);

	DOREPLIFETIME(AInteractiveVRCharacter, bVRCameraTrackingActive);

	DOREPLIFETIME(AInteractiveVRCharacter, LeftMotionController);
	DOREPLIFETIME(AInteractiveVRCharacter, RightMotionController);
}

#pragma endregion

void AInteractiveVRCharacter::Client_SendTransforms() {
	if (IsLocallyControlled()) {
		// Send controller transforms to the server.
		if (ACTOR_IS_STANDALONE(this)) {
			bVRCameraTrackingActive = true;
		} else {
			const FTransform LastCameraTransform = CameraTransform;
			const FTransform LastLeftMotionControllerTransform = LeftMotionControllerTransform;
			const FTransform LastRightMotionControllerTransform = RightMotionControllerTransform;

			CameraTransform = VRCamera ? VRCamera->GetComponentTransform() : FTransform::Identity;
			LeftMotionControllerTransform = LeftMotionController ? LeftMotionController->GetMotionControllerTransform() : FTransform::Identity;
			RightMotionControllerTransform = RightMotionController ? RightMotionController->GetMotionControllerTransform() : FTransform::Identity;

			if (!LastCameraTransform.Equals(CameraTransform)
				|| !LastLeftMotionControllerTransform.Equals(LeftMotionControllerTransform)
				|| !LastRightMotionControllerTransform.Equals(RightMotionControllerTransform)) {
				Server_UpdateTransforms(CameraTransform, LeftMotionControllerTransform, RightMotionControllerTransform);
				Server_UpdateTrackingStatus(true);
			} else {
				Server_UpdateTrackingStatus(false);
			}
		}
	}
}

void AInteractiveVRCharacter::Server_UpdateTrackingStatus_Implementation(const bool bTrackingActive) {
	bVRCameraTrackingActive = bTrackingActive;
}

void AInteractiveVRCharacter::UpdateTeleportLeftRotation() {
	CHECK_RETURN(LeftMotionController);

	if (LeftMotionController->GetIsTeleportActive()) {
		const FRotator TeleportRotation = GetRotationFromInput(TeleportLeftDirectionAxisY, TeleportLeftDirectionAxisX);
		LeftMotionController->SetTeleportRotation(TeleportRotation);
	}
}

void AInteractiveVRCharacter::UpdateTeleportRightRotation() {
	CHECK_RETURN(RightMotionController);

	if (RightMotionController->GetIsTeleportActive()) {
		const FRotator TeleportRotation = GetRotationFromInput(TeleportRightDirectionAxisY, TeleportRightDirectionAxisX);
		RightMotionController->SetTeleportRotation(TeleportRotation);
	}
}

FRotator AInteractiveVRCharacter::GetRotationFromInput(const float UpAxis, const float RightAxis) const {
	FRotator OutRotation;
	// Check whether thumb is near center (to ignore rotation overrides)
	const bool bUsePawnRotation = FMath::Abs(UpAxis) + FMath::Abs(RightAxis) < ThumbDeadZone;
	if (bUsePawnRotation) {
		OutRotation = GetActorRotation();
	} else {
		// Rotate input X+Y to always point forward relative to the current pawn rotation.
		FVector InputVectorDirection = {UpAxis, RightAxis, 0.0f};
		InputVectorDirection.Normalize(0.0001f);
		FRotator ActorRotation;
		ActorRotation.Yaw = GetActorRotation().Yaw;
		OutRotation = ActorRotation.RotateVector(InputVectorDirection).ToOrientationRotator();
	}
	return OutRotation;
}

void AInteractiveVRCharacter::InitiateTeleportation(AInteractiveMotionController* MotionController) {
	if (!bIsTeleporting) {
		OnBeforeTeleport();

		if (MotionController->GetIsValidTeleportDestination()) {
			bIsTeleporting = true;

			TeleportFadeOut();

			// Fade out and only then teleport with the method called by the timer.
			FTimerDelegate TeleportFadeTimerDelegate;
			TeleportFadeTimerDelegate.BindUObject(this, &AInteractiveVRCharacter::FinishTeleportation, MotionController);
			GetWorldTimerManager().SetTimer(TeleportFadeTimerHandle, TeleportFadeTimerDelegate, TeleportFadeOutDuration, false);
		} else {
			MotionController->DisableTeleportationVisuals();
		}
	}
}

void AInteractiveVRCharacter::OnRep_CameraTransform() const {
	if (!IsLocallyControlled()) {
		if (VRCamera != nullptr) {
			VRCamera->SetWorldTransform(CameraTransform);
		}
	}
}

void AInteractiveVRCharacter::OnRep_LeftMotionControllerTransform() const {
	if (!IsLocallyControlled()) {
		if (LeftMotionController != nullptr) {
			LeftMotionController->SetMotionControllerTransform(LeftMotionControllerTransform);
		}
	}
}

void AInteractiveVRCharacter::OnRep_RightMotionControllerTransform() const {
	if (!IsLocallyControlled()) {
		if (RightMotionController != nullptr) {
			RightMotionController->SetMotionControllerTransform(RightMotionControllerTransform);
		}
	}
}

void AInteractiveVRCharacter::Server_UpdateTransforms_Implementation(const FTransform InCameraTransform, const FTransform InLeftHandTransform,
																	 const FTransform InRightHandTransform) {
	CameraTransform = InCameraTransform;
	LeftMotionControllerTransform = InLeftHandTransform;
	RightMotionControllerTransform = InRightHandTransform;

	if (CameraTransform.Equals(FTransform::Identity)) {
		bVRCameraTrackingActive = false;
	} else {
		bVRCameraTrackingActive = true;
	}

	if (VRCamera && !InCameraTransform.Equals(FTransform::Identity)) {
		VRCamera->SetWorldTransform(InCameraTransform);
	}

	if (LeftMotionController) {
		LeftMotionController->SetMotionControllerTransform(InLeftHandTransform);
		LeftMotionController->SetMotionSource(FXRMotionControllerBase::LeftHandSourceId);
	}

	if (RightMotionController) {
		RightMotionController->SetMotionControllerTransform(InRightHandTransform);
		RightMotionController->SetMotionSource(FXRMotionControllerBase::RightHandSourceId);
	}
}

void AInteractiveVRCharacter::FinishTeleportation(AInteractiveMotionController* MotionController) {
	MotionController->DisableTeleportationVisuals();

	FVector DestinationLocation;
	FRotator DestinationRotation;
	MotionController->GetTeleportDestination(DestinationLocation, DestinationRotation);

	// // todo: Make Actor Rotation by using Control Rotation
	// float RotationYaw = DestinationRotation.Yaw;
	// if (RotationYaw < 0) {
	// 	RotationYaw += 360.f;
	// }
	//
	// UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(RotationYaw, EOrientPositionSelector::Orientation);
	Server_Teleport(DestinationLocation, DestinationRotation);

	TeleportFadeIn();

	GetWorldTimerManager().ClearTimer(TeleportFadeTimerHandle);
	bIsTeleporting = false;

	OnAfterTeleport();
}

void AInteractiveVRCharacter::Server_Teleport_Implementation(const FVector NewLocation, const FRotator NewRotation) {
	TeleportTo(NewLocation, NewRotation, false, true);
}

APlayerCameraManager* AInteractiveVRCharacter::GetPlayerCameraManager() {
	if (PlayerCameraManager == nullptr) {
		const APlayerController* PlayerController = Cast<APlayerController>(GetController());
		CHECK_RETURN_VALUE(PlayerController, PlayerCameraManager);

		PlayerCameraManager = PlayerController->PlayerCameraManager;
	}

	return PlayerCameraManager;
}

void AInteractiveVRCharacter::InitializeMotionControllersSource() {
	if (LeftMotionController) {
		LeftMotionController->SetMotionSource(FXRMotionControllerBase::LeftHandSourceId);
	}
	if (RightMotionController) {
		RightMotionController->SetMotionSource(FXRMotionControllerBase::RightHandSourceId);
	}
	bIsMotionControllersInitialized = true;
}

void AInteractiveVRCharacter::TeleportFadeIn() {
	const auto CameraManager = GetPlayerCameraManager();
	CHECK_RETURN(CameraManager);

	CameraManager->StartCameraFade(1.0f, 0.0f, TeleportFadeInDuration, TeleportFadeColor, false, false);
}

void AInteractiveVRCharacter::TeleportFadeOut() {
	const auto CameraManager = GetPlayerCameraManager();
	CHECK_RETURN(CameraManager);

	CameraManager->StartCameraFade(0.0f, 1.0f, TeleportFadeOutDuration, TeleportFadeColor, false, true);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void AInteractiveVRCharacter::OnInputActionResetVR() {
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AInteractiveVRCharacter::OnInputActionTeleportRightBegin() {
	RightMotionController->EnableTeleportationVisuals();
	LeftMotionController->DisableTeleportationVisuals();
}

void AInteractiveVRCharacter::OnInputActionTeleportRightEnd() {
	if (RightMotionController->GetIsTeleportActive()) {
		InitiateTeleportation(RightMotionController);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AInteractiveVRCharacter::OnInputActionTeleportLeftBegin() {
	RightMotionController->DisableTeleportationVisuals();
	LeftMotionController->EnableTeleportationVisuals();
}

void AInteractiveVRCharacter::OnInputActionTeleportLeftEnd() {
	if (LeftMotionController->GetIsTeleportActive()) {
		InitiateTeleportation(LeftMotionController);
	}
}

#pragma region Input Proxy

void AInteractiveVRCharacter::OnInputActionActivateRightBegin() {
	// if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionActivate, RightMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionActivateRightEnd() {
	// if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionActivate, RightMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionActivateLeftBegin() {
	// if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionActivate, LeftMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionActivateLeftEnd() {
	// if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionActivate, LeftMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionGrabRightBegin() {
	// if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionGrab, RightMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionGrabRightEnd() {
	// if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionGrab, RightMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionGrabLeftBegin() {
	// if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionGrab, LeftMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionGrabLeftEnd() {
	// if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionGrab, LeftMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionUseRightBegin() {
	// if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionUse, RightMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionUseRightEnd() {
	// if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionUse, RightMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionUseLeftBegin() {
	// if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionUse, LeftMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputActionUseLeftEnd() {
	// if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionUse, LeftMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputAxisXRight(const float Value) {
	// if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisX, Value, RightMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputAxisXLeft(const float Value) {
	// if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisX, Value, LeftMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputAxisYRight(const float Value) {
	// if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisY, Value, RightMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputAxisYLeft(const float Value) {
	// if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisY, Value, LeftMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputAxisZRight(const float Value) {
	// if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisZ, Value, RightMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::OnInputAxisZLeft(const float Value) {
	// if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	// InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisZ, Value, LeftMotionController->InteractiveControl);
}

void AInteractiveVRCharacter::SetTeleportLeftDirectionX(const float Value) {
	TeleportLeftDirectionAxisX = Value;
}

void AInteractiveVRCharacter::SetTeleportLeftDirectionY(const float Value) {
	TeleportLeftDirectionAxisY = Value;
}

void AInteractiveVRCharacter::SetTeleportRightDirectionX(const float Value) {
	TeleportRightDirectionAxisX = Value;
}

void AInteractiveVRCharacter::SetTeleportRightDirectionY(const float Value) {
	TeleportRightDirectionAxisY = Value;
}
#pragma endregion
