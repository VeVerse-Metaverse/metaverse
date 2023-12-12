// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Components/ArrowComponent.h"
#include "Enums/InteractionType.h"
#include "Structs/InteractiveFingerSettings.h"


#include "InteractiveTargetComponent.generated.h"

class UInteractiveControlComponent;

/** Interaction event replication options. */
USTRUCT(BlueprintType)
struct FEventReplicationOptions {
	GENERATED_BODY()

	/** Focus event is replicated from the server to clients. Will work at the server only if disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Focus"))
	bool bFocus = true;

	/** Overlap event is replicated from the server to clients. Will work at the server only if disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Overlap"))
	bool bOverlap = true;

	/** Activate event is replicated from server to clients and from client to server for the instigator control. Will work at the instigator client only if disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Activate"))
	bool bActivate = true;

	/** Custom events. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Events"))
	bool bEvents = true;

	/** Custom user input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Events"))
	bool bUser = true;
};

/**
 * Interaction event multicast options.
 * @note Grab, attach and detach events are multicasted by default and it can not be disabled as it can lead to confusion and bugs related to lack of actor transform synchronization between servers and clients.
 */
USTRUCT(BlueprintType)
struct FEventMulticastOptions {
	GENERATED_BODY()

	/** Enabling will make server to broadcast focus events to every client connected. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Focus"))
	bool bFocus = true;

	/** Enabling will make server to broadcast overlap events to every client connected. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Overlap"))
	bool bOverlap = true;

	/** Enabling will make server to broadcast activate events to every client connected. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Activate"))
	bool bActivate = true;
};

/** Interaction event client only options. Allow to block the server from doing specific interaction event processing. */
USTRUCT(BlueprintType)
struct FEventClientOnlyOptions {
	GENERATED_BODY()

	/** Force the focus events to be called only at the local client or in standalone mode. If enabled, server won't call any focus events. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Focus"))
	bool bFocus = false;

	/** Force the overlap events to be called only at the local client or in standalone mode. If enabled, server won't call any overlap events. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Overlap"))
	bool bOverlap = false;

	/** Force the grab/attach/detach events to be called only at the local client or in standalone mode. If enabled, server won't call any such events. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Grab"))
	bool bGrab = false;

	/** Force the use events to be called only at the local client or in standalone mode. If enabled, server won't call any such events. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Use"))
	bool bUse = false;

	/** Force the user events to be called only at the local client or in standalone mode. If enabled, server won't call any such events. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="User"))
	bool bUser = false;
};

UENUM(BlueprintType)
enum EHandsVisibility {

	NotVisible UMETA(DisplayName = "NotVisible"),
	LeftVisible UMETA(DisplayName = "LeftVisible"),
	RightVisible UMETA(DisplayName = "RightVisible"),
	BothVisible UMETA(DisplayName = "BothVisible"),
	PawnVisible UMETA(DisplayName = "PawnVisible"),
	AllVisible UMETA(DisplayName = "AllVisible"),
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractiveTargetAttached, UInteractiveControlComponent*, Control);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractiveTargetDetached, UInteractiveControlComponent*, Control);

/** Interaction target. */
UCLASS(BlueprintType, ClassGroup="Interaction", meta=(BlueprintSpawnableComponent))
class VEINTERACTION_API UInteractiveTargetComponent final : public UActorComponent {
	GENERATED_BODY()

public:
	explicit UInteractiveTargetComponent();

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	/** Called when attached to the control. */
	UPROPERTY(BlueprintAssignable)
	FInteractiveTargetAttached OnAttached;

	/** Called when detached from the control. */
	UPROPERTY(BlueprintAssignable)
	FInteractiveTargetDetached OnDetached;

	/** Use physics handles for grab. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handles")
	bool bUsePhysicsHandles;

	/** Pivot handle. */
	UPROPERTY(BlueprintReadWrite, Category = "Handles")
	UPrimitiveComponent* PivotHandle;

	/** Meshes of left handles. */
	UPROPERTY(BlueprintReadWrite, Category = "Handles")
	USkeletalMeshComponent* PivotHandMesh;

	/** Pivot hand bone transform. */
	UPROPERTY(BlueprintReadOnly, Replicated, Category="Handles")
	FTransform PivotHandTransform;

	/** Left Handles count */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ClampMin="1", ClampMax="6"), Category="Handles")
	int32 LeftHandlesCount = 1;

	/** Left hand handles. */
	UPROPERTY(BlueprintReadWrite, Category = "Handles")
	TArray<UPrimitiveComponent*> LeftHandHandles;

