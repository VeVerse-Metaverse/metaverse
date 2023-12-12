// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "Pawns/InteractiveVRPawn.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Interaction.h"
#include "InteractionMacros.h"
#include "VeShared.h"
#include "XRMotionControllerBase.h"
#include "Camera/CameraComponent.h"
#include "Components/InteractiveControlComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Pawns/InteractiveMotionController.h"

// Sets default values
AInteractiveVRPawn::AInteractiveVRPawn(): Super() {
	PrimaryActorTick.bCanEverTick = true;

	InteractiveControl = CreateDefaultSubobject<UInteractiveControlComponent>("InteractionControl");

	VRCameraOrigin = CreateDefaultSubobject<USceneComponent>(NAME_VROrigin);
	VRCamera = CreateDefaultSubobject<UCameraComponent>(NAME_Camera);

	VRCameraOrigin->SetupAttachment(RootComponent);
	VRCamera->SetupAttachment(VRCameraOrigin);
	VRCamera->bLockToHmd = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("Sphere");
	CollisionComponent->SetupAttachment(VRCamera);

	TeleportFadeOutDuration = 0.1f;
	TeleportFadeInDuration = 0.2f;
	TeleportFadeColor = FColor::Black;

	bIsTeleporting = false;

	ThumbDeadZone = 0.5f;

	ensure(InteractiveControl);
	if (InteractiveControl) {
		InteractiveControl->SetControlMode(EInteractionControlMode::PawnVR);
	}

	LeftMotionControllerClass = AInteractiveMotionController::StaticClass();
	RightMotionControllerClass = AInteractiveMotionController::StaticClass();

	// For once updating Motion Controllers source
	bIsMotionControllersInitialized = false;
	MotionControllersInitializingDelay = 0.05f;
}

// Called when the game starts or when spawned
void AInteractiveVRPawn::BeginPlay() {
	Super::BeginPlay();

	SetReplicates(true);
	SetReplicateMovement(true);

	// Setup the interaction control component.
	if (InteractiveControl && (ACTOR_IS_AUTHORITY(this) || InteractiveControl->bProcessClientOverlap)) {
		// Pawn overlap.
		if (CollisionComponent) {
			CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveVRPawn::OnPawnOverlapBegin);
			CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AInteractiveVRPawn::OnPawnOverlapEnd);
		}
	}

#pragma region Initialize VR
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
		MotionControllerTransformUpdateTimerDelegate.BindUObject(this, &AInteractiveVRPawn::Client_SendMotionControllerTransforms);
		World->GetTimerManager().SetTimer(MotionControllerTransformUpdateTimerHandle, MotionControllerTransformUpdateTimerDelegate, MotionControllerTransformUpdateRate, true);
	}
#pragma endregion
}

void AInteractiveVRPawn::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	// Setup the interaction control component.
	if (InteractiveControl && (ACTOR_IS_AUTHORITY(this) || InteractiveControl->bProcessClientOverlap)) {
		// Pawn overlap.
		if (CollisionComponent) {
			CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AInteractiveVRPawn::OnPawnOverlapBegin);
			CollisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &AInteractiveVRPawn::OnPawnOverlapEnd);
		}
	}

#pragma region Cleanup VR
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
#pragma endregion
}

