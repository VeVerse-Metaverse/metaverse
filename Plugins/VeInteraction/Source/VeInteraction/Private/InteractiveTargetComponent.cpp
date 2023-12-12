// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "Components/InteractiveTargetComponent.h"

#include "Interaction.h"
#include "InteractionMacros.h"
#include "Components/InteractiveControlComponent.h"
#include "Interfaces/Interactive.h"
#include "Net/UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"
#include "VeShared.h"

/**
 * This is the handles count added into the interactive actor template BP.
 * So for now we support up to 6 hand setups for different grab points.
 */
constexpr static int MaxHandMarkersCount = 6;

UInteractiveTargetComponent::UInteractiveTargetComponent()
	: Super() {
	PrimaryComponentTick.bCanEverTick = true;

	for (int i = 0; i < MaxHandMarkersCount; i++) {
		const FInteractiveFingerSettings HandFingerSettings;
		RightHandFingerSettings.Add(HandFingerSettings);
		LeftHandFingerSettings.Add(HandFingerSettings);
	}

#if UE_EDITOR && UE_BUILD_DEBUG
	PrimaryComponentTick.TickInterval = 0.0f;
#else
	// Optimized for production.
	PrimaryComponentTick.TickInterval = 0.125f;
#endif
}

void UInteractiveTargetComponent::BeginPlay() {
	Super::BeginPlay();

	if (PhysicsRootComponent == nullptr) {
		PhysicsRootComponent = GetOwner()->GetRootComponent();
	}

	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	OriginalTransform = Owner->GetActorTransform();
}

void UInteractiveTargetComponent::BeginDestroy() {
	Super::BeginDestroy();
}

void UInteractiveTargetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (AttachedToControl) {
		AttachedToControl->OnTargetDestroyed(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UInteractiveTargetComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractiveTargetComponent, AttachedTo);
	DOREPLIFETIME(UInteractiveTargetComponent, AttachedToControl);
	DOREPLIFETIME(UInteractiveTargetComponent, bResetVelocityOnAttach);
	DOREPLIFETIME(UInteractiveTargetComponent, bResetVelocityOnDetach);
	DOREPLIFETIME(UInteractiveTargetComponent, PhysicsComponent);
	DOREPLIFETIME(UInteractiveTargetComponent, PhysicsRootComponent);
	DOREPLIFETIME(UInteractiveTargetComponent, OriginalTransform);
	DOREPLIFETIME(UInteractiveTargetComponent, LeftHandTransform);
	DOREPLIFETIME(UInteractiveTargetComponent, RightHandTransform);
	DOREPLIFETIME(UInteractiveTargetComponent, PivotHandTransform);
}

uint8 UInteractiveTargetComponent::GetInteractiveSlotId() const {
	return InteractiveSlotId;
}

#pragma region Custom RPC
void UInteractiveTargetComponent::Standalone_CustomEvent(UInteractiveControlComponent* Control, const FString& Topic, TArray<uint8> Parameters) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneCustomEvent(Owner, this, Control, Topic, Parameters);
}

void UInteractiveTargetComponent::Server_CustomEvent(UInteractiveControlComponent* Control, const FString& Topic, TArray<uint8> Parameters) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerCustomEvent(Owner, this, Control, Topic, Parameters);
}

void UInteractiveTargetComponent::Client_CustomEvent(UInteractiveControlComponent* Control, const FString& Topic, TArray<uint8> Parameters) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientCustomEvent(Owner, this, Control, Topic, Parameters);
}
#pragma endregion