	/** Meshes of left handles. */
	UPROPERTY(BlueprintReadWrite, Category = "Handles")
	TArray<USkeletalMeshComponent*> LeftHandMeshes;

	/** Left hand bone transform. */
	UPROPERTY(BlueprintReadOnly, Replicated, Category="Handles")
	FTransform LeftHandTransform;

	/** Right Handles count */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ClampMin="1", ClampMax="6"), Category="Handles")
	int32 RightHandlesCount = 1;

	/** Right hand handles. */
	UPROPERTY(BlueprintReadWrite, Category = "Handles")
	TArray<UPrimitiveComponent*> RightHandHandles;

	/** Meshes of right handles. */
	UPROPERTY(BlueprintReadWrite, Category = "Handles")
	TArray<USkeletalMeshComponent*> RightHandMeshes;

	/** Right hand bone transform. */
	UPROPERTY(BlueprintReadOnly, Replicated, Category="Handles")
	FTransform RightHandTransform;

	/** Left Handles count */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Handles")
	TEnumAsByte<EHandsVisibility> HandsVisibility = AllVisible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Handles|Fingers")
	bool bUseFingers;

	/** Describes how fingers should wrap the object. Matches handles. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, EditFixedSize, Category="Handles|Fingers")
	TArray<FInteractiveFingerSettings> LeftHandFingerSettings;

	/** Describes how fingers should wrap the object. Matches handles. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, EditFixedSize, Category="Handles|Fingers")
	TArray<FInteractiveFingerSettings> RightHandFingerSettings;

	/** Describes how fingers should wrap the object. Matches handles. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, EditFixedSize, Category="Handles|Fingers")
	FInteractiveFingerSettings DefaultHandFingerSettings;

	/** World Scale for construction script */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldScale", meta=(ClampMin="0.000001"))
	float Scale = 1.0f;

#pragma region Interactive Slots
	/** The slot this object is assigned to at the control. */
	UPROPERTY()
	uint8 InteractiveSlotId;

	UFUNCTION(BlueprintCallable)
	uint8 GetInteractiveSlotId() const;
#pragma endregion

#pragma region Replication Options

	/** Events which should be run only at clients and ignored at the server side. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Intercast", meta=(DisplayName="Client Only Events"))
	FEventClientOnlyOptions ClientOnlyEvents;

	/** Events to be replicated. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Intercast", meta=(DisplayName="Replicate Events"))
	FEventReplicationOptions ReplicateEvents;

	/**
	 * Events to be multicasted to clients. Disabled state for "only instigator client", enabled is for "all clients".
	 * @note Grab, attach and detach events are multicasted by default and it can not be disabled as it can lead to confusion and bugs related to lack of actor transform synchronization between servers and clients.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Intercast", meta=(DisplayName="Multicast Events"))
	FEventMulticastOptions MulticastEvents;

	/** Check if the event should run only at clients (ignore at the server side). */
	UFUNCTION(BlueprintPure)
	bool IsClientOnly(const EInteractionType Interaction) const {
		switch (Interaction) {
		case EInteractionType::Focus:
			return ClientOnlyEvents.bFocus;
		case EInteractionType::Overlap:
			return ClientOnlyEvents.bOverlap;
		case EInteractionType::Grab:
			return ClientOnlyEvents.bGrab;
		case EInteractionType::Use:
			return ClientOnlyEvents.bUse;
		case EInteractionType::User:
			return ClientOnlyEvents.bUser;
		default:
			return false;
		}
	}

	/** Check if the event should be replicated from the server to clients (and from client to server in case of input events). */
	UFUNCTION(BlueprintPure)
	bool IsReplicating(const EInteractionType Interaction) const {
		switch (Interaction) {
		case EInteractionType::Focus:
			return ReplicateEvents.bFocus;
		case EInteractionType::Overlap:
			return ReplicateEvents.bOverlap;
		case EInteractionType::Activate:
			return ReplicateEvents.bActivate;
		case EInteractionType::Grab:
			return !ClientOnlyEvents.bGrab;
		case EInteractionType::Use:
			return !ClientOnlyEvents.bUse;
		case EInteractionType::Events:
			return ReplicateEvents.bEvents;
		case EInteractionType::User:
			return ReplicateEvents.bUser;
		default:
			return false;
		}
	}

	/** Check if the event should be multicasted to all clients or to the instigator only. */
	UFUNCTION(BlueprintPure)
	bool IsMulticasting(const EInteractionType Interaction) const {
		switch (Interaction) {
		case EInteractionType::Focus:
			return MulticastEvents.bFocus;
		case EInteractionType::Overlap:
			return MulticastEvents.bOverlap;
		case EInteractionType::Activate:
			return MulticastEvents.bActivate;
		case EInteractionType::Grab:
			// Grab can only be always multicasted or client-only. Otherwise it will lead to broken transform synchronization between clients and servers. 
			return !ClientOnlyEvents.bGrab;
		case EInteractionType::Use:
			return !ClientOnlyEvents.bUse;
		case EInteractionType::User:
			return !ClientOnlyEvents.bUser;
		default:
			return false;
		}
	}

