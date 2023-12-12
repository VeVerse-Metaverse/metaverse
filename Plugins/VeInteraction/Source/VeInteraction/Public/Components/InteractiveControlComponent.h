// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
#include "MotionControllerComponent.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "Enums/InteractionType.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Structs/InteractiveFingerSettings.h"


#include "InteractiveControlComponent.generated.h"

class APawn;
class AVRMotionController;
class UInteractiveControlComponent;
class UInteractiveTargetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMenuModeChange, FName, OldMode, FName, NewMode, UInteractiveControlComponent*, ControlComponent,
											  UInteractiveTargetComponent*, TargetComponent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteract, UInteractiveControlComponent*, ControlComponent, UInteractiveTargetComponent*, TargetComponent);

/** This component attaches to the pawn and works as the source of interactions performed by the pawn.  */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class VEINTERACTION_API UInteractiveControlComponent final : public UActorComponent {
	GENERATED_BODY()

public:
	explicit UInteractiveControlComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/** Changes when focused target changes. */
	bool bFocusedNewTarget;

	/** Changes when triggered target changes. */
	bool bTriggeredNewTarget;

#pragma region Vectors
	/** Focus origin vector (pawn camera or motion controller). */
	UPROPERTY()
	FVector FocusOrigin;

	/** Focus location vector (where the cast ray hits the surface of the object). */
	UPROPERTY()
	FVector FocusLocation;

	/** Focus normal vector (normal of the surface the cast ray hits the surface of the object). */
	UPROPERTY()
	FVector FocusNormal;

	/** Overlap origin vector (pawn camera or motion controller). */
	UPROPERTY()
	FVector OverlapOrigin;

	/** Overlap location vector (where the cast ray hits the surface of the object). */
	UPROPERTY()
	FVector OverlapLocation;

	/** Overlap normal vector (normal of the surface the cast ray hits the surface of the object). */
	UPROPERTY()
	FVector OverlapNormal;

	/** Focus origin vector (pawn camera or motion controller). */
	UPROPERTY()
	FVector ActivateOrigin;

	/** Focus location vector (where the cast ray hits the surface of the object). */
	UPROPERTY()
	FVector ActivateLocation;

	/** Focus normal vector (normal of the surface the cast ray hits the surface of the object). */
	UPROPERTY()
	FVector ActivateNormal;

	/** Grab origin vector (pawn camera or motion controller). */
	UPROPERTY()
	FVector GrabOrigin;

	/** Grab location vector (where the cast ray hits the surface of the object). */
	UPROPERTY()
	FVector GrabLocation;

	/** Grab normal vector (normal of the surface the cast ray hits the surface of the object). */
	UPROPERTY()
	FVector GrabNormal;

	/** Grabbed object location. */
	UPROPERTY()
	FVector GrabbedObjectLocation;

	/** Grabbed object scale. */
	UPROPERTY()
	FVector GrabbedObjectScale;

	/** Grabbed object rotation. */
	UPROPERTY()
	FRotator GrabbedObjectRotation;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_RightHandFingerSettings, Category="Fingers")
	FInteractiveFingerSettings RightHandFingerSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_LeftHandFingerSettings, Category="Fingers")
	FInteractiveFingerSettings LeftHandFingerSettings;

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_RightHandFingerSettings();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_LeftHandFingerSettings();

#pragma endregion

public:
	FORCEINLINE void SetControlMode(const EInteractionControlMode NewControlMode) { ControlMode = NewControlMode; }

	UPROPERTY()
	bool bIsAuthority = false;

#if WITH_EDITORONLY_DATA
	/** Should the debug object for focus trace be drawn or not. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugDrawFocusTrace = true;
#endif

	/** Physics handle to attach objects. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intercast")
	UPhysicsHandleComponent* PhysicsHandle;

#pragma region Menu
	/** Enables menu with specified mode. */
	UFUNCTION(BlueprintCallable)
	void SetMenuMode(FName Mode);

	/** Menu mode change callback. */
	UPROPERTY(BlueprintAssignable)
	FOnMenuModeChange OnMenuModeChange;

	/** Current menu mode. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName MenuMode;
#pragma endregion

#pragma region Control Owner Getters
	/** Get the owning motion controller. */
	UFUNCTION(BlueprintPure)
	AActor* GetMotionController() const;

	/** Get the owning pawn. */
	UFUNCTION(BlueprintPure)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure)
	APlayerController* GetPlayerController() const;
