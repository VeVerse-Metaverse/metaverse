// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

 

#pragma once

#include "GameFrameworkEditorSubsystem.h"
#include "IPlaceable.h"
#include "PlayerComponent.h"
#include "TransformationEnums.h"
#include "EditorComponent.generated.h"

enum class EGizmoPlacement : uint8;

/** In-game editor component. Designed to be added to the player controller. */
UCLASS(ClassGroup=(Metaverse), meta=(BlueprintSpawnableComponent))
class VEGAMEFRAMEWORK_API UEditorComponent final : public UPlayerComponent {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UEditorComponent, FOnSelectionTransformChanged, const FTransform& /* InTransform */);

	// DECLARE_EVENT_OneParam(UEditorComponent, FOnSelectedActorsChanged, const TArray<AActor*>& /* SelectedActors */);

#pragma region Game Framework

public:
	UEditorComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Updates currently selected object transforms if any selected. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion

#pragma region Actor helpers
	/**
	 * Gets the placeable component from the supplied object
	 * @param Actor Pointer to an AActor or UActorComponent based UObject
	 * @return Placeable component pointer or nullptr if invalid object or has no placeable component.
	 */
	UPlaceableComponent* GetPlaceableComponent(AActor* Actor) const;

#pragma endregion

	DECLARE_EVENT_OneParam(UEditorComponent, FOnSelectedActorsChanged, const TArray<AActor*>& /* SelectedActors */);

public:
	FOnSelectedActorsChanged& GetOnSelectionChanged() { return OnSelectedActorsChanged; }

private:
	FOnSelectedActorsChanged OnSelectedActorsChanged;

	UPROPERTY(VisibleInstanceOnly)
	TArray<AActor*> SelectedActors;

	AActor* GetSelectedActor() const;

	/** Check if actor is currently selected. */
	bool IsActorSelected(AActor* Actor) const {
		return SelectedActors.Contains(Actor);
	}

	void SelectActor(AActor* Actor);

public:
	UFUNCTION(Server, Reliable, Category = "Editor")
	void Server_PlaceActor(const FVePlaceableClassMetadata& ClassMetadata, const FVector& WorldLocation, const FRotator& WorldRotation);

	/** Filters hit results, used in trace functions to remove incorrect and unwanted trace results. */
	void FilterHits(TArray<FHitResult>& OutHits) const;

public:
	/** Called everytime a component or actor is going to be added to the selection list, filters out unwanted actors that cannot be selected and transformed. */
	static bool ShouldSelect(const AActor* Actor);

	/**
	 * Returns the current transform domain.
	 * @note returns NONE if no transformation being applied in the current frame.
	 */
	UFUNCTION(BlueprintPure, Category = "Runtime Transformer")
	ETransformationDomain GetCurrentTransformDomain() const;

	/** @return True if currently transforming the selection using gizmo. */
	UFUNCTION(BlueprintPure, Category="Runtime Transformer")
	bool IsTransformInProgress() const { return CurrentTransformDomain != ETransformationDomain::None; }

	/** Resets transformation domain, called when there is no current transformation using gizmo taking place. */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	void ClearDomain();

#pragma region Tracing objects

private:
	/**
	 * Returns the start and end points of the Mouse based on the Player Controller possessing this pawn
	 * @note Works only at the owning client, requires valid player controller, mouse and screen.
	 * @return False if failed to get mouse screen coordinates from underlying Slate framework.
	 */
	bool Owner_GetMouseTraceWorldRay(FVector& OutRayStart, FVector& OutRayEnd) const;