// Called every frame
void AInteractiveVRPawn::Tick(const float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

#pragma region Update VR
	if (InputComponent == nullptr) {
		return;
	}

	if (IsLocallyControlled()) {
		if (LeftMotionController && LeftMotionController->GetIsTeleportActive()) {
			const auto TeleportDirectionX = InputComponent->GetAxisValue(NAME_InteractTeleportLeftDirectionAxisX);
			const auto TeleportDirectionY = InputComponent->GetAxisValue(NAME_InteractTeleportLeftDirectionAxisY);
			const auto LeftMotionControllerTeleportRotation = GetRotationFromInput(TeleportDirectionY, TeleportDirectionX);
			LeftMotionController->SetTeleportRotation(LeftMotionControllerTeleportRotation);
		}
		if (RightMotionController && RightMotionController->GetIsTeleportActive()) {
			const auto TeleportDirectionX = InputComponent->GetAxisValue(NAME_InteractTeleportRightDirectionAxisX);
			const auto TeleportDirectionY = InputComponent->GetAxisValue(NAME_InteractTeleportRightDirectionAxisY);
			const auto RightMotionControllerTeleportRotation = GetRotationFromInput(TeleportDirectionY, TeleportDirectionX);
			RightMotionController->SetTeleportRotation(RightMotionControllerTeleportRotation);
		}

		// todo: Optimize Motion Controllers Source with timer delegate. 
		// Once updating Motion Controllers source
		if (!bIsMotionControllersInitialized) {
			MotionControllersInitializingDelay -= DeltaSeconds;
			if (MotionControllersInitializingDelay <= 0.0f) {
				InitializeMotionControllersSource();
			}
		}
	}
#pragma endregion
}

// Called to bind functionality to input
void AInteractiveVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent == nullptr) return;

#pragma region Common Input Bindings
	PlayerInputComponent->BindAction(ActivateBinding, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionActivateBegin);
	PlayerInputComponent->BindAction(ActivateBinding, IE_Released, this, &AInteractiveVRPawn::OnInputActionActivateEnd);
	PlayerInputComponent->BindAction(UseBinding, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionUseBegin);
	PlayerInputComponent->BindAction(UseBinding, IE_Released, this, &AInteractiveVRPawn::OnInputActionUseEnd);
	PlayerInputComponent->BindAction(GrabBinding, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionGrabBegin);
	PlayerInputComponent->BindAction(GrabBinding, IE_Released, this, &AInteractiveVRPawn::OnInputActionGrabEnd);
	PlayerInputComponent->BindAxis(InputAxisXBinding, this, &AInteractiveVRPawn::OnInputAxisX);
	PlayerInputComponent->BindAxis(InputAxisYBinding, this, &AInteractiveVRPawn::OnInputAxisY);
	PlayerInputComponent->BindAxis(InputAxisZBinding, this, &AInteractiveVRPawn::OnInputAxisZ);
#pragma endregion

#pragma region VR Input Bindings
	PlayerInputComponent->BindAction(NAME_InteractResetVR, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionResetVR);

	PlayerInputComponent->BindAction(NAME_InteractTeleportLeft, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionTeleportLeftBegin);
	PlayerInputComponent->BindAction(NAME_InteractTeleportLeft, IE_Released, this, &AInteractiveVRPawn::OnInputActionTeleportLeftEnd);
	PlayerInputComponent->BindAction(NAME_InteractTeleportRight, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionTeleportRightBegin);
	PlayerInputComponent->BindAction(NAME_InteractTeleportRight, IE_Released, this, &AInteractiveVRPawn::OnInputActionTeleportRightEnd);

	PlayerInputComponent->BindAction(NAME_InteractActivateLeft, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionActivateLeftBegin);
	PlayerInputComponent->BindAction(NAME_InteractActivateLeft, IE_Released, this, &AInteractiveVRPawn::OnInputActionActivateLeftEnd);
	PlayerInputComponent->BindAction(NAME_InteractActivateRight, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionActivateRightBegin);
	PlayerInputComponent->BindAction(NAME_InteractActivateRight, IE_Released, this, &AInteractiveVRPawn::OnInputActionActivateRightEnd);

	PlayerInputComponent->BindAction(NAME_InteractGrabLeft, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionGrabLeftBegin);
	PlayerInputComponent->BindAction(NAME_InteractGrabLeft, IE_Released, this, &AInteractiveVRPawn::OnInputActionGrabLeftEnd);
	PlayerInputComponent->BindAction(NAME_InteractGrabRight, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionGrabRightBegin);
	PlayerInputComponent->BindAction(NAME_InteractGrabRight, IE_Released, this, &AInteractiveVRPawn::OnInputActionGrabRightEnd);

	PlayerInputComponent->BindAction(NAME_InteractUseLeft, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionUseLeftBegin);
	PlayerInputComponent->BindAction(NAME_InteractUseLeft, IE_Released, this, &AInteractiveVRPawn::OnInputActionUseLeftEnd);
	PlayerInputComponent->BindAction(NAME_InteractUseRight, IE_Pressed, this, &AInteractiveVRPawn::OnInputActionUseRightBegin);
	PlayerInputComponent->BindAction(NAME_InteractUseRight, IE_Released, this, &AInteractiveVRPawn::OnInputActionUseRightEnd);