#pragma endregion

#pragma region Event bindings
	/** Pawn overlap begin delegate event. */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
						const FHitResult& SweepResult);

	/** Pawn overlap end delegate event. */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**
	 * Receives input begin event from the pawn.
	 * @param Action Type of the action to replicate.
	 * @param ProxyControl Used when the source is not the same as the current (pawn control), used for motion controller control component that can't do a server RPC by its own and need the pawn actor channel.
	 */
	UFUNCTION()
	void OnInputBegin(EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl = nullptr);
	
	/** User Input Begin*/
	void OnMotionControllerUserInputBegin(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl = nullptr);
	void OnMobileUserInputBegin(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl = nullptr);
	void OnPawnUserInputBegin(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl = nullptr);

	/** User Input Begin Local*/
	void Standalone_UserInputBeginLocal(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey, const FVector& InOrigin,
										const FVector& InLocation, const FVector& InNormal);
	void Client_UserInputBeginLocal(UInteractiveTargetComponent* SelectedActivateTarget, const FName& Name, const FKey& Key, const FVector_NetQuantize InOrigin,
									const FVector_NetQuantize InLocation, const FVector_NetQuantizeNormal InNormal);
	void Server_UserInputBeginLocal(const FName& InName, const FKey& InKey);

	
	/** User Input Begin Remote*/
	UFUNCTION(Server, Reliable, Category = "Intercast")
	void Server_UserInputBeginRemote(const FName& Name, const FKey& Key, UInteractiveControlComponent* ProxyControl = nullptr);
	
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_UserInputBeginRemote(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey, const FVector_NetQuantize& InOrigin,
										const FVector_NetQuantize& InLocation, const FVector_NetQuantizeNormal& InNormal);
	
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_UserInputBeginRemote(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey, const FVector_NetQuantize& InOrigin,
									 const FVector_NetQuantize& InLocation, const FVector_NetQuantizeNormal& InNormal);

	UPROPERTY(BlueprintAssignable)
	FOnInteract OnUserInputBeginDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInteract OnUserInputEndDelegate;


	/** Call this from the input binding to send a begin user input event. */
	UFUNCTION(BlueprintCallable)
	void OnUserInputBegin(FName InInput, FKey InKey, UInteractiveControlComponent* InProxyControl = nullptr);

	UFUNCTION()
	void OnPawnInputBegin(EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl = nullptr);

	UFUNCTION()
	void OnMotionControllerInputBegin(EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl = nullptr);

	UFUNCTION()
	void OnMobileInputBegin(EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl = nullptr);
	
	/** User Input End Local*/
	void Standalone_UserInputEndLocal(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey);
	void Client_UserInputEndLocal(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey);
	void Server_UserInputEndLocal(const FName& InName, const FKey& InKey);
	
	/** User Input End Remote*/
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_UserInputEndRemote(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey);
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_UserInputEndRemote(UInteractiveTargetComponent* InProxyControl, const FName& InName, const FKey& InKey);
	UFUNCTION(Server, Reliable, Category = "Intercast")
	void Server_UserInputEndRemote(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl);

	/** User Input End*/
	void OnMotionControllerUserInputEnd(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl = nullptr);
	void OnMobileUserInputEnd(const FName& Name, const FKey& Key, UInteractiveControlComponent* ProxyControl = nullptr);
	void OnPawnUserInputEnd(const FName& InName, const FKey& InKey, UInteractiveControlComponent* InProxyControl = nullptr);

	/** Call this from the input binding to send a end user input event. */
	UFUNCTION(BlueprintCallable)
	void OnUserInputEnd(FName InInput, FKey InKey, UInteractiveControlComponent* ProxyControl = nullptr);

	/**
	 * Receives input end event from the pawn.
	 * @param Action Type of the action to replicate.
	 * @param ProxyControl Used when the source is not the same as the current (pawn control), used for motion controller control component that can't do a server RPC by its own and need the pawn actor channel.
	 */
	UFUNCTION(BlueprintCallable)
	void OnInputEnd(EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl = nullptr);

	UFUNCTION()
	void OnPawnInputEnd(EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl = nullptr);

	UFUNCTION()
	void OnMotionControllerInputEnd(EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl = nullptr);

	UFUNCTION()
	void OnMobileInputEnd(EInteractionInputActionType Action, UInteractiveControlComponent* ProxyControl = nullptr);

	UFUNCTION()
	void OnCustomInputEnd(FKey InKey, UInteractiveControlComponent* ProxyControl = nullptr);

	/** Receives input axis value from the pawn. todo: Think about resetting the value after some time so not to spam? */
	UFUNCTION(BlueprintCallable)
	void OnInputAxis(EInteractionInputAxisType Axis, float Value, UInteractiveControlComponent* ProxyControl = nullptr);

	UFUNCTION()
	void OnPawnInputAxis(EInteractionInputAxisType Axis, float Value, UInteractiveControlComponent* ProxyControl = nullptr);

	UFUNCTION()
	void OnMotionControllerInputAxis(EInteractionInputAxisType Axis, float Value, UInteractiveControlComponent* ProxyControl = nullptr);

	UFUNCTION()
	void OnMobileInputAxis(EInteractionInputAxisType Axis, float Value, UInteractiveControlComponent* ProxyControl = nullptr);

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// float AxisForwardValue = 0.0f;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// float AxisRightValue = 0.0f;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// float AxisUpValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AxisTolerance = 0.001f;

	const float AxisZeroValue = 0.0f;