public:
	/**
	 * Called by the pawn to select objects. Mouse trace for gizmo or actors/components to select.
	 * @note If there are any selected objects and gizmo is present, then it will prioritize finding a gizmo, even if it is behind an object. Otherwise the first selectable hit object will be selected.
	 * @note World space locations are used during the trace.
	 * @note Designed to be called on the owning client having a valid player controller with screen and mouse, so it won't work on dedicated servers.
	 * @note Replicated automatically if required.
	 * @return True if hit a selectable object or gizmo.
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	bool Owner_MouseTraceByObjectTypes(TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels, TArray<AActor*> IgnoredActors, bool bMultiselect = false);

	/**
	 * Called by the pawn to select objects. Mouse trace for gizmo or actors/components to select.
	 * @note If there are any selected objects and gizmo is present, then it will prioritize finding a gizmo, even if it is behind an object. Otherwise the first selectable hit object will be selected.
	 * @note World space locations are used during the trace.
	 * @note Designed to be called on the owning client having a valid player controller with screen and mouse, so it won't work on dedicated servers.
	 * @note Replicated automatically if required.
	 * @return True if hit a selectable object or gizmo.
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	bool Owner_MouseTraceByChannel(TEnumAsByte<ECollisionChannel> CollisionTraceChannel, TArray<AActor*> IgnoredActors, bool bMultiselect = false);

	/**
	 * Called by the pawn to select objects. Mouse trace for gizmo or actors/components to select.
	 * @note If there are any selected objects and gizmo is present, then it will prioritize finding a gizmo, even if it is behind an object. Otherwise the first selectable hit object will be selected.
	 * @note World space locations are used during the trace.
	 * @note Designed to be called on the owning client having a valid player controller with screen and mouse, so it won't work on dedicated servers.
	 * @note Replicated automatically if required.
	 * @return True if hit a selectable object or gizmo.
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	bool Owner_MouseTraceByProfile(const FName& CollisionProfileName, TArray<AActor*> IgnoredActors, bool bMultiselect = false);

	/**
	 * World ray trace for gizmo or actors/components to select.
	 * @note If there are any selected objects and gizmo is present, then it will prioritize finding a gizmo, even if it is behind an object. Otherwise the first selectable hit object will be selected.
	 * @note Never deselects currently selected objects if trace didnt hit anything
	 * @note World space locations are used during the trace.
	 * @return True if hit a selectable object or gizmo.
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	bool TraceByObjectTypes(const FVector& RayStartLocation, const FVector& RayEndLocation, TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels, TArray<AActor*> IgnoredActors, bool bAddToPreviouslySelectedComponents = false);

	/**
	 * World ray trace for gizmo or actors/components to select.
	 * @note If there are any selected objects and gizmo is present, then it will prioritize finding a gizmo, even if it is behind an object. Otherwise the first selectable hit object will be selected.
	 * @note Never deselects currently selected objects if trace didnt hit anything.
	 * @note World space locations are used during the trace.
	 * @return True if hit a selectable object or gizmo.
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	bool TraceByChannel(const FVector& RayStartLocation, const FVector& RayEndLocation, TEnumAsByte<ECollisionChannel> CollisionTraceChannel, TArray<AActor*> IgnoredActors, bool bAddToPreviouslySelectedComponents = false);

	/**
	 * World ray trace for gizmo or actors/components to select.
	 * @note If there are any selected objects and gizmo is present, then it will prioritize finding a gizmo, even if it is behind an object. Otherwise the first selectable hit object will be selected.
	 * @note Never deselects currently selected objects if trace didnt hit anything.
	 * @note World space locations are used during the trace.
	 * @return True if hit a selectable object or gizmo.
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	bool TraceByProfile(const FVector& RayStartLocation, const FVector& RayEndLocation, const FName& ProfileName, TArray<AActor*> IgnoredActors, bool bAddToPreviouslySelectedComponents = false);

	/**
	 * Processes the hits generated by tracing and selects either a gizmo or if there is no gizmo hit, the first placeable object hit.
	 * @note Can be called manually to process custom hit results.
	 * @return True if hit own gizmo or a placeable actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	bool ProcessTracedObjects(const TArray<FHitResult>& HitResults, bool bMultiselect = false);

#pragma endregion

	/**
	 * If the gizmo has transformation domain set it will transform the selected object(s) using this domain.
	 * The transform delta is calculated with the given ray origin and ray direction in world space (usually the mouse world position & mouse world direction).
	 * @return The delta transform calculated (after any snapping).
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	FTransform UpdateTransform(const FVector& LookingVector, const FVector& RayOrigin, const FVector& RayDirection);

	/** Applies delta transform to the selected object. */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	void ApplyDeltaTransform(const FTransform& DeltaTransform);

	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	void ApplyTransform(const FTransform& InTransform);

#pragma region Gizmo Events
	DECLARE_EVENT_ThreeParams(UEditorComponent, FOnGizmoStateChanged, ETransformType /*GizmoType*/, bool /*bTransformInProgress*/, ETransformationDomain /*Domain*/);