#pragma endregion
}

void AInteractiveVRPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractiveVRPawn, LeftMotionControllerTransform);
	DOREPLIFETIME(AInteractiveVRPawn, RightMotionControllerTransform);
	DOREPLIFETIME(AInteractiveVRPawn, LeftMotionController);
	DOREPLIFETIME(AInteractiveVRPawn, RightMotionController);
}

APlayerCameraManager* AInteractiveVRPawn::GetPlayerCameraManager() {
	if (PlayerCameraManager == nullptr) {
		const APlayerController* PlayerController = Cast<APlayerController>(GetController());
		CHECK_RETURN_VALUE(PlayerController, PlayerCameraManager);

		PlayerCameraManager = PlayerController->PlayerCameraManager;
	}

	return PlayerCameraManager;
}

void AInteractiveVRPawn::OnPawnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex,
											const bool bFromSweep,
											const FHitResult& SweepResult) {
	if (OtherActor == this) return;

	if (InteractiveControl && (ACTOR_IS_AUTHORITY(this) || InteractiveControl->bProcessClientOverlap)) {
		InteractiveControl->OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}

void AInteractiveVRPawn::OnPawnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex) {
	if (OtherActor == this) return;

	if (InteractiveControl && (ACTOR_IS_AUTHORITY(this) || InteractiveControl->bProcessClientOverlap)) {
		InteractiveControl->OnOverlapEnd(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	}
}

void AInteractiveVRPawn::InitializeMotionControllersSource() {
	if (LeftMotionController) {
		LeftMotionController->SetMotionSource(FXRMotionControllerBase::LeftHandSourceId);
	}
	if (RightMotionController) {
		RightMotionController->SetMotionSource(FXRMotionControllerBase::RightHandSourceId);
	}
	bIsMotionControllersInitialized = true;
}

void AInteractiveVRPawn::OnRep_LeftMotionControllerTransform() const {
	if (LeftMotionController != nullptr) {
		LeftMotionController->SetMotionControllerTransform(LeftMotionControllerTransform);
	}
}

void AInteractiveVRPawn::OnRep_RightMotionControllerTransform() const {
	if (RightMotionController != nullptr) {
		RightMotionController->SetMotionControllerTransform(RightMotionControllerTransform);
	}
}

void AInteractiveVRPawn::Client_SendMotionControllerTransforms() {
	// Send controller transforms to the server.
	if (IsLocallyControlled() && !ACTOR_IS_STANDALONE(this)) {
		const FTransform LastLeftMotionControllerTransform = LeftMotionControllerTransform;
		const FTransform LastRightMotionControllerTransform = RightMotionControllerTransform;

		LeftMotionControllerTransform = LeftMotionController ? LeftMotionController->GetMotionControllerTransform() : FTransform::Identity;
		RightMotionControllerTransform = RightMotionController ? RightMotionController->GetMotionControllerTransform() : FTransform::Identity;

		if (!LastLeftMotionControllerTransform.Equals(LeftMotionControllerTransform) || !LastRightMotionControllerTransform.Equals(RightMotionControllerTransform)) {
			Server_UpdateMotionControllerTransforms(LeftMotionControllerTransform, RightMotionControllerTransform);
		}
	}
}

void AInteractiveVRPawn::Server_UpdateMotionControllerTransforms_Implementation(const FTransform InLeftHandTransform, const FTransform InRightHandTransform) {
	LeftMotionControllerTransform = InLeftHandTransform;
	RightMotionControllerTransform = InRightHandTransform;

	if (LeftMotionController) {
		LeftMotionController->SetMotionControllerTransform(InLeftHandTransform);
		LeftMotionController->SetMotionSource(FXRMotionControllerBase::LeftHandSourceId);
	}

	if (RightMotionController) {
		RightMotionController->SetMotionControllerTransform(InRightHandTransform);
		RightMotionController->SetMotionSource(FXRMotionControllerBase::RightHandSourceId);
	}
}

FRotator AInteractiveVRPawn::GetRotationFromInput(const float UpAxis, const float RightAxis) const {
	// Rotate input X+Y to always point forward relative to the current pawn rotation.
	const auto InputCenterNormal = UKismetMathLibrary::Normal(FVector(UpAxis, RightAxis, 0.f));
	const auto InputNormalRotated = FRotator(0.f, GetActorRotation().Yaw, 0.f).RotateVector(InputCenterNormal);
	const auto InputRotationXResult = UKismetMathLibrary::MakeRotFromX(InputNormalRotated);

	// Check whether thumb is near center (to ignore rotation overrides). Adjust this value to narrow the 'dead zone' center.
	const bool bCheckThumbNearCenter = UKismetMathLibrary::Abs(UpAxis) + UKismetMathLibrary::Abs(RightAxis) >= ThumbDeadZone;

	// Select the rotation created by thumb pad input or use current pawn rotation. Use Default rotation if thumb is near center of the pad.
	return UKismetMathLibrary::SelectRotator(InputRotationXResult, GetActorRotation(), bCheckThumbNearCenter);
}

void AInteractiveVRPawn::InitiateTeleportation(AInteractiveMotionController* MotionController) {
	if (!bIsTeleporting) {
		OnBeforeTeleport();

		if (MotionController->GetIsValidTeleportDestination()) {
			bIsTeleporting = true;

			TeleportFadeOut();

			// Fade out and only then teleport with the method called by the timer.
			FTimerDelegate TeleportFadeTimerDelegate;
			TeleportFadeTimerDelegate.BindUObject(this, &AInteractiveVRPawn::FinishTeleportation, MotionController);
			GetWorldTimerManager().SetTimer(TeleportFadeTimerHandle, TeleportFadeTimerDelegate, TeleportFadeOutDuration, false);
		} else {
			MotionController->DisableTeleportationVisuals();
		}
	}
}

void AInteractiveVRPawn::FinishTeleportation(AInteractiveMotionController* MotionController) {
	MotionController->DisableTeleportationVisuals();

	FVector DestinationLocation;
	FRotator DestinationRotation;
	MotionController->GetTeleportDestination(DestinationLocation, DestinationRotation);

	Server_Teleport(DestinationLocation, DestinationRotation);

	TeleportFadeIn();

	GetWorldTimerManager().ClearTimer(TeleportFadeTimerHandle);
	bIsTeleporting = false;

	OnAfterTeleport();
}

void AInteractiveVRPawn::Server_Teleport_Implementation(const FVector NewLocation, const FRotator NewRotation) {
	TeleportTo(NewLocation, NewRotation, false, true);
}

void AInteractiveVRPawn::TeleportFadeOut() {
	const auto CameraManager = GetPlayerCameraManager();
	CHECK_RETURN(CameraManager);

	CameraManager->StartCameraFade(0.0f, 1.0f, TeleportFadeOutDuration, TeleportFadeColor, false, true);
}

void AInteractiveVRPawn::TeleportFadeIn() {
	const auto CameraManager = GetPlayerCameraManager();
	CHECK_RETURN(CameraManager);

	CameraManager->StartCameraFade(1.0f, 0.0f, TeleportFadeInDuration, TeleportFadeColor, false, false);
}

#pragma region Input Proxy

void AInteractiveVRPawn::OnInputActionActivateBegin() {
	if (InteractiveControl == nullptr) return;
	InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionActivate);
}