#pragma endregion

#pragma region Custom RPC
	void Standalone_CustomEvent(UInteractiveControlComponent* Control,
								const FString& Topic,
								TArray<uint8> Parameters);

	void Server_CustomEvent(UInteractiveControlComponent* Control,
							const FString& Topic,
							TArray<uint8> Parameters);

	void Client_CustomEvent(UInteractiveControlComponent* Control,
							const FString& Topic,
							TArray<uint8> Parameters);
#pragma endregion

#pragma region Focus
	void Standalone_FocusBegin(UInteractiveControlComponent* Control,
							   FVector_NetQuantize InOrigin = FVector_NetQuantize(),
							   FVector_NetQuantize InLocation = FVector_NetQuantize(),
							   FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Server_FocusBegin(UInteractiveControlComponent* Control,
						   FVector_NetQuantize InOrigin = FVector_NetQuantize(),
						   FVector_NetQuantize InLocation = FVector_NetQuantize(),
						   FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Client_FocusBegin(UInteractiveControlComponent* Control,
						   FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
						   FVector_NetQuantize InLocation = FVector_NetQuantize(),
						   FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());


	void Standalone_FocusEnd(UInteractiveControlComponent* Control);

	void Server_FocusEnd(UInteractiveControlComponent* Control);

	void Client_FocusEnd(UInteractiveControlComponent* Control);
#pragma endregion

#pragma region Overlap
	void Standalone_OverlapBegin(UInteractiveControlComponent* Control,
								 FVector_NetQuantize InOrigin = FVector_NetQuantize(),
								 FVector_NetQuantize InLocation = FVector_NetQuantize(),
								 FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Server_OverlapBegin(UInteractiveControlComponent* Control,
							 FVector_NetQuantize InOrigin = FVector_NetQuantize(),
							 FVector_NetQuantize InLocation = FVector_NetQuantize(),
							 FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Client_OverlapBegin(UInteractiveControlComponent* Control,
							 FVector_NetQuantize InOrigin = FVector_NetQuantize(),
							 FVector_NetQuantize InLocation = FVector_NetQuantize(),
							 FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Standalone_OverlapEnd(UInteractiveControlComponent* Control);

	void Server_OverlapEnd(UInteractiveControlComponent* Control);

	void Client_OverlapEnd(UInteractiveControlComponent* Control);
#pragma endregion

#pragma region Activate
	void Standalone_ActivateBegin(UInteractiveControlComponent* Control,
								  FVector_NetQuantize InOrigin = FVector_NetQuantize(),
								  FVector_NetQuantize InLocation = FVector_NetQuantize(),
								  FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Server_ActivateBegin(UInteractiveControlComponent* Control,
							  FVector_NetQuantize InOrigin = FVector_NetQuantize(),
							  FVector_NetQuantize InLocation = FVector_NetQuantize(),
							  FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Client_ActivateBegin(UInteractiveControlComponent* Control,
							  FVector_NetQuantize InOrigin = FVector_NetQuantize(),
							  FVector_NetQuantize InLocation = FVector_NetQuantize(),
							  FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Standalone_ActivateEnd(UInteractiveControlComponent* Control);

	void Server_ActivateEnd(UInteractiveControlComponent* Control);

	void Client_ActivateEnd(UInteractiveControlComponent* Control);
#pragma endregion

#pragma region UserInput
	void Standalone_UserInputBegin(UInteractiveControlComponent* InControl, const FName& InInputName, const FKey& InKey, const FVector_NetQuantize& InOrigin,
								   const FVector_NetQuantize& InLocation, const FVector_NetQuantizeNormal& InNormal);


	void Server_UserInputBegin(UInteractiveControlComponent* InControl,
							   const FName& InName, const FKey& InKey,
							   const FVector_NetQuantize& InOrigin = FVector_NetQuantize(),
							   const FVector_NetQuantize& InLocation = FVector_NetQuantize(),
							   const FVector_NetQuantizeNormal& InNormal = FVector_NetQuantizeNormal());

	void Client_UserInputBegin(UInteractiveControlComponent* InControl,
							   const FName& InName, const FKey& InKey,
							   const FVector_NetQuantize& InOrigin = FVector_NetQuantize(),
							   const FVector_NetQuantize& InLocation = FVector_NetQuantize(),
							   const FVector_NetQuantizeNormal& InNormal = FVector_NetQuantizeNormal());

	void Standalone_UserInputEnd(UInteractiveControlComponent* InControl, const FName& InName, const FKey& InKey);

	void Server_UserInputEnd(UInteractiveControlComponent* InControl, const FName& InName, const FKey& InKey);

	void Client_UserInputEnd(UInteractiveControlComponent* InControl, const FName& InName, const FKey& InKey);
#pragma endregion

#pragma region Grab
	/** Change Scale Owner when grabbed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grab Scale")
	bool bVRScaleDownWithGrab = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grab Scale")
	float VRGrabbedSize = 15.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grab Scale")
	bool bPawnScaleDownWithGrab = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grab Scale")
	float PawnGrabbedSize = 15.0f;

	/** Transform to place object at reset. */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	FTransform OriginalTransform = FTransform::Identity;

	UPROPERTY()
	FVector FinalScale;

	void ChangeOwnerScale(bool ScaleDown, bool bIsVR);

	void Standalone_GrabBegin(UInteractiveControlComponent* Control,
							  FVector_NetQuantize InOrigin = FVector_NetQuantize(),
							  FVector_NetQuantize InLocation = FVector_NetQuantize(),
							  FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Server_GrabBegin(UInteractiveControlComponent* Control,
						  FVector_NetQuantize InOrigin = FVector_NetQuantize(),
						  FVector_NetQuantize InLocation = FVector_NetQuantize(),
						  FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Client_GrabBegin(UInteractiveControlComponent* Control,
						  FVector_NetQuantize InOrigin = FVector_NetQuantize(),
						  FVector_NetQuantize InLocation = FVector_NetQuantize(),
						  FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	void Standalone_GrabEnd(UInteractiveControlComponent* Control);

	void Server_GrabEnd(UInteractiveControlComponent* Control);

	void Client_GrabEnd(UInteractiveControlComponent* Control);
#pragma endregion

#pragma region Use
	void Standalone_UseBegin(UInteractiveControlComponent* Control, uint8 UseKeyIndex = 0);

	void Server_UseBegin(UInteractiveControlComponent* Control, uint8 UseKeyIndex = 0);

	void Client_UseBegin(UInteractiveControlComponent* Control, uint8 UseKeyIndex = 0);

	void Standalone_UseEnd(UInteractiveControlComponent* Control, uint8 UseKeyIndex = 0);

	void Server_UseEnd(UInteractiveControlComponent* Control, uint8 UseKeyIndex = 0);

	void Client_UseEnd(UInteractiveControlComponent* Control, uint8 UseKeyIndex = 0);
#pragma endregion

#pragma region Attach
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|AttachmentRules")
	EAttachmentRule DefaultMotionControllerLocationAttachmentRule = EAttachmentRule::SnapToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|AttachmentRules")
	EAttachmentRule DefaultMotionControllerRotationAttachmentRule = EAttachmentRule::SnapToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|AttachmentRules")
	EAttachmentRule DefaultMotionControllerScaleAttachmentRule = EAttachmentRule::KeepWorld;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|AttachmentRules")
	bool DefaultMotionControllerWeldSimulatedBodies = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Deprecated))
	EAttachmentRule RightHandLocationAttachmentRule = EAttachmentRule::SnapToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Deprecated))
	EAttachmentRule RightHandRotationAttachmentRule = EAttachmentRule::SnapToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Deprecated))
	EAttachmentRule RightHandScaleAttachmentRule = EAttachmentRule::KeepWorld;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Deprecated))
	bool RightHandWeldSimulatedBodies = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Deprecated))
	EAttachmentRule LeftHandLocationAttachmentRule = EAttachmentRule::SnapToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Deprecated))
	EAttachmentRule LeftHandRotationAttachmentRule = EAttachmentRule::SnapToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Deprecated))
	EAttachmentRule LeftHandScaleAttachmentRule = EAttachmentRule::KeepWorld;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Deprecated))
	bool LeftHandWeldSimulatedBodies = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|AttachmentRules")
	EAttachmentRule PawnLocationAttachmentRule = EAttachmentRule::SnapToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|AttachmentRules")
	EAttachmentRule PawnRotationAttachmentRule = EAttachmentRule::SnapToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|AttachmentRules")
	EAttachmentRule PawnScaleAttachmentRule = EAttachmentRule::KeepWorld;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|AttachmentRules")
	bool PawnWeldSimulatedBodies = true;

	/** Socket to use when attaching object to a pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PawnAttachSocketName = NAME_None;
	/** Socket to use when attaching object to a pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LeftHandAttachSocketName = TEXT("AttachLeft");
	/** Socket to use when attaching object to a pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RightHandAttachSocketName = TEXT("AttachRight");

	/** The exact scene component to that we are attached. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Interaction")
	USceneComponent* AttachedTo;
	/** The control component to that we are attached. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Interaction")
	UInteractiveControlComponent* AttachedToControl;

	/** Should the physics state be updated. Enable on attach, disable on detach. Physics component must be set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	bool bUpdatePhysicsState = false;

	/** Should the physics component velocity be reset to zero when attached. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Interaction")
	bool bResetVelocityOnAttach = true;

	/** Should the physics component velocity be reset to zero when detached. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Interaction")
	bool bResetVelocityOnDetach = false;

	/** The component to reattach the physics component to after detach from the pawn. Defaults to the root component if not set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Interaction")
	USceneComponent* PhysicsRootComponent;

	/** The component to reattach the physics component to after detach from the pawn. */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void SetPhysicsRootComponent(class UPrimitiveComponent* Component);

	/** The component responsible for the object physics calculations, used for attach/detach physics state update. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Interaction")
	UPrimitiveComponent* PhysicsComponent;

	/** The component responsible for the object physics calculations, used for attach/detach physics state update. */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void SetPhysicsComponent(class UPrimitiveComponent* Component);

	/** Re-enable physics and fix root component attachment after detach. */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void ResetPhysicsComponent(bool bSimulatePhysics = false, bool bResetVelocity = false) const;

	void SetAttachedToComponent(USceneComponent* Parent = nullptr);

	/** Notify the target about control it is attached to. */
	void AttachToControl(UInteractiveControlComponent* Control);
	/** Notify the target about control it is attached to. */
	void DetachFromControl(UInteractiveControlComponent* Control);