#pragma endregion

#pragma region Selections
	/** Select the interaction target component we should send the activate events to. */
	UInteractiveTargetComponent* SelectActivateTarget();

	/** Select the interaction target component we should send the grab events to. */
	UInteractiveTargetComponent* SelectGrabTarget();

	/** Select the interaction target component we should send the use events to. */
	UInteractiveTargetComponent* SelectUseTarget() const;
#pragma endregion

#pragma region Targets
	/** Target focused by the pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Intercast")
	UInteractiveTargetComponent* FocusedTarget;

	/** The primary overlapped actor target that will receive interaction events. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Intercast")
	UInteractiveTargetComponent* OverlappedTarget;

	/** The target currently being interacted. Set by ActivateBegin, reset by ActivateEnd. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Intercast")
	UInteractiveTargetComponent* ActivateTarget;

	/** The target currently being grabbed. Set by GrabBegin, reset by GrabEnd. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Intercast")
	UInteractiveTargetComponent* GrabTarget;

	/** The target currently attached and can be used. Set by Attach, reset by Detach. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Intercast")
	UInteractiveTargetComponent* UseTarget;

	/** All overlapping actor targets. Written by Begin/End Overlap, Read by TickComponent. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Intercast")
	TArray<UInteractiveTargetComponent*> OverlappingTargets;
#pragma endregion

#pragma region Vector Getters
	UFUNCTION(BlueprintGetter, Category = "Intercast")
	FVector GetFocusLocation() const { return FocusLocation; }

	UFUNCTION(BlueprintGetter, Category = "Intercast")
	FVector GetFocusNormal() const { return FocusNormal; }

	UFUNCTION(BlueprintGetter, Category = "Intercast")
	FVector GetFocusOrigin() const { return FocusOrigin; }

	UFUNCTION(BlueprintGetter, Category = "Intercast")
	FVector GetOverlapLocation() const { return OverlapLocation; }

	UFUNCTION(BlueprintGetter, Category = "Intercast")
	FVector GetOverlapNormal() const { return OverlapNormal; }

	UFUNCTION(BlueprintGetter, Category = "Intercast")
	FVector GetOverlapOrigin() const { return OverlapOrigin; }

	UFUNCTION(BlueprintGetter, Category = "Intercast")
	FVector GetGrabLocation() const { return GrabLocation; }

	UFUNCTION(BlueprintGetter, Category = "Intercast")
	FVector GetGrabNormal() const { return GrabNormal; }

	UFUNCTION(BlueprintGetter, Category = "Intercast")
	FVector GetGrabOrigin() const { return GrabOrigin; }
#pragma endregion

#pragma region Target Getters
	/** Get the target focused by the owning pawn. */
	UFUNCTION(BlueprintPure, Category = "Intercast")
	UInteractiveTargetComponent* GetPawnFocusedTarget();

	/** Get the closest target overlapped by the owning pawn. */
	UFUNCTION(BlueprintPure, Category = "Intercast")
	UInteractiveTargetComponent* GetPawnOverlappedTarget();

	/** Get the target focused by the owning motion controller. */
	UFUNCTION(BlueprintPure, Category = "Intercast")
	UInteractiveTargetComponent* GetMotionControllerFocusedTarget();

	/** Get the closest target overlapped by the owning motion controller. */
	UFUNCTION(BlueprintPure, Category = "Intercast")
	UInteractiveTargetComponent* GetMotionControllerOverlappedTarget();