private:
	FOnGizmoStateChanged OnGizmoStateChanged;

public:
	FOnGizmoStateChanged& GetOnGizmoStateChanged() { return OnGizmoStateChanged; }

#pragma endregion

public:
	/**
	 * Whether to Set the System to Rotate Multiple Objects around their own axis (true)
	 * or to work rotate around where the Gizmo is at (false)

	 @see bRotateLocalAxis
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	void SetRotateOnLocalAxis(bool bRotateLocalAxis);

	/**
	 * Select Actor adds the Actor's Root Component to a list of components that will be used for the Runtime Transforms
	 * @param Actor The Actor whose Root Component will be added to the list.
	 * @param bMultiselect - If a selection happens, whether to append to the previously selected components or not
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	void SelectActor(AActor* Actor, bool bMultiselect = false);

	/**
	 * Selects all the Root Components of the Actors in given list.
	 * @see SelectActor func
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	void SelectMultipleActors(const TArray<AActor*>& Actors, bool bMultiselect = false);

	/**
	 * Deselects a given Actor's Root Component, if found on the list.
	 * @param Actor whose RootComponent to deselect
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	void DeselectActor(AActor* Actor);

	/** Deselects all the currently selected placeable actors. */
	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	void DeselectAll();

	/** Destroys all the currently selected placeable actors. */
	UFUNCTION(BlueprintCallable, Category="Runtime Transform")
	void DestroySelected();

	void Owner_DestroyPlaceableActor(AActor* Actor);

	void Owner_UpdatePlaceables(const TArray<AActor*>& Actors);

private:
	UFUNCTION(Server, Reliable, Category = "Runtime Transform")
	void RemoteServer_UpdatePlaceables(const TArray<AActor*>& Actors);

	UFUNCTION()
	void LocalServer_UpdatePlaceables(const TArray<AActor*>& Actors);

private:
	/** Notify the editor subsystem and actor's component that the actor has been selected. */
	void HandleActorSelected(AActor* Actor);

	/** Notify the editor subsystem and actor's component that the actor has been deselected. */
	void HandleActorDeselected(AActor* Actor);

	/**
	 * Creates or replaces gizmo with the current transformation tool gizmo.
	 * @note Destroys active gizmo if current transformation tool has changed.
	 */
	void UpdateGizmoActor();

	/** Updates the gizmo transform. Called on selection change. */
	void UpdateGizmoPlacement();

	/** Gets gizmo class for current transformation type. */
	UClass* GetGizmoClass() const;

	/** Resets the transform to all zeros (including scale). */
	static void ResetDeltaTransform(FTransform& Transform);

	/** Sets current transformation domain, called when gizmo axis component is clicked. */
	void SetTransformationDomain(ETransformationDomain Domain);