#pragma region Focus
void UInteractiveTargetComponent::Standalone_FocusBegin(UInteractiveControlComponent* Control,
														const FVector_NetQuantize InOrigin,
														const FVector_NetQuantize InLocation,
														const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneFocusBegin(Owner, this, Control, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Server_FocusBegin(UInteractiveControlComponent* Control,
													const FVector_NetQuantize InOrigin,
													const FVector_NetQuantize InLocation,
													const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerFocusBegin(Owner, this, Control, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Client_FocusBegin(UInteractiveControlComponent* Control,
													const FVector_NetQuantize InFocusOrigin,
													const FVector_NetQuantize InLocation,
													const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientFocusBegin(Owner, this, Control, InFocusOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Standalone_FocusEnd(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneFocusEnd(Owner, this, Control);
}

void UInteractiveTargetComponent::Server_FocusEnd(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerFocusEnd(Owner, this, Control);
}

void UInteractiveTargetComponent::Client_FocusEnd(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientFocusEnd(Owner, this, Control);
}
#pragma endregion

#pragma region Overlap
void UInteractiveTargetComponent::Standalone_OverlapBegin(UInteractiveControlComponent* Control,
														  const FVector_NetQuantize InOrigin,
														  const FVector_NetQuantize InLocation,
														  const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneOverlapBegin(Owner, this, Control, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Server_OverlapBegin(UInteractiveControlComponent* Control, const FVector_NetQuantize InOrigin, const FVector_NetQuantize InLocation,
													  const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerOverlapBegin(Owner, this, Control, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Client_OverlapBegin(UInteractiveControlComponent* Control, const FVector_NetQuantize InFocusOrigin, const FVector_NetQuantize InLocation,
													  const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientOverlapBegin(Owner, this, Control, InFocusOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Standalone_OverlapEnd(UInteractiveControlComponent* Control) {

	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneOverlapEnd(Owner, this, Control);
}

void UInteractiveTargetComponent::Server_OverlapEnd(UInteractiveControlComponent* Control) {

	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerOverlapEnd(Owner, this, Control);
}

void UInteractiveTargetComponent::Client_OverlapEnd(UInteractiveControlComponent* Control) {

	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientOverlapEnd(Owner, this, Control);
}
#pragma endregion

#pragma region Activate
void UInteractiveTargetComponent::Standalone_ActivateBegin(UInteractiveControlComponent* Control, const FVector_NetQuantize InOrigin,
														   const FVector_NetQuantize InLocation, const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneActivateBegin(Owner, this, Control, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Server_ActivateBegin(UInteractiveControlComponent* Control, const FVector_NetQuantize InOrigin,
													   const FVector_NetQuantize InLocation, const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerActivateBegin(Owner, this, Control, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Client_ActivateBegin(UInteractiveControlComponent* Control, const FVector_NetQuantize InOrigin,
													   const FVector_NetQuantize InLocation, const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientActivateBegin(Owner, this, Control, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Standalone_ActivateEnd(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneActivateEnd(Owner, this, Control);
}

void UInteractiveTargetComponent::Server_ActivateEnd(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerActivateEnd(Owner, this, Control);
}

void UInteractiveTargetComponent::Client_ActivateEnd(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientActivateEnd(Owner, this, Control);
}

void UInteractiveTargetComponent::Standalone_UserInputBegin(UInteractiveControlComponent* InControl, const FName& InInputName, const FKey& InKey,
															const FVector_NetQuantize& InOrigin, const FVector_NetQuantize& InLocation, const FVector_NetQuantizeNormal& InNormal) {

	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneUserInputBegin(Owner, this, InControl, InInputName, InKey, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Server_UserInputBegin(UInteractiveControlComponent* InControl, const FName& InName, const FKey& InKey, const FVector_NetQuantize& InOrigin,
														const FVector_NetQuantize& InLocation, const FVector_NetQuantizeNormal& InNormal) {

	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerUserInputBegin(Owner, this, InControl, InName, InKey, InOrigin, InLocation, InNormal);

}

void UInteractiveTargetComponent::Client_UserInputBegin(UInteractiveControlComponent* InControl, const FName& InName, const FKey& InKey, const FVector_NetQuantize& InOrigin,
														const FVector_NetQuantize& InLocation, const FVector_NetQuantizeNormal& InNormal) {

	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientUserInputBegin(Owner, this, InControl, InName, InKey, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Standalone_UserInputEnd(UInteractiveControlComponent* InControl, const FName& InName, const FKey& InKey) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneUserInputEnd(Owner, this, InControl, InName, InKey);
}

void UInteractiveTargetComponent::Server_UserInputEnd(UInteractiveControlComponent* InControl, const FName& InName, const FKey& InKey) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerUserInputEnd(Owner, this, InControl, InName, InKey);
}

void UInteractiveTargetComponent::Client_UserInputEnd(UInteractiveControlComponent* InControl, const FName& InName, const FKey& InKey) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientActivateEnd(Owner, this, InControl);
}
#pragma endregion

#pragma region Grab

void UInteractiveTargetComponent::Standalone_GrabBegin(UInteractiveControlComponent* Control, const FVector_NetQuantize InOrigin,
													   const FVector_NetQuantize InLocation, const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneGrabBegin(Owner, this, Control, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Server_GrabBegin(UInteractiveControlComponent* Control, const FVector_NetQuantize InOrigin,
												   const FVector_NetQuantize InLocation, const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerGrabBegin(Owner, this, Control, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Client_GrabBegin(UInteractiveControlComponent* Control, FVector_NetQuantize InOrigin,
												   const FVector_NetQuantize InLocation, const FVector_NetQuantizeNormal InNormal) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientGrabBegin(Owner, this, Control, InOrigin, InLocation, InNormal);
}

void UInteractiveTargetComponent::Standalone_GrabEnd(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneGrabEnd(Owner, this, Control);
}

void UInteractiveTargetComponent::Server_GrabEnd(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerGrabEnd(Owner, this, Control);
}

void UInteractiveTargetComponent::Client_GrabEnd(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientGrabEnd(Owner, this, Control);
}
#pragma endregion

#pragma region Attach

void UInteractiveTargetComponent::ChangeOwnerScale(bool ScaleDown, bool bIsVR) {
	if (bIsVR ? bVRScaleDownWithGrab : bPawnScaleDownWithGrab) {
		const auto Owner = GetOwner();
		CHECK_RETURN(Owner);

		if (ScaleDown) {
			FVector OwnerOrigin;
			FVector OwnerExtent;
			Owner->GetActorBounds(true, OwnerOrigin, OwnerExtent);
			const float WorldSize = FMath::Sqrt(FMath::Square(OwnerExtent.GetAbsMax()) * 2);
			float ScaleMultiplier = PawnGrabbedSize / WorldSize;
			if (bIsVR) {
				ScaleMultiplier = VRGrabbedSize / WorldSize;
			}

			FinalScale = Owner->GetActorScale() * ScaleMultiplier;
		} else {
			FinalScale = OriginalTransform.GetScale3D();
		}

		Owner->SetActorScale3D(FinalScale);
	}
}

void UInteractiveTargetComponent::SetPhysicsRootComponent(UPrimitiveComponent* Component) {
	PhysicsRootComponent = Component;
	Component->SetIsReplicated(true);
}

void UInteractiveTargetComponent::SetPhysicsComponent(UPrimitiveComponent* Component) {
	PhysicsComponent = Component;
	Component->SetIsReplicated(true);
}

void UInteractiveTargetComponent::ResetPhysicsComponent(const bool bSimulatePhysics, const bool bResetVelocity) const {
	if (PhysicsComponent == nullptr) {
		return;
	}

	PhysicsComponent->SetSimulatePhysics(bSimulatePhysics);

	if (bResetVelocity) {
		PhysicsComponent->SetAllPhysicsLinearVelocity(FVector(0), false);
		PhysicsComponent->SetAllPhysicsAngularVelocityInRadians(FVector(0), false);
	}

	if (PhysicsRootComponent == PhysicsComponent) {
		return;
	}

	if (PhysicsComponent == GetOwner()->GetRootComponent()) {
		return;
	}

	PhysicsComponent->AttachToComponent(PhysicsRootComponent,
										FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true));
}

void UInteractiveTargetComponent::SetAttachedToComponent(USceneComponent* Parent) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);

	if (ACTOR_IS_AUTHORITY(Owner)) {
		AttachedTo = Parent;
	}
}

void UInteractiveTargetComponent::AttachToControl(UInteractiveControlComponent* Control) {
	OnAttached.Broadcast(Control);
	SetAttachedToControl(Control);
}

void UInteractiveTargetComponent::DetachFromControl(UInteractiveControlComponent* Control) {
	OnDetached.Broadcast(Control);
	SetAttachedToControl(nullptr);
}

void UInteractiveTargetComponent::SetAttachedToControl(UInteractiveControlComponent* Parent) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);

	if (Owner->HasAuthority()) {
		AttachedToControl = Parent;
	}
}

bool UInteractiveTargetComponent::IsAttached() const {
	return AttachedTo != nullptr;
}

bool UInteractiveTargetComponent::IsAttachedToControl(UInteractiveControlComponent* Control) const {
	return AttachedToControl == Control;
}

bool UInteractiveTargetComponent::IsAttachedToControlPawn(UInteractiveControlComponent* Control) const {
	if (AttachedToControl == nullptr || Control == nullptr) return false;
	return AttachedToControl->GetPawn() == Control->GetPawn();
}

bool UInteractiveTargetComponent::IsAttachedToControlMotionController(UInteractiveControlComponent* Control) const {
	if (AttachedToControl == nullptr || Control == nullptr) return false;
	return AttachedToControl->GetMotionController() == Control->GetMotionController();
}

void UInteractiveTargetComponent::Standalone_Attach(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneAttach(Owner, this, Control);
}

void UInteractiveTargetComponent::Server_Attach(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerAttach(Owner, this, Control);
}

void UInteractiveTargetComponent::Client_Attach(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientAttach(Owner, this, Control);
}

void UInteractiveTargetComponent::Standalone_Detach(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneDetach(Owner, this, Control);
}

void UInteractiveTargetComponent::Server_Detach(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerDetach(Owner, this, Control);
}

void UInteractiveTargetComponent::Client_Detach(UInteractiveControlComponent* Control) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientDetach(Owner, this, Control);
}
#pragma endregion

#pragma region Activate Axis
void UInteractiveTargetComponent::Standalone_ActivateAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, float Value) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneActivateAxis(Owner, this, Control, Axis, Value);
}

void UInteractiveTargetComponent::Server_ActivateAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, const float Value) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerActivateAxis(Owner, this, Control, Axis, Value);
}

void UInteractiveTargetComponent::Client_ActivateAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, const float Value) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientActivateAxis(Owner, this, Control, Axis, Value);
}
#pragma endregion

#pragma region Use
void UInteractiveTargetComponent::Standalone_UseBegin(UInteractiveControlComponent* Control, uint8 UseKeyIndex) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneUseBegin(Owner, this, Control, UseKeyIndex);
}

void UInteractiveTargetComponent::Server_UseBegin(UInteractiveControlComponent* Control, const uint8 UseKeyIndex) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerUseBegin(Owner, this, Control, UseKeyIndex);
}

void UInteractiveTargetComponent::Client_UseBegin(UInteractiveControlComponent* Control, const uint8 UseKeyIndex) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientUseBegin(Owner, this, Control, UseKeyIndex);
}

void UInteractiveTargetComponent::Standalone_UseEnd(UInteractiveControlComponent* Control, uint8 UseKeyIndex) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneUseEnd(Owner, this, Control, UseKeyIndex);
}

void UInteractiveTargetComponent::Server_UseEnd(UInteractiveControlComponent* Control, const uint8 UseKeyIndex) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerUseEnd(Owner, this, Control, UseKeyIndex);
}

void UInteractiveTargetComponent::Client_UseEnd(UInteractiveControlComponent* Control, const uint8 UseKeyIndex) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientUseEnd(Owner, this, Control, UseKeyIndex);
}
#pragma endregion

#pragma region UseAxis
void UInteractiveTargetComponent::Standalone_UseAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, float Value, uint8 UseKeyIndex) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnStandaloneUseAxis(Owner, this, Control, Axis, Value, UseKeyIndex);
}

void UInteractiveTargetComponent::Server_UseAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, const float Value, const uint8 UseKeyIndex) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnServerUseAxis(Owner, this, Control, Axis, Value, UseKeyIndex);
}

void UInteractiveTargetComponent::Client_UseAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, const float Value, const uint8 Slot) {
	const auto Owner = GetOwner();
	CHECK_RETURN(Owner);
	CHECK_RETURN(Owner->Implements<UInteractive>());
	IInteractive::Execute_OnClientUseAxis(Owner, this, Control, Axis, Value, Slot);
}
#pragma endregion