void AInteractiveVRPawn::OnInputActionActivateEnd() {
	if (InteractiveControl == nullptr) return;
	InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionActivate);
}

void AInteractiveVRPawn::OnInputActionGrabBegin() {
	if (InteractiveControl == nullptr) return;
	InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionGrab);
}

void AInteractiveVRPawn::OnInputActionGrabEnd() {
	if (InteractiveControl == nullptr) return;
	InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionGrab);
}

void AInteractiveVRPawn::OnInputActionUseBegin() {
	if (InteractiveControl == nullptr) return;
	InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionUse);
}

void AInteractiveVRPawn::OnInputActionUseEnd() {
	if (InteractiveControl == nullptr) return;
	InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionUse);
}

void AInteractiveVRPawn::OnInputAxisX(const float Value) {
	if (InteractiveControl == nullptr) return;
	InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisX, Value);
}

void AInteractiveVRPawn::OnInputAxisY(const float Value) {
	if (InteractiveControl == nullptr) return;
	InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisY, Value);
}

void AInteractiveVRPawn::OnInputAxisZ(const float Value) {
	if (InteractiveControl == nullptr) return;
	InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisZ, Value);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void AInteractiveVRPawn::OnInputActionResetVR() {
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AInteractiveVRPawn::OnInputActionTeleportRightBegin() {
	if (RightMotionController) {
		RightMotionController->EnableTeleportationVisuals();
	}

	if (LeftMotionController) {
		LeftMotionController->DisableTeleportationVisuals();
	}
}

void AInteractiveVRPawn::OnInputActionTeleportRightEnd() {
	if (RightMotionController->GetIsTeleportActive()) {
		InitiateTeleportation(RightMotionController);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AInteractiveVRPawn::OnInputActionTeleportLeftBegin() {
	if (RightMotionController) {
		RightMotionController->DisableTeleportationVisuals();
	}

	if (LeftMotionController) {
		LeftMotionController->EnableTeleportationVisuals();
	}
}

void AInteractiveVRPawn::OnInputActionTeleportLeftEnd() {
	if (LeftMotionController->GetIsTeleportActive()) {
		InitiateTeleportation(LeftMotionController);
	}
}

void AInteractiveVRPawn::OnInputActionActivateRightBegin() {
	if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionActivate, RightMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionActivateRightEnd() {
	if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionActivate, RightMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionActivateLeftBegin() {
	if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionActivate, LeftMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionActivateLeftEnd() {
	if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionActivate, LeftMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionGrabRightBegin() {
	if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionGrab, RightMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionGrabRightEnd() {
	if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionGrab, RightMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionGrabLeftBegin() {
	if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionGrab, LeftMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionGrabLeftEnd() {
	if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionGrab, LeftMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionUseRightBegin() {
	if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionUse, RightMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionUseRightEnd() {
	if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionUse, RightMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionUseLeftBegin() {
	if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputBegin(EInteractionInputActionType::ActionUse, LeftMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputActionUseLeftEnd() {
	if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputEnd(EInteractionInputActionType::ActionUse, LeftMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputAxisXRight(const float Value) {
	if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisX, Value, RightMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputAxisXLeft(const float Value) {
	if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisX, Value, LeftMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputAxisYRight(const float Value) {
	if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisY, Value, RightMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputAxisYLeft(const float Value) {
	if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisY, Value, LeftMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputAxisZRight(const float Value) {
	if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisZ, Value, RightMotionController->InteractiveControl);
}

void AInteractiveVRPawn::OnInputAxisZLeft(const float Value) {
	if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return;
	InteractiveControl->OnInputAxis(EInteractionInputAxisType::AxisZ, Value, LeftMotionController->InteractiveControl);
}

#pragma endregion