private:
	void SetAttachedToControl(UInteractiveControlComponent* Parent = nullptr);

public:
	UFUNCTION(BlueprintPure, Category="Interaction")
	bool IsAttached() const;

	/** Check if the target is attached to the specified control. */
	UFUNCTION(BlueprintPure, Category="Interaction")
	bool IsAttachedToControl(UInteractiveControlComponent* Control) const;

	/** Check if the target is attached to the specified control pawn. */
	UFUNCTION(BlueprintPure, Category="Interaction")
	bool IsAttachedToControlPawn(UInteractiveControlComponent* Control) const;

	/** Check if the target is attached to the specified control motion controller. */
	UFUNCTION(BlueprintPure, Category="Interaction")
	bool IsAttachedToControlMotionController(UInteractiveControlComponent* Control) const;

public:
	void Standalone_Attach(UInteractiveControlComponent* Control);

	// UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Interaction")
	void Server_Attach(UInteractiveControlComponent* Control);

	// UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Interaction")
	void Client_Attach(UInteractiveControlComponent* Control);

	void Standalone_Detach(UInteractiveControlComponent* Control);

	// UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Interaction")
	void Server_Detach(UInteractiveControlComponent* Control);

	// UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Interaction")
	void Client_Detach(UInteractiveControlComponent* Control);
#pragma endregion

#pragma region Activate Axis
	UFUNCTION(Category="Interaction")
	void Standalone_ActivateAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, float Value);

	UFUNCTION(Category="Interaction")
	void Server_ActivateAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, float Value);

	UFUNCTION(Category="Interaction")
	void Client_ActivateAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, float Value);
#pragma endregion

#pragma region Use Axis
	UFUNCTION(Category="Interaction")
	void Standalone_UseAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, float Value, uint8 UseKeyIndex = 0);

	UFUNCTION(Category="Interaction")
	void Server_UseAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, float Value, uint8 UseKeyIndex = 0);

	UFUNCTION(Category="Interaction")
	void Client_UseAxis(UInteractiveControlComponent* Control, const EInteractionInputAxisType Axis, float Value, uint8 Slot = 0);
#pragma endregion
};