#pragma endregion

#pragma region Attach/Detach RPC
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void Standalone_AttachLocal(UInteractiveTargetComponent* InteractiveTarget = nullptr);

	/** Attach the target to the control owner at the server. */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category="Interaction")
	void Server_AttachLocal(UInteractiveTargetComponent* InteractiveTarget = nullptr);

	/** Attach the target to the control owner at the owning client. */
	UFUNCTION(Client, Reliable, Category = "Interaction")
	void Client_AttachRemote(UInteractiveTargetComponent* InteractiveTarget = nullptr);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void Client_AttachLocal(UInteractiveTargetComponent* InteractiveTarget = nullptr);

	/** Attach the target to the control owner at any client. */
	UFUNCTION(NetMulticast, Reliable, Category = "Interaction")
	void Multicast_AttachRemote(UInteractiveTargetComponent* InteractiveTarget = nullptr);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void Standalone_DetachLocal(UInteractiveTargetComponent* InteractiveTarget = nullptr);

	/** Detach the interactive target from this control at the server. */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category="Interaction")
	void Server_DetachLocal(UInteractiveTargetComponent* InteractiveTarget = nullptr);

	/**
	 * Server request the interactive target to detach from this control at the client.
	 * @remarks Used on server to ask owning client to detach the actor.
	 */
	UFUNCTION(Client, Reliable, Category = "Interaction")
	void Client_DetachRemote(UInteractiveTargetComponent* InteractiveTarget = nullptr);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void Client_DetachLocal(UInteractiveTargetComponent* InteractiveTarget = nullptr);

	/**
	 * Server request the interactive target to detach from this control at all connected clients.
	 * @remarks Used on server to ask every client to detach the actor.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Interaction")
	void Multicast_DetachRemote(UInteractiveTargetComponent* InteractiveTarget = nullptr);
#pragma endregion

#pragma region Helpers
	void AttachTargetToMotionController(UInteractiveTargetComponent* InteractiveTarget, UMotionControllerComponent* MotionControllerComponent);
	void AttachTargetToPawn(UInteractiveTargetComponent* InteractiveTarget);

	/** Helper method to attach the target to the control owner. */
	UFUNCTION(BlueprintCallable)
	void AttachTarget(UInteractiveTargetComponent* InteractiveTarget);
	void DetachTargetFromMotionController(UInteractiveTargetComponent* InteractiveTarget);
	void DetachTargetFromPawn(UInteractiveTargetComponent* InteractiveTarget) const;

	/** Helper method to attach the target to the control owner. */
	UFUNCTION(BlueprintCallable)
	void DetachTarget(UInteractiveTargetComponent* InteractiveTarget);

	UFUNCTION(BlueprintCallable)
	void OnTargetDestroyed(UInteractiveTargetComponent* InteractiveTargetComponent);

	UFUNCTION(Client, Reliable)
	void Client_OnTargetDestroyed(UInteractiveTargetComponent* InteractiveTargetComponent);

	UFUNCTION(BlueprintPure)
	bool IsDedicatedServer() const;
#pragma endregion

#pragma region RPC