public:
	// 	
	//Gets the List of Actors that will be ignored in the Server Trace(for now its only the current Gizmo of Actor)
	//Since the Gizmo trace is handled locally (Gizmo appears differently to each player)
	TArray<AActor*> GetIgnoredActorsForServerTrace() const;

	// Syncs the Selected Components to the Clients (caller needs to be server)
	void LocalServer_ReplicateTraceResults(bool bTraceSuccessful, bool bMultiselect);

	UFUNCTION(Server, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteServer_TraceByObjectTypes(const FVector& WorldRayStart, const FVector& WorldRayEnd, const TArray<TEnumAsByte<ECollisionChannel>>& CollisionChannels, bool bMultiselect);

	UFUNCTION(Server, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteServer_TraceByChannel(const FVector& WorldRayStart, const FVector& WorldRayEnd, ECollisionChannel TraceChannel, bool bMultiselect);

	UFUNCTION(Server, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteServer_TraceByProfile(const FVector& WorldRayStart, const FVector& WorldRayEnd, const FName& ProfileName, bool bMultiselect);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_ClearDomain();

	void LocalServer_ClearDomain();

	UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteMulticast_ClearDomain();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_ApplyTransform(const FTransform& DeltaTransform);

	void LocalServer_ApplyTransform(const FTransform& DeltaTransform);

	UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteMulticast_ApplyTransform(const FTransform& DeltaTransform);

	/*
	 * Calls the ServerClearDomain.
	 * Then it calls ServerApplyTransform and Resets the Accumulated Network Transform.
	 * @see ServerClearDomain
	 * @see ServerApplyTransform
	 */
	UFUNCTION(BlueprintCallable, Category = "Replicated Runtime Transformer")
	void Owner_FinishTransform();

	/*
	 * ServerCall, Reliable. DeselectAll is performed in the Server.
	 * Currently no Validation takes place.
	 * @ see DeselectAll
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_DeselectAll();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_DestroySelection();

	/*
	 * Multicast, Reliable. DeselectAll is performed in the Clients.
	 * @ see DeselectAll
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteMulticast_DeselectAll();
	UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteMulticast_DestroySelection();


	/*
	 * ServerCall, Reliable. SetRotateOnLocalAxis is performed in the Server.
	 * Currently no Validation takes place.
	 * @ see SetRotateOnLocalAxis
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_SetRotateOnLocalAxis(bool bRotateLocalAxis);

	/*
	 * Multicast, Reliable. SetRotateOnLocalAxis is performed in the Clients.
	 * @ see SetRotateOnLocalAxis
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteMulticast_SetRotateOnLocalAxis(bool bRotateLocalAxis);

	/*
	* ServerCall, Reliable. CloneSelected is performed in the Server.
	* Currently no Validation takes place. 

	* WARNING: Component Cloning will NOT take place. (PluginLimitations.txt for details)

	* NOTE: The Objects must be Replicating in order to be reflected in the Clients.
	* Objects that are cloned are NOT directly handled but rather a Timer is used to check
	* when the objects have finished beginplay, so that we are sure that the networking logic has been processed for them

	* @ see CloneSelected
	* @ see CheckUnreplicatedActors
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_CloneSelected(bool bSelectNewClones = true, bool bAppendToList = false);

	/*
	 * A function called by a Timer that checks when a List of Actors have BegunPlay
	 * and have been replicated. Once all Actors that are on the Unreplicated list have been processed,
	 * the remaining functions such as MulticastSetSelectedComponents are called.
	 */
	void CheckUnreplicatedActors();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_SetDomain(ETransformationDomain Domain);

	UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteMulticast_SetDomain(ETransformationDomain Domain);

	/** Multicasts the Selected Components of the Server to all Clients. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_SyncSelectedComponents();

	/** Multicasts the Selected actors of the Server to all Clients. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_SyncSelectedActors();

	/** Multicast, Reliable. Syncs the SelectedComponents of the Server to the Clients. */
	UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteMulticast_SetSelectedComponents(const TArray<USceneComponent*>& Components);
	/** Multicast, Reliable. Syncs the SelectedComponents of the Server to the Clients. */
	UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	void RemoteMulticast_SetSelectedActors(const TArray<AActor*>& Actors);

	//Tries to resync the Selections 
	void ResyncSelection();

	//Networking Variables

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceDistance = 10000.0;

private:
	/*
	* Ignore Non-Replicated Objects means that the objects that do not satisfy
	* the replication conditions will become unselectable. This only takes effect if using the ServerTracing
	* The Replication Conditions:
	* - For an actor, replicating must be on
	* - For a component, both its owner and itself need to be replicating
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Replicated Runtime Transformer", meta = (AllowPrivateAccess = "true"))
	bool bIgnoreNonReplicatedObjects = true;

	/*
	 * Optional minimum time to wait for all Cloned objects to fully replicate and are selectable.
	 * It is not required, but there are occassions (especially when cloning multiple objects at once)
	 * where the newly spawned clone objects are not selected on the client side because the object, 
	 * although has begun play, is not still fully network addressable on the client side and so a nullptr is passed 
	 * (so no selection occurs).
	 * The time it actually takes to Replicate can be more because it also waits for all clone objects to have begun play.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Replicated Runtime Transformer", meta = (AllowPrivateAccess = "true"))
	float MinimumCloneReplicationTime;

	//The frequency at which checks are done on newly spawned clones. Whether they are suitable for replication.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Replicated Runtime Transformer", meta = (AllowPrivateAccess = "true"))
	float CloneReplicationCheckFrequency;

	FTransform NetworkDeltaTransform;

	// List of clone actor/components that need replication but haven't been replicated yet
	UPROPERTY()
	TArray<USceneComponent*> UnreplicatedComponentClones;

	FTimerHandle CheckUnrepTimerHandle;
	FTimerHandle ResyncSelectionTimerHandle;


	//Other Vars
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Transformations", meta = (AllowPrivateAccess = "true"))
	ETransformSpace CurrentTransformSpace;

	/** Transform accumulated for snapping feature */
	FTransform AccumulatedDeltaTransform;

	/** Class to spawn a translation gizmo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ATranslationGizmo> TranslationGizmoClass;

	/** Class to spawn a rotation gizmo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ARotationGizmo> RotationGizmoClass;

	/** Class to spawn a scale gizmo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AScaleGizmo> ScaleGizmoClass;

	/** Current gizmo spawned for the selected objects */
	UPROPERTY()
	TWeakObjectPtr<class AGizmoBase> Gizmo;

	/** Current transformation domain/tool, NONE if there are no selected objects or a gizmo has not been hit yet. */
	ETransformationDomain CurrentTransformDomain;

	/** Where to place a gizmo when multiple objects are selected on first or on the last one */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Transformations", meta = (AllowPrivateAccess = "true"))
	EGizmoPlacement GizmoPlacement;

	/*
	 * This property only matters when multiple objects are selected.
	 * Whether multiple objects should rotate on their local axes (true) or on the axes the Gizmo is in (false)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Transformations", meta = (AllowPrivateAccess = "true"))
	bool bMultiselectRotateOnLocalAxis;

	//Property that checks whether a CLICK on an already selected object should deselect the object or not.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Transformations", meta = (AllowPrivateAccess = "true"))
	bool bToggleSelectedInMultiSelection = true;

	//Whether we need to Sync with Server if there is a mismatch in number of Selections.
	bool bResyncSelection;

#pragma region deprecated

	/**
	* Whether to Force Mobility on items that are not Moveable
	* if true, Mobility on Components will be changed to Moveable (WARNING: does not set it back to its original mobility!)
	* if false, no movement transformations will be attempted on Static/Stationary Components

	*/
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Transformations", meta = (AllowPrivateAccess = "true"))
	// bool bForceMobility;

	/**
	 * Deselects a given Component, if found on the list.
	 * @param Component the Component to deselect
	 */
	// UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	// void DeselectComponent(class USceneComponent* Component);

	// /*
	//  * ServerCall, Reliable. SetSpaceType is performed in the Server.
	//  * Currently no Validation takes place.
	//  * @ see SetSpaceType
	//  */
	// UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	// void ServerSetSpaceType(ETransformSpace Space);
	//
	// /*
	//  * Multicast, Reliable. SetSpaceType is performed in the Clients.
	//  * @ see SetSpaceType
	//  */
	// UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	// void MulticastSetSpaceType(ETransformSpace Space);
	//
	// /*
	//  * ServerCall, Reliable. SetTransformationType is performed in the Server.
	//  * Currently no Validation takes place.
	//  * @ see SetTransformationType
	//  */
	// UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	// void ServerSetTransformationType(ETransformType Transformation);
	//
	// /*
	//  * Multicast, Reliable. SetTransformationType is performed in the Clients.
	//  * @ see SetTransformationType
	//  */
	// UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	// void MulticastSetTransformationType(ETransformType Transformation);

	/**
	 * Selects all the Components in given list.
	 * @see SelectComponent func
	 */
	// UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	// void SelectMultipleComponents(const TArray<class USceneComponent*>& Components
	// 	, bool bAppendToList = false);

	// /*
	// The core functionality, but can be called by Selection of Multiple objects
	// so as to not call UpdateGizmo every time
	// */
	// void DeselectComponent_Internal(TArray<class USceneComponent*>& OutComponentList
	// 	, class USceneComponent* Component);
	// void DeselectComponentAtIndex_Internal(TArray<class USceneComponent*>& OutComponentList
	// 	, int32 Index);

	/*
	 * ServerCall, Reliable. SetComponentBased is performed in the Server.
	 * Currently no Validation takes place.
	 * @ see SetComponentBased
	 */
	// UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	// void ServerSetComponentBased(bool bIsComponentBased);

	/*
	 * Multicast, Reliable. SetComponentBased is performed in the Clients.
	 * @ see SetComponentBased
	 */
	// UFUNCTION(NetMulticast, Reliable, Category = "Replicated Runtime Transformer")
	// void MulticastSetComponentBased(bool bIsComponentBased);

	/**
	 * Array storing Selected Components. Although a quick O(1) removal is needed (like a Set),
	 * it is Crucial that we maintain the order of the elements as they were selected
	 */
	// TArray<USceneComponent*> SelectedComponents;

	// /*
	// * Map storing the Snap values for each transformation
	// * bSnappingEnabled must be true AND, the value for the current transform MUST NOT be 0 for these values to take effect.
	// */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Transformations", meta = (AllowPrivateAccess = "true"))
	// TMap<ETransformType, float> SnappingValues;
	//
	// 	/**
	// 	* Whether Snapping an Object for each Transformation is enabled or not.
	// 	* SnappingValue for each Transformation must also NOT be zero for it to work 
	// 	* (if, snapping value is 0 for a transformation, no snapping will take place)
	//
	// 	* @see SetSnappingValue function & SnappingValues Map var
	// 	*/
	// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Transformations", meta = (AllowPrivateAccess = "true"))
	// 	TMap<ETransformType, bool> SnappingEnabled;

	// /**
	//  * Sets the Current Transformation (Translation, Rotation or Scale)
	//  */
	// UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	// void SetTransformationType(ETransformType TransformationType);

	// 	/*
	// 	 * Enables/Disables Snapping for a given Transformation
	// 	 * Snapping Value for the Given Transformation MUST NOT be 0 for Snapping to work
	//
	// 	 @see SetSnappingValue
	// 	 */
	// 	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	// 	void SetSnappingEnabled(ETransformType TransformationType, bool bSnappingEnabled);
	//
	// 	/*
	// 	 * Sets a Snapping Value for a given Transformation
	// 	 * Snapping Value MUST NOT be 0  and Snapping must be enabled for the given transformation for Snapping to work
	//
	// 	 @see SetSnappingEnabled
	// 	 */
	// 	UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	// 	void SetSnappingValue(ETransformType TransformationType, float SnappingValue);

	/*
	 * Gets the list of Selected Components.

	 @return outComponentList - the List of Currently Selected Components
	 @return outGizmoPlacedComponent - the Component in the list that currently has the Gizmo attached
	*/
	// UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	// void GetSelectedComponents(TArray<class USceneComponent*>& OutComponentList
	// , class USceneComponent*& OutGizmoPlacedComponent) const;

	// TArray<class USceneComponent*> GetSelectedComponents() const;
	//
	// /*
	// * Makes an exact copy of the Actors that are owners of the components and makes
	// * a copy of them.
	//
	// * Don't spam this :)
	// * @param bSelectNewClones - whether to add the new clones to the Selection
	// * @param bAppendToList - If the New Clones are selected, whether to Append them to the List or Clear the previous Selections
	// */
	// UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	// void CloneSelected(bool bSelectNewClones = true, bool bAppendToList = false);

	// protected:
	// 	TArray<class USceneComponent*> CloneFromList(
	// 		const TArray<class USceneComponent*>& ComponentList);
	//
	//
	// private:
	// 	TArray<class USceneComponent*> CloneActors(
	// 		const TArray<AActor*>& Actors);
	//
	// 	TArray<class USceneComponent*> CloneComponents(
	// 		const TArray<class USceneComponent*>& Components);
	//

public:
	/**
	 * Select Component adds a given Component to a list of components that will be used for the Runtime Transforms
	 * @param Component The component to add to the list.
	 * @param bAppendToList - If a selection happens, whether to append to the previously selected components or not
	*/
	// UFUNCTION(BlueprintCallable, Category = "Runtime Transformer")
	// void SelectComponent(class USceneComponent* Component, bool bAppendToList = false);

	/* 
// 	* Function Similar to MouseTraceByObjectTypes
// 	* Performs a Local Trace for Gizmos (since they appear differently for each player)
// 	* and then Performs a Server Trace for the rest of the Objects found in the Server.
//
// 	* ONLY CALL THIS if the PAWN has a Valid Player Controller. 
// 	
// 	* @see MouseTraceByObjectTypes for Param Desc
// 	*/
	// 	UFUNCTION(BlueprintCallable, Category = "Replicated Runtime Transformer")
	// 	void ReplicatedMouseTraceByObjectTypes(float TraceDistance, TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels, bool bMultiselect = false);
	//
	// 	/*
	// 	* Function Similar to MouseTraceByChannel
	// 	* Performs a Local Trace for Gizmos (since they appear differently for each player)
	// 	* and then Performs a Server Trace for the rest of the Objects found in the Server.
	//
	// 	* ONLY CALL THIS if the PAWN has a Valid Player Controller.
	// 	
	// 	* @see MouseTraceByChannel for Param Desc
	// 	*/
	// 	UFUNCTION(BlueprintCallable, Category = "Replicated Runtime Transformer")
	// 	void ReplicatedMouseTraceByChannel(float TraceDistance
	// 		, TEnumAsByte<ECollisionChannel> CollisionChannel
	// 		, bool bAppendToList = false);
	//
	// 	/*
	// 	* Function Similar to MouseTraceByProfile
	// 	* Performs a Local Trace for Gizmos (since they appear differently for each player)
	// 	* and then Performs a Server Trace for the rest of the Objects found in the Server.
	//
	// 	* ONLY CALL THIS if the PAWN has a Valid Player Controller.
	//
	// 	* @see MouseTraceByProfile for Param Desc
	// 	*/
	// 	UFUNCTION(BlueprintCallable, Category = "Replicated Runtime Transformer")
	// 	void ReplicatedMouseTraceByProfile(float TraceDistance
	// 		, const FName& ProfileName
	// 		, bool bAppendToList = false);
#pragma region Gizmo Events
	//
	// DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGizmoStateChanged, ETransformType, GizmoType, bool, bTransformInProgress, ETransformationDomain, Domain);
	//
	// UPROPERTY(BlueprintAssignable)
	// FOnGizmoStateChanged OnGizmoStateChangedDelegate;
	//
	// /*
	//  * Called when the Gizmo State has changed (i.e. Domain has changed)
	//  * @param GizmoType - the type of Gizmo that was changed (Translation, Rotation or Scale)
	//  * @param bTransformInProgress - whether the Transform is currently in progress. This is basically a bool that evaluates to Domain != NONE
	//  * @param Domain - The current domain that the Gizmo State changed to
	//  */
	// UFUNCTION(BlueprintNativeEvent, Category = "Runtime Transformer")
	// void OnGizmoStateChanged(ETransformType GizmoType, bool bTransformInProgress, ETransformationDomain Domain);
	//
	// virtual void OnGizmoStateChanged_Implementation(ETransformType GizmoType
	// 	, bool bTransformInProgress, ETransformationDomain Domain) {
	// 	OnGizmoStateChangedDelegate.Broadcast(GizmoType, bTransformInProgress, Domain);
	// 	//this should be overriden for custom logic
	// }

#pragma endregion
#pragma endregion

#pragma region TransformSpace

	DECLARE_EVENT_OneParam(FGameFrameworkEditorSubsystem, FOnTransformSpaceChanged, const ETransformSpace /* TransformSpace */);

private:
	FOnTransformSpaceChanged OnTransformSpaceChanged;
	ETransformSpace TransformSpace = ETransformSpace::World;

public:
	/** Fired when transform space changes by a toolbar or hotkey */
	FOnTransformSpaceChanged& GetOnTransformSpaceChanged() { return OnTransformSpaceChanged; }

	/** Returns current transform space */
	ETransformSpace GetTransformSpace() const { return TransformSpace; }

	/** Called to change and broadcast transform space */
	void SetTransformSpace(const ETransformSpace InTransformSpace);

#pragma endregion


	DECLARE_EVENT(FGameFrameworkEditorSubsystem, FOnCloneObjectChanged);

private:
	FOnCloneObjectChanged OnCloneObjectChanged;

public:
	/** Fired when transform space changes by a toolbar or hotkey */
	FOnCloneObjectChanged& GetOnCloneObjectChanged() { return OnCloneObjectChanged; }

	/** Called to change and broadcast transform space */
	void SetCloneObject();


#pragma region TransformTool

	DECLARE_EVENT_OneParam(FGameFrameworkEditorSubsystem, FOnTransformToolChanged, const ETransformType /* Tool */);

private:
	FOnTransformToolChanged OnTransformToolChanged;
	ETransformType CurrentTransformTool = ETransformType::Translation;

public:
	/** Fired when transform tool changes by a toolbar or hotkey */
	FOnTransformToolChanged& GetOnTransformToolChanged() { return OnTransformToolChanged; }

	/** Returns current transform tool */
	ETransformType GetTransformTool() const { return CurrentTransformTool; }

	void SetTransformTool(const ETransformType InTransformTool);

#pragma endregion

#pragma region TransformSnap

	DECLARE_EVENT_ThreeParams(UEditorComponent, FOnTransformSnapChanged, const ETransformType /* Type */, const bool /* bEnabled */, const float /* Value */);

private:
	FOnTransformSnapChanged OnTransformSnapChanged;

	bool bSnapTranslation = false;
	bool bSnapRotation = false;
	bool bSnapScale = false;

	float TranslationSnapValue = 1.0f;
	float RotationSnapValue = 45.0f;
	float ScaleSnapValue = 0.1f;

public:
	FOnTransformSnapChanged& GetOnTransformSnappingChanged() { return OnTransformSnapChanged; }

	float IsSnappingEnabled(const ETransformType Tool) const {
		switch (Tool) {
		case ETransformType::Translation:
			return bSnapTranslation;
		case ETransformType::Rotation:
			return bSnapRotation;
		case ETransformType::Scale:
			return bSnapScale;
		default:
			return 0;
		}
	}

	bool IsTranslationSnappingEnabled() const { return bSnapTranslation; }
	bool IsRotationSnappingEnabled() const { return bSnapRotation; }
	bool IsScaleSnappingEnabled() const { return bSnapScale; }

	float GetSnappingValue(const ETransformType Tool) const {
		switch (Tool) {
		case ETransformType::Translation:
			return TranslationSnapValue;
		case ETransformType::Rotation:
			return RotationSnapValue;
		case ETransformType::Scale:
			return ScaleSnapValue;
		default:
			return 0;
		}
	}

	float GetTranslationSnappingValue() const { return TranslationSnapValue; }
	float GetRotationSnappingValue() const { return RotationSnapValue; }
	float GetScaleSnappingValue() const { return ScaleSnapValue; }

	/** Sets the snapping mode for provided transform tool and broadcasts corresponding events. */
	void SetSnapping(const ETransformType Tool, const bool bEnable);

	/** Sets the snapping mode for provided transform tool and broadcasts corresponding events. */
	void SetCloning();

	/** Sets the snapping value for provided transform tool and broadcasts corresponding events. */
	void SetSnappingValue(const ETransformType Tool, const float Value);

#pragma endregion TransformSnap

#pragma region Create Placeable Object

public:
	UFUNCTION()
	void Owner_OnDrop(const FVePlaceableClassMetadata& ClassMetadata, const FVector2D& ScreenPosition);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_CreatePlaceable(const FGuid& PlaceableId);

	UFUNCTION()
	void LocalServer_CreatePlaceable(const FGuid& PlaceableId);

	// UFUNCTION()
	// void LocalClient_CreatePlaceable(const FVePlaceableMetadata& Metadata);

protected:
	void Owner_RequestCreatePlaceable(const FVePlaceableClassMetadata& ClassMetadata, const FVector& Location, const FRotator& Rotation);
	void CreatePlaceable(const FVePlaceableMetadata& Metadata);
	void CreateTemporaryPlaceable(const FVePlaceableMetadata& Metadata);


#pragma endregion Create Placeable Object

#pragma region Transform Placeable Object

public:
	void Owner_OnTransformComplete();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replicated Runtime Transformer")
	void RemoteServer_OnTransformComplete(AActor* Actor, const FTransform& Transform);

	UFUNCTION()
	void LocalServer_OnTransformComplete(AActor* Actor, const FTransform& Transform);

	/** Fired when transform changed using transform widget or gizmo */
	FOnSelectionTransformChanged& GetOnSelectionTransformChanged() { return OnSelectionTransformChanged; }

private:
	FOnSelectionTransformChanged OnSelectionTransformChanged;

#pragma endregion Transform Placeable Object

private:
	void DebugHitHelper(const FHitResult& HitResult);

};