#pragma region Custom RPC
	/** Standalone game event. */
	void Standalone_CustomEventLocal(UInteractiveTargetComponent* InteractiveTarget,
									 const FString& Topic,
									 const TArray<uint8>& Parameters);

	/** Server side method that does actual event processing. */
	void Server_CustomEventLocal(UInteractiveTargetComponent* InteractiveTarget,
								 const FString& Topic,
								 const TArray<uint8>& Parameters);

	/** Any client local event. */
	void Client_CustomEventLocal(UInteractiveTargetComponent* InteractiveTarget,
								 const FString& Topic,
								 const TArray<uint8>& Parameters);

	/**
	 * Server only event. Proxies the event if required.
	 * @param ProxyControl Server side instance of the endpoint control component. Can be nullptr if event is not proxied and should be processed in place.
	 * @param InteractiveTarget
	 * @param Topic
	 * @param Parameters
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Intercast")
	void Server_CustomEventRemote(UInteractiveTargetComponent* InteractiveTarget,
								  const FString& Topic,
								  const TArray<uint8>& Parameters,
								  UInteractiveControlComponent* ProxyControl = nullptr);

	/** RPC called from the server. Owning client only event. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_CustomEventRemote(UInteractiveTargetComponent* InteractiveTarget,
								  const FString& Topic,
								  const TArray<uint8>& Parameters);

	/** RPC called from the server. Multicast client event. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_CustomEventRemote(UInteractiveTargetComponent* InteractiveTarget,
									 const FString& Topic,
									 const TArray<uint8>& Parameters);
#pragma endregion

#pragma region Focus Begin
	/** Standalone game event, called when interaction target enters control focus. */
	void Standalone_FocusBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
									FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
									FVector_NetQuantize InFocusLocation = FVector_NetQuantize(),
									FVector_NetQuantizeNormal InFocusNormal = FVector_NetQuantizeNormal());

	/** Server side method that does actual input begin event processing. */
	void Server_FocusBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
								FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
								FVector_NetQuantize InFocusLocation = FVector_NetQuantize(),
								FVector_NetQuantizeNormal InFocusNormal = FVector_NetQuantizeNormal());

	/** Any client local event, called when interaction target enters control focus. */
	void Client_FocusBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
								FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
								FVector_NetQuantize InFocusLocation = FVector_NetQuantize(),
								FVector_NetQuantizeNormal InFocusNormal = FVector_NetQuantizeNormal());

	/** RPC called from the server. Multicast client event, called when interaction target enters control focus. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_FocusBeginRemote(UInteractiveTargetComponent* InteractiveTarget,
									FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
									FVector_NetQuantize InFocusLocation = FVector_NetQuantize(),
									FVector_NetQuantizeNormal InFocusNormal = FVector_NetQuantizeNormal());

	/** RPC called from the server. Owning client only event, called when interaction target enters control focus. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_FocusBeginRemote(UInteractiveTargetComponent* InteractiveTarget,
								 FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
								 FVector_NetQuantize InFocusLocation = FVector_NetQuantize(),
								 FVector_NetQuantizeNormal InFocusNormal = FVector_NetQuantizeNormal());
#pragma endregion

#pragma region Focus End
	/** Server only event, called when interaction target leaves control focus. */
	void Standalone_FocusEndLocal(UInteractiveTargetComponent* InteractiveTarget);

	/** Server only event, called when interaction target leaves control focus. */
	void Server_FocusEndLocal(UInteractiveTargetComponent* InteractiveTarget);

	/** ... */
	void Client_FocusEndLocal(UInteractiveTargetComponent* InteractiveTarget);

	/** Owning client only event, called when interaction target leaves control focus. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_FocusEndRemote(UInteractiveTargetComponent* InteractiveTarget);

	/** Multicast client event, called when interaction target leaves control focus. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_FocusEndRemote(UInteractiveTargetComponent* InteractiveTarget);
#pragma endregion

#pragma region Overlap Begin
	void Standalone_OverlapBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
									  FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
									  FVector_NetQuantize InFocusLocation = FVector_NetQuantize(),
									  FVector_NetQuantizeNormal InFocusNormal = FVector_NetQuantizeNormal());

	/** Server only event, called when interaction target overlaps with the control. */
	void Server_OverlapBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
								  FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
								  FVector_NetQuantize InFocusLocation = FVector_NetQuantize(),
								  FVector_NetQuantizeNormal InFocusNormal = FVector_NetQuantizeNormal());

	/** ... */
	void Client_OverlapBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
								  FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
								  FVector_NetQuantize InFocusLocation = FVector_NetQuantize(),
								  FVector_NetQuantizeNormal InFocusNormal = FVector_NetQuantizeNormal());

	/** Owning client only event, called when interaction target overlaps with the pawn. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_OverlapBeginRemote(UInteractiveTargetComponent* InteractiveTarget,
								   FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
								   FVector_NetQuantize InFocusLocation = FVector_NetQuantize(),
								   FVector_NetQuantizeNormal InFocusNormal = FVector_NetQuantizeNormal());

	/** Multicast client event, called when interaction target overlaps with the pawn. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_OverlapBeginRemote(UInteractiveTargetComponent* InteractiveTarget,
									  FVector_NetQuantize InFocusOrigin = FVector_NetQuantize(),
									  FVector_NetQuantize InFocusLocation = FVector_NetQuantize(),
									  FVector_NetQuantizeNormal InFocusNormal = FVector_NetQuantizeNormal());
#pragma endregion

#pragma region Overlap End
	/** ... */
	void Standalone_OverlapEndLocal(UInteractiveTargetComponent* InteractiveTarget);

	/** Server only event, called when interaction target stops overlapping with the pawn. */
	void Server_OverlapEndLocal(UInteractiveTargetComponent* InteractiveTarget);

	/** Client only event, called when interaction target stops overlapping with the pawn. */
	void Client_OverlapEndLocal(UInteractiveTargetComponent* InteractiveTarget);

	/** Owning client only event, called when interaction target stops overlapping with the pawn. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_OverlapEndRemote(UInteractiveTargetComponent* InteractiveTarget);

	/** Multicast client event, called when interaction target stops overlapping with the pawn. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_OverlapEndRemote(UInteractiveTargetComponent* InteractiveTarget);
#pragma endregion

#pragma region Activate Begin
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnActivateBegin;

	/** Server side method that does actual input begin event processing. Selects the interactive target by itself. */
	void Server_ActivateBeginLocal();

	void Standalone_ActivateBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
									   FVector_NetQuantize InOrigin = FVector_NetQuantize(),
									   FVector_NetQuantize InLocation = FVector_NetQuantize(),
									   FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());


	/** ... */
	void Client_ActivateBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
								   FVector_NetQuantize InOrigin = FVector_NetQuantize(),
								   FVector_NetQuantize InLocation = FVector_NetQuantize(),
								   FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	/**
	 * Server only event, called when the pawn interacts with the target. Proxies the event if required.
	 * @param ProxyControl Server side instance of the endpoint control component. Can be nullptr if event is not proxied and should be processed in place.
	 */
	UFUNCTION(Server, Reliable, Category = "Intercast")
	void Server_ActivateBeginRemote(UInteractiveControlComponent* ProxyControl = nullptr);

	/** Owning client only event, called when the pawn interacts with the target. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_ActivateBeginRemote(UInteractiveTargetComponent* Target,
									FVector_NetQuantize InOrigin = FVector_NetQuantize(),
									FVector_NetQuantize InLocation = FVector_NetQuantize(),
									FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	/** Multicast client event, called the pawn interacts with the target. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_ActivateBeginRemote(UInteractiveTargetComponent* Target,
									   FVector_NetQuantize InOrigin = FVector_NetQuantize(),
									   FVector_NetQuantize InLocation = FVector_NetQuantize(),
									   FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

#pragma endregion

#pragma region Activate End
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnActivateEnd;

	void Standalone_ActivateEndLocal(UInteractiveTargetComponent* InteractiveTarget);

	/** Server side method that does actual input end event processing. */
	void Server_ActivateEndLocal();

	/** Server side method that does actual input end event processing. */
	void Client_ActivateEndLocal(UInteractiveTargetComponent* InteractiveTarget);

	/**
	 * Server only event, called when the pawn interacts with the target.
	 * @param ProxyControl Server side instance of the endpoint control component. Can be nullptr if event is not proxied and should be processed in place.
	 */
	UFUNCTION(Server, Reliable, Category = "Intercast")
	void Server_ActivateEndRemote(UInteractiveControlComponent* ProxyControl = nullptr);

	/** Owning client only event, called when the pawn interacts with the target. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_ActivateEndRemote(UInteractiveTargetComponent* InteractiveTarget);

	/** Multicast client event, called the pawn interacts with the target. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_ActivateEndRemote(UInteractiveTargetComponent* InteractiveTarget);
#pragma endregion

#pragma region Activate Axis
	void Standalone_ActivateAxisLocal(UInteractiveTargetComponent* InteractiveTarget,
									  EInteractionInputAxisType Axis, float Value = 0.f);

	/** Server side method to process activate axis events. */
	void Server_ActivateAxisLocal(EInteractionInputAxisType Axis, float Value = 0.f);
	/** Client side method to process activate axis events. */
	void Client_ActivateAxisLocal(UInteractiveTargetComponent* InteractiveTarget,
								  EInteractionInputAxisType Axis, float Value = 0.f);

	/** Server only event, called when the pawn interacts with the target using an axis input. */
	UFUNCTION(Server, Reliable, Category = "Intercast")
	void Server_ActivateAxisRemote(EInteractionInputAxisType Axis, float Value = 0.f,
								   UInteractiveControlComponent* ProxyControl = nullptr);

	/** Owning client only event, called when the pawn interacts with the target using an axis input. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_ActivateAxisRemote(UInteractiveTargetComponent* Target,
								   EInteractionInputAxisType Axis, float Value = 0.f);

	/** Multicast client event, called the pawn interacts with the target using an axis input. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_ActivateAxisRemote(UInteractiveTargetComponent* Target,
									  EInteractionInputAxisType Axis, float Value = 0.f);

#pragma endregion

#pragma region Grab Begin
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnGrabBegin;

	void Standalone_GrabBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
								   FVector_NetQuantize InOrigin = FVector_NetQuantize(),
								   FVector_NetQuantize InLocation = FVector_NetQuantize(),
								   FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	/**
	 * Server only event, called when the pawn interacts with the target. Proxies the event if required.
	 * @param ProxyControl Server side instance of the endpoint control component. Can be nullptr if event is not proxied and should be processed in place.
	 */
	UFUNCTION(Server, Reliable, Category = "Intercast")
	void Server_GrabBeginRemote(UInteractiveControlComponent* ProxyControl = nullptr);

	/** Server side method that does actual input begin event processing. Selects the interactive target by itself. */
	void Server_GrabBeginLocal();

	/** Multicast client event, called the pawn interacts with the target. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_GrabBeginRemote(UInteractiveTargetComponent* InteractiveTarget,
								   FVector_NetQuantize InOrigin = FVector_NetQuantize(),
								   FVector_NetQuantize InLocation = FVector_NetQuantize(),
								   FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	/** Owning client only event, called when the pawn interacts with the target. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_GrabBeginRemote(UInteractiveTargetComponent* InteractiveTarget,
								FVector_NetQuantize InOrigin = FVector_NetQuantize(),
								FVector_NetQuantize InLocation = FVector_NetQuantize(),
								FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

	/** ... */
	void Client_GrabBeginLocal(UInteractiveTargetComponent* InteractiveTarget,
							   FVector_NetQuantize InOrigin = FVector_NetQuantize(),
							   FVector_NetQuantize InLocation = FVector_NetQuantize(),
							   FVector_NetQuantizeNormal InNormal = FVector_NetQuantizeNormal());

#pragma endregion

#pragma region Grab End
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnGrabEnd;

	void Standalone_GrabEndLocal(UInteractiveTargetComponent* InteractiveTarget);

	/**
	 * Server only event, called when the pawn interacts with the target.
	 * @param ProxyControl Server side instance of the endpoint control component. Can be nullptr if event is not proxied and should be processed in place.
	 */
	UFUNCTION(Server, Reliable, Category = "Intercast")
	void Server_GrabEndRemote(UInteractiveControlComponent* ProxyControl = nullptr);

	/** Server side method that does actual input end event processing. */
	void Server_GrabEndLocal();

	/** Multicast client event, called the pawn interacts with the target. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_GrabEndRemote(UInteractiveTargetComponent* InteractiveTarget);

	/** Owning client only event, called when the pawn interacts with the target. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_GrabEndRemote(UInteractiveTargetComponent* InteractiveTarget);

	/** Server side method that does actual input end event processing. */
	void Client_GrabEndLocal(UInteractiveTargetComponent* InteractiveTarget);
#pragma endregion

#pragma region Use Begin
	void Standalone_UseBeginLocal(UInteractiveTargetComponent* InteractiveTarget, uint8 UseIndex = 0);

	/**
	 * Server only event, called when the pawn interacts with the attached target. Routes to self or proxy as required.
	 * @param ProxyControl Server side instance of the endpoint control component. Can be nullptr if event is not proxied and should be processed in place.
	 * @param UseIndex
	 */
	UFUNCTION(Server, Reliable, Category = "Intercast")
	void Server_UseBeginRemote(UInteractiveControlComponent* ProxyControl = nullptr, uint8 UseIndex = 0);

	/** Server side method that does actual input begin event processing. */
	void Server_UseBeginLocal(uint8 UseIndex = 0);

	/** Multicast client event, called the pawn interacts with the target. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_UseBeginRemote(UInteractiveTargetComponent* InteractiveTarget, uint8 UseIndex = 0);

	/** Owning client only event, called when the pawn interacts with the target. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_UseBeginRemote(UInteractiveTargetComponent* InteractiveTarget, uint8 UseIndex = 0);

	void Client_UseBeginLocal(UInteractiveTargetComponent* InteractiveTarget, uint8 UseIndex = 0);

#pragma endregion

#pragma region Use End
	void Standalone_UseEndLocal(UInteractiveTargetComponent* InteractiveTarget, uint8 UseIndex = 0);

	/**
	 * Server only event, called when the pawn interacts with the attached target.
	 * @param ProxyControl Server side instance of the endpoint control component. Can be nullptr if event is not proxied and should be processed in place.
	 * @param UseIndex
	 */
	UFUNCTION(Server, Reliable, Category = "Intercast")
	void Server_UseEndRemote(UInteractiveControlComponent* ProxyControl = nullptr, uint8 UseIndex = 0);

	/** Server side method that does actual input end event processing. */
	void Server_UseEndLocal(uint8 UseIndex);

	/** Owning client only event, called when the pawn interacts with the target. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_UseEndRemote(UInteractiveTargetComponent* InteractiveTarget, uint8 UseIndex = 0);

	void Client_UseEndLocal(UInteractiveTargetComponent* InteractiveTarget, uint8 UseIndex = 0);

	/** Multicast client event, called the pawn interacts with the target. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_UseEndRemote(UInteractiveTargetComponent* InteractiveTarget, uint8 UseIndex = 0);

#pragma endregion

#pragma region Use Axis
	void Standalone_UseAxisLocal(UInteractiveTargetComponent* InteractiveTarget,
								 EInteractionInputAxisType Axis, float Value = 0.f);

	/** Server side method to process activate axis events. */
	auto Server_UseAxisLocal(EInteractionInputAxisType Axis, float Value = 0.f) -> void;
	/** Client side method to process activate axis events. */
	void Client_UseAxisLocal(UInteractiveTargetComponent* InteractiveTarget,
							 EInteractionInputAxisType Axis, float Value = 0.f);

	/** Server only event, called when the pawn interacts with the target using an axis input. */
	UFUNCTION(Server, Reliable, Category = "Intercast")
	void Server_UseAxisRemote(EInteractionInputAxisType Axis, float Value = 0.f,
							  UInteractiveControlComponent* ProxyControl = nullptr);

	/** Owning client only event, called when the pawn interacts with the target using an axis input. */
	UFUNCTION(Client, Reliable, Category = "Intercast")
	void Client_UseAxisRemote(UInteractiveTargetComponent* Target,
							  EInteractionInputAxisType Axis, float Value = 0.f);

	/** Multicast client event, called the pawn interacts with the target using an axis input. */
	UFUNCTION(NetMulticast, Reliable, Category = "Intercast")
	void Multicast_UseAxisRemote(UInteractiveTargetComponent* Target,
								 EInteractionInputAxisType Axis, float Value = 0.f);

#pragma endregion

#pragma region Options
	/** Should this component check for client side overlapped targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bProcessClientOverlap = true;

	/** Should this component check for client side focused targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bProcessClientFocus = true;

	/** Component to use when attaching object to a pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* PawnAttachComponent;

	/** Socket to use when attaching object to a pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PawnAttachSocketName = NAME_None;

	/** Component to use when attaching object to a left hand. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* LeftHandAttachComponent;

	/** Socket to use when attaching object to a pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LeftHandAttachSocketName = NAME_None;

	/** Component to use when attaching object to a right hand. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RightHandAttachComponent;

	/** Socket to use when attaching object to a pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RightHandAttachSocketName = NAME_None;

	/** Is the component used for VR interactions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Deprecated))
	bool bIsVR = false;

	/** Is the component used for Mobile interactions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Deprecated))
	bool bIsMobile = false;

	/** Allows to determine the mode this control is functioning in. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInteractionControlMode ControlMode;

	/** The current platform. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EInteractionPlatform Platform;

	/** Enables the GrabEnd events to be called at the use (attached) target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableUseTargetGrabEndEvents = false;

	/** Should the component adapt for the third person interactions or not. Applicable only for pawn and pawn mobile modes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsThirdPerson = false;

	/** Should the pawn focus be used or not. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Intercast|Options")
	bool bUseFocus = true;

	/** Should the pawn overlap be used or not. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Intercast|Options")
	bool bUseOverlap = true;

	/** Is the interaction enabled or not. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intercast|Options")
	bool bUseInput = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Intercast|Options")
	float MaxFocusDistance = 200.0f;

	/** Controls minimal direction sector to select triggerable objects from: -1 is 360deg, 0 is 180deg, etc. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Intercast|Options")
	float MinOverlapDirection = -1.f;
#pragma endregion

};
