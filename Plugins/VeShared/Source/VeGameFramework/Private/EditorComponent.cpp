// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "EditorComponent.h"
#include "TimerManager.h"
#include "VeGameFramework.h"
#include "Gizmos/RotationGizmo.h"
#include "Gizmos/ScaleGizmo.h"
#include "Gizmos/TranslationGizmo.h"
#include "Kismet/GameplayStatics.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Gizmos/GizmoPlacement.h"
#include "Api2PlaceableSubsystem.h"
#include "PlaceableComponent.h"
#include "PlaceableLib.h"
#include "DrawDebugHelpers.h"
#include "VeWorldSettings.h"


UEditorComponent::UEditorComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	// PrimaryComponentTick.TickInterval = 0.167f / 3.0f; // 30 Hz

	GizmoPlacement = EGizmoPlacement::OnLastSelection;
	CurrentTransformDomain = ETransformationDomain::None;
	CurrentTransformSpace = ETransformSpace::World;
	TranslationGizmoClass = ATranslationGizmo::StaticClass();
	RotationGizmoClass = ARotationGizmo::StaticClass();
	ScaleGizmoClass = AScaleGizmo::StaticClass();

	CloneReplicationCheckFrequency = 0.05f;
	MinimumCloneReplicationTime = 0.01f;

	bResyncSelection = false;
	SetIsReplicatedByDefault(false);
	bIgnoreNonReplicatedObjects = true;

	ResetDeltaTransform(AccumulatedDeltaTransform);
	ResetDeltaTransform(NetworkDeltaTransform);

	bMultiselectRotateOnLocalAxis = false;
	bToggleSelectedInMultiSelection = true;
}

void UEditorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UEditorComponent::BeginPlay() {
	Super::BeginPlay();

	if (!IsRunningDedicatedServer()) {
		GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
		if (EditorSubsystem) {
			EditorSubsystem->SetEditorComponent(this);
		}
	}
}

void UEditorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	DeselectAll();

	if (!IsRunningDedicatedServer()) {
		GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
		if (EditorSubsystem) {
			EditorSubsystem->ResetEditorComponent();
		}
	}

	Super::EndPlay(EndPlayReason);
}

UPlaceableComponent* UEditorComponent::GetPlaceableComponent(AActor* Actor) const {
	if (!IsValid(Actor)) {
		return nullptr;
	}

	return Actor->FindComponentByClass<UPlaceableComponent>();
}

void UEditorComponent::Server_PlaceActor_Implementation(const FVePlaceableClassMetadata& ClassMetadata, const FVector& WorldLocation, const FRotator& WorldRotation) { }

AActor* UEditorComponent::GetSelectedActor() const {
	if (SelectedActors.Num()) {
		return SelectedActors.Last();
	}
	return nullptr;
}

void UEditorComponent::SelectActor(AActor* Actor) {}

void UEditorComponent::FilterHits(TArray<FHitResult>& OutHits) const {
	// Filter hits on non-replicated objects.
	if (bIgnoreNonReplicatedObjects) {
		for (auto Iter = OutHits.CreateIterator(); Iter; ++Iter) {
			// Do not exclude non-replicated gizmos.
			if (Cast<AGizmoBase>(Iter->GetActor())) {
				continue;
			}

			if (IsValid(Iter->GetActor()) && Iter->GetActor()->IsSupportedForNetworking()) {
				continue;
			}

			if (IsValid(Iter->GetActor()) && Iter->Component.IsValid()) {
				VeLogWarningFunc("Removing (Actor: %s ComponentHit: %s) from hits because it is not supported for networking.", *Iter->GetActor()->GetName(), *Iter->Component->GetName());
			}

			Iter.RemoveCurrent();
		}
	}
}

bool UEditorComponent::ShouldSelect(const AActor* Actor) {
	const bool bValid = IsValid(Actor);
	const bool bHasComponent = IsValid(Actor->FindComponentByClass<UPlaceableComponent>());
	return bValid && bHasComponent;
}

void UEditorComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Gizmo.IsValid()) return;

	if (const APlayerController* PlayerController = GetPlayerController()) {
		FVector WorldLocation, WorldDirection;
		if (PlayerController->IsLocalController() && PlayerController->PlayerCameraManager) {
			if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection)) {
				const FTransform DeltaTransform = UpdateTransform(PlayerController->PlayerCameraManager->GetActorForwardVector(), WorldLocation, WorldDirection);

				NetworkDeltaTransform = FTransform(
					DeltaTransform.GetRotation() * NetworkDeltaTransform.GetRotation(),
					DeltaTransform.GetLocation() + NetworkDeltaTransform.GetLocation(),
					DeltaTransform.GetScale3D() + NetworkDeltaTransform.GetScale3D());
			}
		}
	}

	// Only consider local view
	if (const APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(this, 0)) {
		if (LocalPlayerController->PlayerCameraManager) {
			Gizmo->ScaleGizmo(LocalPlayerController->PlayerCameraManager->GetCameraLocation(), LocalPlayerController->PlayerCameraManager->GetActorForwardVector(), LocalPlayerController->PlayerCameraManager->GetFOVAngle());
		}
	}

	Gizmo->UpdateGizmoSpace(GetTransformSpace()); //ToDo: change when this is called to improve performance when a gizmo is there without doing anything
}

FTransform UEditorComponent::UpdateTransform(const FVector& LookingVector, const FVector& RayOrigin, const FVector& RayDirection) {
	FTransform DeltaTransform;
	DeltaTransform.SetScale3D(FVector::ZeroVector);

	// If there is no gizmo or no current transformation applied, return empty transform.
	if (!Gizmo.IsValid() || CurrentTransformDomain == ETransformationDomain::None) {
		return DeltaTransform;
	}

	const FVector RayEnd = RayOrigin + 100'000'000 * RayDirection;

	const FTransform CalcDeltaTransform = Gizmo->GetDeltaTransform(LookingVector, RayOrigin, RayEnd, CurrentTransformDomain);

	//The delta transform we are actually going to apply (will remain same if there is no snapping taking place)
	DeltaTransform = CalcDeltaTransform;

	float SnappingValue = 0.0f;

	switch (CurrentTransformTool) {
	case ETransformType::Translation:
		if (IsTranslationSnappingEnabled()) {
			SnappingValue = GetTranslationSnappingValue();
		}
		break;
	case ETransformType::Rotation:
		if (IsRotationSnappingEnabled()) {
			SnappingValue = GetRotationSnappingValue();
		}
		break;
	case ETransformType::Scale:
		if (IsScaleSnappingEnabled()) {
			SnappingValue = GetScaleSnappingValue();
		}
		break;
	default:
		break;
	}

	DeltaTransform = Gizmo->GetSnappedTransform(AccumulatedDeltaTransform, CalcDeltaTransform, CurrentTransformDomain, SnappingValue);

	ApplyDeltaTransform(DeltaTransform);
	return DeltaTransform;
}

void UEditorComponent::ApplyDeltaTransform(const FTransform& DeltaTransform) {
	bool bSnappingEnabled = false;
	float SnappingValue = 0.0f;

	switch (CurrentTransformTool) {
	case ETransformType::Translation:
		bSnappingEnabled = IsTranslationSnappingEnabled();
		SnappingValue = GetTranslationSnappingValue();
		break;
	case ETransformType::Rotation:
		bSnappingEnabled = IsRotationSnappingEnabled();
		SnappingValue = GetRotationSnappingValue();
		break;
	case ETransformType::Scale:
		bSnappingEnabled = IsScaleSnappingEnabled();
		SnappingValue = GetScaleSnappingValue();
		break;
	default:
		break;
	}


	for (auto* Actor : SelectedActors) {
		if (!IsValid(Actor)) {
			continue;
		}

		if (auto* PlaceableComponent = GetPlaceableComponent(Actor); IsValid(PlaceableComponent)) {
			const FTransform& ComponentTransform = Actor->GetRootComponent()->GetComponentTransform();

			FQuat DeltaRotation = DeltaTransform.GetRotation();
			FVector DeltaLocation = ComponentTransform.GetLocation() - Gizmo->GetActorLocation();
			//DeltaScale is Unrotated Scale to Get Local Scale since World Scale is not supported
			FVector DeltaScale = ComponentTransform.GetRotation().UnrotateVector(DeltaTransform.GetScale3D());

			// Rotate around multi selection center.
			if (!bMultiselectRotateOnLocalAxis) {
				DeltaLocation = DeltaRotation.RotateVector(DeltaLocation);
			}

			FTransform NewTransform(
				DeltaRotation * ComponentTransform.GetRotation(),
				//adding Gizmo Location + prevDeltaLocation 
				// (i.e. location from Gizmo to Object after optional Rotating)
				// + deltaTransform Location Offset
				DeltaLocation + Gizmo->GetActorLocation() + DeltaTransform.GetLocation(),
				DeltaScale + ComponentTransform.GetScale3D());


			/* SNAPPING LOGIC PER COMPONENT */
			if (bSnappingEnabled && SnappingValue) {
				NewTransform = Gizmo->GetSnappedTransformPerComponent(ComponentTransform, NewTransform, CurrentTransformDomain, SnappingValue);
			}

			// Actor->GetRootComponent()->SetMobility(EComponentMobility::Type::Movable);
			// SetTransform(sc, newTransform);
			ApplyTransform(NewTransform);
		}
	}
}

void UEditorComponent::ApplyTransform(const FTransform& InTransform) {
	// if (!DeltaTransform.IsValid()) {
	// 	return;
	// }

	auto* SelectedActor = GetSelectedActor();
	if (IsValid(SelectedActor)) {
		SelectedActor->GetRootComponent()->SetMobility(EComponentMobility::Type::Movable);
		SelectedActor->SetActorTransform(InTransform);

		OnSelectionTransformChanged.Broadcast(InTransform);
	}
}

bool UEditorComponent::ProcessTracedObjects(const TArray<FHitResult>& HitResults, const bool bMultiselect) {
	// Reset current transformation domain for case when we don't hit any gizmos.
	ClearDomain();

	// Try to find a hit gizmo before selecting anything.
	if (Gizmo.IsValid()) {
		for (auto& HitResult : HitResults) {
			if (Gizmo == HitResult.GetActor()) {
				// Get transformation axes from the gizmo.
				if (USceneComponent* ComponentHit = Cast<USceneComponent>(HitResult.Component)) {
					SetTransformationDomain(Gizmo->GetTransformationAxes(ComponentHit));

					// If gizmo has domain, use gizmo and set the current transformation domain.
					if (CurrentTransformDomain != ETransformationDomain::None) {
						Gizmo->SetTransformProgressState(true, CurrentTransformDomain);
						return true;
					}
				}
			}
		}
	}

	// Try to find a placeable actor.
	for (auto& HitResult : HitResults) {
		// Do not include any other gizmo.
		if (Cast<AGizmoBase>(HitResult.GetActor())) {
			continue;
		}

		auto* Actor = HitResult.GetActor();

		// Debug
		// DebugHitHelper(HitResult);

		// Select actor if it is a valid placeable or continue if not.
		if (ShouldSelect(Actor)) {
			SelectActor(Actor, bMultiselect);
			return true;
		}
	}

	return false;
}

void UEditorComponent::SetRotateOnLocalAxis(const bool bRotateLocalAxis) {
	bMultiselectRotateOnLocalAxis = bRotateLocalAxis;
}

void UEditorComponent::SelectActor(AActor* Actor, const bool bMultiselect) {
	if (!IsValid(Actor)) {
		return;
	}

	// Deselect all previously selected actors if not using multiselect.
	if (!bMultiselect) {
		DeselectAll();
	}

	// Add actor to selection, or deselect from multiple selection if required.
	HandleActorSelected(Actor);

	UpdateGizmoPlacement();
}

void UEditorComponent::SelectMultipleActors(const TArray<AActor*>& Actors, bool bMultiselect) {
	bool bAnyActorValid = false;

	for (auto* Actor : Actors) {
		if (!ShouldSelect(Actor)) {
			continue;
		}

		if (!bMultiselect) {
			DeselectAll();

			// Change the multiselect flag that we can actually select multiple actors.
			bMultiselect = true;
		}

		// Set to valid if we passed filtration at least for one actor.
		bAnyActorValid = true;

		HandleActorSelected(Actor);
	}

	if (bAnyActorValid) {
		UpdateGizmoPlacement();
	}
}

void UEditorComponent::DeselectActor(AActor* Actor) {
	if (!IsValid(Actor)) {
		return;
	}

	HandleActorDeselected(Actor);

	UpdateGizmoPlacement();
}

void UEditorComponent::DeselectAll() {
	if (SelectedActors.IsEmpty()) {
		return;
	}

	for (auto* Actor : SelectedActors) {
		if (!IsValid(Actor)) {
			continue;
		}

		if (auto* Component = GetPlaceableComponent(Actor)) {
			Component->OnDeselected();
		}
	}

	SelectedActors.Empty();

	OnSelectedActorsChanged.Broadcast(SelectedActors);

	UpdateGizmoPlacement();
}

void UEditorComponent::DestroySelected() {
	for (auto* SelectedActor : SelectedActors) {
		Owner_DestroyPlaceableActor(SelectedActor);
	}

	SelectedActors.Empty();
	OnSelectedActorsChanged.Broadcast(SelectedActors);
	UpdateGizmoPlacement();
}

void UEditorComponent::Owner_DestroyPlaceableActor(AActor* Actor) {
	if (!IsValid(Actor)) {
		return;
	}

	GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api, Placeable);
	if (!PlaceableSubsystem) {
		return;
	}

	auto* SelectedPlaceableComponent = Actor->FindComponentByClass<UPlaceableComponent>();
	if (!IsValid(SelectedPlaceableComponent)) {
		return;
	}

	const auto Callback = MakeShared<FOnGenericRequestCompleted>();
	Callback->BindLambda([=](const bool bInSuccessful, const FString& Error) {
		if (!bInSuccessful) {
			VeLogErrorFunc("failed to delete a placeable actor %s", *Error);
			return;
		}

		RemoteServer_UpdatePlaceables({Actor});
	});

	const auto& PlaceableId = SelectedPlaceableComponent->GetMetadata().Id;
	PlaceableSubsystem->DeletePlaceable(PlaceableId, Callback);
}

void UEditorComponent::Owner_UpdatePlaceables(const TArray<AActor*>& Actors) {
	RemoteServer_UpdatePlaceables(Actors);
}

void UEditorComponent::RemoteServer_UpdatePlaceables_Implementation(const TArray<AActor*>& Actors) {
	LocalServer_UpdatePlaceables(Actors);
}

void UEditorComponent::LocalServer_UpdatePlaceables(const TArray<AActor*>& Actors) {
	GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api2, Placeable);
	if (!PlaceableSubsystem) {
		return;
	}

	for (auto* PlaceableActor : Actors) {
		if (!IsValid(PlaceableActor)) {
			continue;
		}

		auto* PlaceableComponent = UPlaceableLib::GetPlaceableComponent(PlaceableActor);
		if (!IsValid(PlaceableComponent)) {
			continue;
		}

		const auto Callback = MakeShared<FOnPlaceableRequestCompleted2>();
		Callback->BindLambda([=](const FApiPlaceableMetadata& ApiMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			if (InResponseCode == EApi2ResponseCode::NotFound) {
				if (!PlaceableActor->Destroy()) {
					VeLogError("failed to destroy placeable actor: %s", *PlaceableActor->GetName());
				}
				return;
			}

			if (InResponseCode != EApi2ResponseCode::Ok) {
				VeLogError("failed to update placeable actor: %s", *InError);
				return;
			}

			if (PlaceableComponent) {
				PlaceableComponent->LocalServer_OnUpdated(FVePlaceableMetadata(ApiMetadata));
			}
		});

		PlaceableSubsystem->Get(PlaceableComponent->GetMetadata().Id, Callback);
	}
}

void UEditorComponent::HandleActorSelected(AActor* Actor) {
	if (!IsValid(Actor)) {
		return;
	}

	if (IsActorSelected(Actor)) {
		// Remove actor from selected set in multiselect mode
		if (bToggleSelectedInMultiSelection) {
			SelectedActors.Remove(Actor);
			if (auto* Component = GetPlaceableComponent(Actor); IsValid(Component)) {
				Component->OnDeselected();
			}
			OnSelectedActorsChanged.Broadcast(SelectedActors);
		}
	} else {
		if (auto* Component = GetPlaceableComponent(Actor); IsValid(Component)) {
			SelectedActors.Add(Actor);
			Component->OnSelected();
			OnSelectedActorsChanged.Broadcast(SelectedActors);
		}
	}
}

void UEditorComponent::HandleActorDeselected(AActor* Actor) {
	DeselectActor(Actor);
}

void UEditorComponent::UpdateGizmoActor() {
	if (SelectedActors.Num() > 0) {
		// We have some actors selected, so check if we need to create or replace a gizmo.
		bool bCreateGizmo = true;
		if (Gizmo.IsValid()) {
			if (GetTransformTool() == Gizmo->GetGizmoType()) {
				bCreateGizmo = false;
			} else {
				Gizmo->Destroy();
				Gizmo.Reset();
			}
		}

		if (bCreateGizmo) {
			if (auto* World = GetWorld(); IsValid(World)) {
				if (auto* GizmoClass = GetGizmoClass(); IsValid(GizmoClass)) {
					Gizmo = World->SpawnActor<AGizmoBase>(GizmoClass);
					Gizmo->GetOnGizmoStateChanged().AddWeakLambda(this, [=](const ETransformType GizmoType, const bool bTransformInProgress, const ETransformationDomain CurrentDomain) {
						OnGizmoStateChanged.Broadcast(GizmoType, bTransformInProgress, CurrentDomain);
					});
				}
			}
		}
	} else {
		// There are no selected placeable actors, so destroy the existing gizmo.
		if (Gizmo.IsValid()) {
			Gizmo->Destroy();
			Gizmo.Reset();
		}
	}
}

void UEditorComponent::UpdateGizmoPlacement() {
	UpdateGizmoActor();

	if (!Gizmo.IsValid()) {
		return;
	}

	if (SelectedActors.Num() <= 0) {
		VeLogWarningFunc("no selected placeable actors");
		return;
	}

	AActor* GizmoAttachmentRoot = nullptr;

	switch (GizmoPlacement) {
	case EGizmoPlacement::OnFirstSelection:
		GizmoAttachmentRoot = SelectedActors[0];
		break;
	case EGizmoPlacement::OnLastSelection:
		GizmoAttachmentRoot = SelectedActors.Last();
		break;
	default:
		break;
	}

	if (GizmoAttachmentRoot) {
		Gizmo->AttachToActor(GizmoAttachmentRoot, FAttachmentTransformRules::SnapToTargetIncludingScale);
	} else {
		VeLogErrorFunc("failed to find a gizmo valid attachment root");
		return;
	}

	Gizmo->UpdateGizmoSpace(GetTransformSpace());
}

UClass* UEditorComponent::GetGizmoClass() const {
	switch (GetTransformTool()) {
	case ETransformType::Translation:
		return TranslationGizmoClass;
	case ETransformType::Rotation:
		return RotationGizmoClass;
	case ETransformType::Scale:
		return ScaleGizmoClass;
	default:
		return nullptr;
	}
}

ETransformationDomain UEditorComponent::GetCurrentTransformDomain() const {
	return CurrentTransformDomain;
}

void UEditorComponent::ClearDomain() {
	// Clear the accumulated transform when we stop transforming
	ResetDeltaTransform(AccumulatedDeltaTransform);
	SetTransformationDomain(ETransformationDomain::None);
}

bool UEditorComponent::Owner_GetMouseTraceWorldRay(FVector& OutRayStart, FVector& OutRayEnd) const {
	if (const auto* PlayerController = GetPlayerController()) {
		FVector WorldLocation, WorldDirection;
		if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection)) {
			OutRayStart = WorldLocation;
			OutRayEnd = WorldLocation + WorldDirection * TraceDistance;
			return true;
		}
	}
	return false;
}

bool UEditorComponent::Owner_MouseTraceByObjectTypes(const TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels, const TArray<AActor*> IgnoredActors, const bool bMultiselect) {
	if (!HasPlayer()) {
		return false;
	}

	FVector WorldRayStart, WorldRayEnd;
	if (Owner_GetMouseTraceWorldRay(WorldRayStart, WorldRayEnd)) {
		// Trace objects and handle any valid hit.
		const bool bTraceSuccessful = TraceByObjectTypes(WorldRayStart, WorldRayEnd, CollisionChannels, IgnoredActors, bMultiselect);

		if (IsLocallyControlled()) {
			if (GetNetMode() == NM_Standalone) {
				// Standalone mode, just deselect everything if didn't hit anything.
				if (!bTraceSuccessful && !bMultiselect) {
					DeselectAll();
				}
			} else if (HasAuthority()) {
				// Listen server, replicate server trace results to connected clients.
				LocalServer_ReplicateTraceResults(bTraceSuccessful, bMultiselect);
			} else {
				if (!bTraceSuccessful && !bMultiselect) {
					// Trace didn't hit anything, so we request server to deselect all. 
					RemoteServer_DeselectAll();
				} else {
					if (CurrentTransformDomain == ETransformationDomain::None) {
						// If no transformation domain set, then we have no gizmo hit and should go a server side trace for objects. 
						RemoteServer_TraceByObjectTypes(WorldRayStart, WorldRayEnd, CollisionChannels, bMultiselect);
					} else {
						// Transformation domain set if the user clicked the gizmo, so just change the current domain on the server.
						RemoteServer_SetDomain(CurrentTransformDomain);
					}
				}
			}
		} else {
			VeLogWarningFunc("failed to initiate trace on non-locally controlled pawn");
		}
	}

	return false;
}

bool UEditorComponent::Owner_MouseTraceByChannel(const TEnumAsByte<ECollisionChannel> CollisionTraceChannel, const TArray<AActor*> IgnoredActors, const bool bMultiselect) {
	if (!HasPlayer()) {
		return false;
	}

	FVector WorldRayStart, WorldRayEnd;
	if (Owner_GetMouseTraceWorldRay(WorldRayStart, WorldRayEnd)) {
		// Trace objects and handle any valid hit.
		const bool bTraceSuccessful = TraceByChannel(WorldRayStart, WorldRayEnd, CollisionTraceChannel, IgnoredActors, bMultiselect);

		if (IsLocallyControlled()) {
			if (GetNetMode() == NM_Standalone) {
				// Standalone mode, just deselect everything if didn't hit anything.
				if (!bTraceSuccessful && !bMultiselect) {
					DeselectAll();
				}
			} else if (HasAuthority()) {
				// Listen server, replicate server trace results to connected clients.
				LocalServer_ReplicateTraceResults(bTraceSuccessful, bMultiselect);
			} else {
				if (!bTraceSuccessful && !bMultiselect) {
					// Trace didn't hit anything, so we request server to deselect all. 
					RemoteServer_DeselectAll();
				} else {
					if (CurrentTransformDomain == ETransformationDomain::None) {
						// If no transformation domain set, then we have no gizmo hit and should go a server side trace for objects. 
						RemoteServer_TraceByChannel(WorldRayStart, WorldRayEnd, CollisionTraceChannel, bMultiselect);
					} else {
						// Transformation domain set, so the user clicked the gizmo, so just change domain on the server.
						RemoteServer_SetDomain(CurrentTransformDomain);
					}
				}
			}
		} else {
			VeLogWarningFunc("failed to initiate trace on non-locally controlled pawn");
		}
	}

	return false;
}

bool UEditorComponent::Owner_MouseTraceByProfile(const FName& CollisionProfileName, const TArray<AActor*> IgnoredActors, const bool bMultiselect) {
	if (!HasPlayer()) {
		return false;
	}

	FVector WorldRayStart, WorldRayEnd;
	if (Owner_GetMouseTraceWorldRay(WorldRayStart, WorldRayEnd)) {
		// Trace objects and handle any valid hit.
		const bool bTraceSuccessful = TraceByProfile(WorldRayStart, WorldRayEnd, CollisionProfileName, IgnoredActors, bMultiselect);

		if (IsLocallyControlled()) {
			if (GetNetMode() == NM_Standalone) {
				// Standalone mode, just deselect everything if didn't hit anything.
				if (!bTraceSuccessful && !bMultiselect) {
					DeselectAll();
				}
			} else if (HasAuthority()) {
				// Listen server, replicate server trace results to connected clients.
				LocalServer_ReplicateTraceResults(bTraceSuccessful, bMultiselect);
			} else {
				if (!bTraceSuccessful && !bMultiselect) {
					// Trace didn't hit anything, so we request server to deselect all. 
					RemoteServer_DeselectAll();
				} else {
					if (CurrentTransformDomain == ETransformationDomain::None) {
						// If no transformation domain set, then we have no gizmo hit and should go a server side trace for objects. 
						RemoteServer_TraceByProfile(WorldRayStart, WorldRayEnd, CollisionProfileName, bMultiselect);
					} else {
						// Transformation domain set, so the user clicked the gizmo, so just change domain on the server.
						RemoteServer_SetDomain(CurrentTransformDomain);
					}
				}
			}
		} else {
			VeLogWarningFunc("failed to initiate trace on non-locally controlled pawn");
		}
	}

	return false;
}

bool UEditorComponent::TraceByObjectTypes(const FVector& RayStartLocation, const FVector& RayEndLocation, TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels, TArray<AActor*> IgnoredActors, bool bAddToPreviouslySelectedComponents) {
	if (const auto* World = GetWorld(); IsValid(World)) {
		FCollisionObjectQueryParams CollisionObjectQueryParams;
		FCollisionQueryParams CollisionQueryParams;

		for (auto& CollisionChannel : CollisionChannels) {
			CollisionObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);
		}

		CollisionQueryParams.AddIgnoredActors(IgnoredActors);

		TArray<FHitResult> OutHits;
		if (World->LineTraceMultiByObjectType(OutHits, RayStartLocation, RayEndLocation, CollisionObjectQueryParams, CollisionQueryParams)) {
			FilterHits(OutHits);

			return ProcessTracedObjects(OutHits, bAddToPreviouslySelectedComponents);
		}
	}
	return false;
}

bool UEditorComponent::TraceByChannel(const FVector& RayStartLocation, const FVector& RayEndLocation, TEnumAsByte<ECollisionChannel> CollisionTraceChannel, TArray<AActor*> IgnoredActors, bool bAddToPreviouslySelectedComponents) {
	if (const auto* World = GetWorld()) {
		FCollisionQueryParams CollisionQueryParams;

		CollisionQueryParams.AddIgnoredActors(IgnoredActors);

		TArray<FHitResult> OutHits;
		if (World->LineTraceMultiByChannel(OutHits, RayStartLocation, RayEndLocation, CollisionTraceChannel, CollisionQueryParams)) {
			FilterHits(OutHits);

			return ProcessTracedObjects(OutHits, bAddToPreviouslySelectedComponents);
		}
	}
	return false;
}

bool UEditorComponent::TraceByProfile(const FVector& RayStartLocation, const FVector& RayEndLocation, const FName& ProfileName, TArray<AActor*> IgnoredActors, bool bAddToPreviouslySelectedComponents) {
	if (UWorld* world = GetWorld()) {
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActors(IgnoredActors);

		TArray<FHitResult> OutHits;
		if (world->LineTraceMultiByProfile(OutHits, RayStartLocation, RayEndLocation, ProfileName, CollisionQueryParams)) {
			FilterHits(OutHits);
			return ProcessTracedObjects(OutHits, bAddToPreviouslySelectedComponents);
		}
	}
	return false;
}

void UEditorComponent::ResetDeltaTransform(FTransform& Transform) {
	Transform = FTransform();
	Transform.SetScale3D(FVector::ZeroVector);
}

void UEditorComponent::SetTransformationDomain(ETransformationDomain Domain) {
	if (Gizmo.IsValid()) {
		Gizmo->SetTransformProgressState(Domain != ETransformationDomain::None, Domain);
	}

	CurrentTransformDomain = Domain;
}

TArray<AActor*> UEditorComponent::GetIgnoredActorsForServerTrace() const {
	TArray<AActor*> ignoredActors;
	//Ignore Gizmo in Server Trace Test if it's not Server controlling Pawn (since Gizmo is relative)
	if (!IsLocallyControlled()) {
		if (Gizmo.IsValid()) ignoredActors.Add(Gizmo.Get());
	}
	return ignoredActors;
}

void UEditorComponent::LocalServer_ReplicateTraceResults(bool bTraceSuccessful, bool bMultiselect) {
	//Only perform this on Clients
	if (!HasAuthority()) {
		if (!bTraceSuccessful && !bMultiselect) DeselectAll();
		RemoteMulticast_SetDomain(CurrentTransformDomain);
		// MulticastSetSelectedComponents(SelectedComponents);
	}
}

void UEditorComponent::LocalServer_ApplyTransform(const FTransform& DeltaTransform) {
	RemoteMulticast_ApplyTransform(DeltaTransform);
}

void UEditorComponent::Owner_FinishTransform() {
	const auto* Pawn = GetPawn();
	if (!IsValid(Pawn)) {
		return;
	}

#if 1

	ClearDomain();
	Owner_OnTransformComplete();

#else
	if (Pawn->IsLocallyControlled()) {
		if (Pawn->GetNetMode() == NM_Standalone) {
			ClearDomain();
		} else if (Pawn->HasAuthority()) {
			LocalServer_ClearDomain();
			LocalServer_ApplyTransform(NetworkDeltaTransform);
		} else {
			RemoteServer_ClearDomain();
			RemoteServer_ApplyTransform(NetworkDeltaTransform);
			ResetDeltaTransform(NetworkDeltaTransform);
		}
	}
#endif
}

void UEditorComponent::LocalServer_ClearDomain() {
	RemoteMulticast_ClearDomain();
}

void UEditorComponent::RemoteServer_DestroySelection_Implementation() {
	RemoteMulticast_DestroySelection();
}

void UEditorComponent::CheckUnreplicatedActors() {
	// int32 RemoveCount = 0;
	// const float TimeElapsed = GetWorldTimerManager().GetTimerElapsed(CheckUnrepTimerHandle);
	// for (auto& c : UnreplicatedComponentClones) {
	// 	// rather than calling "IsSupportedForNetworking" (which returns true all the time)
	// 	// call HasActorBegunPlay (which means we are sure the BeginPlay for AActor has finished completely.
	// 	// and so we can safely send this reference over the network.
	// 	if (c && c->HasBegunPlay() && c->IsSupportedForNetworking() &&
	// 		TimeElapsed > MinimumCloneReplicationTime) // Make sure there's a minimum time...
	// 		++RemoveCount;
	// }
	//
	// // Remove with performance (Swapping). We don't care about the order in this case
	// // since the role of this is to check that all unreplicated components have been replicated.
	// UnreplicatedComponentClones.RemoveAtSwap(0, RemoveCount);
	//
	// // check if all clones have been replicated
	// if (UnreplicatedComponentClones.Num() == 0) {
	// 	//stop calling this if no more unreplicated actors
	// 	GetWorldTimerManager().ClearTimer(CheckUnrepTimerHandle);
	//
	// 	UE_LOG(LogTemp, Log, TEXT("[SERVER] Time Elapsed for %d Replicated Actors to replicate: %f"), SelectedComponents.Num(), TimeElapsed);
	//
	// 	//send all the selected replicated actors!
	// 	MulticastSetSelectedComponents(SelectedComponents);
	//
	// }
}

void UEditorComponent::RemoteServer_SyncSelectedActors_Implementation() {
	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		RemoteMulticast_SetSelectedActors(SelectedActors);
	}
}

void UEditorComponent::RemoteMulticast_SetSelectedActors_Implementation(const TArray<AActor*>& Actors) {
	// todo
}

void UEditorComponent::ResyncSelection() {
	if (bResyncSelection) {
		UE_LOG(LogTemp, Warning, TEXT("Resyncing Selection"));
		RemoteServer_SyncSelectedActors();
		RemoteServer_SyncSelectedComponents();
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Resyncing FINISHED"));
		//stop calling this if no more resync is needed
		if (auto* WorldTimerManager = GetWorldTimerManager()) {
			WorldTimerManager->ClearTimer(ResyncSelectionTimerHandle);
		}
	}
}

void UEditorComponent::RemoteMulticast_SetSelectedComponents_Implementation(const TArray<USceneComponent*>& Components) {
	// if (GetLocalRole() < ROLE_Authority) {
	// 	UE_LOG(LogTemp, Log, TEXT("MulticastSelect ComponentCount: %d"), Components.Num());
	// }
	//
	//
	// DeselectAll(); //calling here because Selecting MultipleComponents empty is not going to call Deselect all
	// // SelectMultipleComponents(Components, true);
	//
	// //Tells whether we have Selected the exact number of components that came in 
	// // or there was a nullptr in Components and therefore there is a difference.
	// // if there is a difference we will need to resync
	// bResyncSelection = (Components.Num() != SelectedComponents.Num());
	// if (bResyncSelection) {
	// 	//Timer to loop until all Unreplicated Actors have finished replicating!
	// 	if (UWorld* world = GetWorld()) {
	// 		if (!ResyncSelectionTimerHandle.IsValid())
	// 			world->GetTimerManager().SetTimer(ResyncSelectionTimerHandle, this
	// 				, &UEditorTransformComponent::ResyncSelection
	// 				, 0.1f, true, 0.0f);
	// 	}
	// }
	//
	//
	// if (GetLocalRole() < ROLE_Authority) {
	// 	UE_LOG(LogTemp, Log, TEXT("Selected ComponentCount: %d"), SelectedComponents.Num());
	// }
}

void UEditorComponent::RemoteServer_SyncSelectedComponents_Implementation() {
	// MulticastSetSelectedComponents(SelectedComponents);
}

void UEditorComponent::RemoteMulticast_SetDomain_Implementation(ETransformationDomain Domain) {
	SetTransformationDomain(Domain);
}

void UEditorComponent::RemoteServer_SetDomain_Implementation(ETransformationDomain Domain) {
	RemoteMulticast_SetDomain(Domain);
}

void UEditorComponent::RemoteServer_CloneSelected_Implementation(bool bSelectNewClones, bool bAppendToList) {
	// if (bComponentBased) {
	// UE_LOG(LogTemp, Warning, TEXT("** Component Cloning is currently not supported in a Network Environment :( **"));
	// see PluginLimitations.txt for a reason why Component Cloning is not supported
	// return;
	// }
	//
	// auto ComponentListCopy = GetSelectedComponents();
	//
	// //just create 'em, not select 'em (we select 'em later)
	// auto CloneList = CloneFromList(ComponentListCopy);
	//
	// //if we have to select the new clones, multicast for these new objects
	// if (bSelectNewClones) {
	// 	// SelectMultipleComponents(CloneList, bAppendToList);
	// 	UnreplicatedComponentClones = CloneList;
	//
	// 	//Timer to loop until all Unreplicated Actors have finished replicating!
	// 	if (UWorld* world = GetWorld()) {
	// 		if (!CheckUnrepTimerHandle.IsValid())
	// 			world->GetTimerManager().SetTimer(CheckUnrepTimerHandle, this
	// 				, &UEditorTransformComponent::CheckUnreplicatedActors
	// 				, CloneReplicationCheckFrequency, true, 0.0f);
	// 	}
	// }
}

void UEditorComponent::RemoteMulticast_SetRotateOnLocalAxis_Implementation(bool bRotateLocalAxis) {
	SetRotateOnLocalAxis(bRotateLocalAxis);
}

void UEditorComponent::RemoteServer_SetRotateOnLocalAxis_Implementation(bool bRotateLocalAxis) {
	RemoteMulticast_SetRotateOnLocalAxis(bRotateLocalAxis);
}


void UEditorComponent::RemoteMulticast_DeselectAll_Implementation() {
	DeselectAll();
}

void UEditorComponent::RemoteMulticast_DestroySelection_Implementation() {
	DestroySelected();
}

void UEditorComponent::RemoteServer_DeselectAll_Implementation() {
	RemoteMulticast_DeselectAll();
}

void UEditorComponent::RemoteMulticast_ApplyTransform_Implementation(const FTransform& DeltaTransform) {
	if (const auto* Controller = GetPlayerController(); IsValid(Controller) && !Controller->IsLocalController()) {
		ApplyDeltaTransform(DeltaTransform);
	}
}

void UEditorComponent::RemoteServer_ApplyTransform_Implementation(const FTransform& DeltaTransform) {
	LocalServer_ApplyTransform(DeltaTransform);
}

void UEditorComponent::RemoteMulticast_ClearDomain_Implementation() {
	ClearDomain();
}

void UEditorComponent::RemoteServer_ClearDomain_Implementation() {
	LocalServer_ClearDomain();
}

void UEditorComponent::RemoteServer_TraceByObjectTypes_Implementation(const FVector& WorldRayStart, const FVector& WorldRayEnd, const TArray<TEnumAsByte<ECollisionChannel>>& CollisionChannels, const bool bMultiselect) {
	// Note that this will change the current transformation domain.
	const bool bTraceSuccessful = TraceByObjectTypes(WorldRayStart, WorldRayEnd, CollisionChannels, GetIgnoredActorsForServerTrace(), bMultiselect);

	if (!bTraceSuccessful && !bMultiselect) {
		// Deselect everything if not hit any selectable object and not multi-selecting.
		DeselectAll();
	}

	// Notify clients that transformation domain has changed.
	RemoteMulticast_SetDomain(CurrentTransformDomain);

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		RemoteMulticast_SetSelectedActors(SelectedActors);
	}
}

void UEditorComponent::RemoteServer_TraceByChannel_Implementation(const FVector& WorldRayStart, const FVector& WorldRayEnd, const ECollisionChannel TraceChannel, const bool bMultiselect) {
	// Note that this will change the current transformation domain.
	const bool bTraceSuccessful = TraceByChannel(WorldRayStart, WorldRayEnd, TraceChannel, GetIgnoredActorsForServerTrace(), bMultiselect);

	if (!bTraceSuccessful && !bMultiselect) {
		// Deselect everything if not hit any selectable object and not multi-selecting.
		DeselectAll();
	}

	// Notify clients that transformation domain has changed.
	RemoteMulticast_SetDomain(CurrentTransformDomain);

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		RemoteMulticast_SetSelectedActors(SelectedActors);
	}
}

void UEditorComponent::RemoteServer_TraceByProfile_Implementation(const FVector& WorldRayStart, const FVector& WorldRayEnd, const FName& ProfileName, const bool bMultiselect) {
	// Note that this will change the current transformation domain.
	const bool bTraceSuccessful = TraceByProfile(WorldRayStart, WorldRayEnd, ProfileName, GetIgnoredActorsForServerTrace(), bMultiselect);

	if (!bTraceSuccessful && !bMultiselect) {
		// Deselect everything if not hit any selectable object and not multi-selecting. 
		DeselectAll();
	}

	// Notify clients that transformation domain has changed.
	RemoteMulticast_SetDomain(CurrentTransformDomain);

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		RemoteMulticast_SetSelectedActors(SelectedActors);
	}
}

#undef RTT_LOG

#pragma region deprecated
// void UEditorTransformComponent::MulticastSetTransformationType_Implementation(ETransformType Transformation) {
// 	SetTransformationType(Transformation);
// }
//
// void UEditorTransformComponent::ServerSetTransformationType_Implementation(ETransformType Transformation) {
// 	MulticastSetTransformationType(Transformation);
// }

// void UEditorTransformComponent::MulticastSetSpaceType_Implementation(ETransformSpace Space) {
// SetTransformSpace(Space);
// }

// void UEditorTransformComponent::ServerSetSpaceType_Implementation(ETransformSpace Space) {
// MulticastSetSpaceType(Space);
// }

// void UEditorTransformComponent::SetTransformationType(const ETransformType TransformationType) {
// 	//Don't continue if these are the same.
// 	if (CurrentTransformation == TransformationType) return;
//
// 	CurrentTransformation = TransformationType;
//
// 	//Clear the Accumulated tranform when we have a new Transformation
// 	ResetDeltaTransform(AccumulatedDeltaTransform);
//
// 	UpdateGizmoPlacement();
// }

// void UEditorTransformComponent::SetSnappingEnabled(const ETransformType TransformationType, const bool bSnappingEnabled) {
// 	SnappingEnabled.Add(TransformationType, bSnappingEnabled);
// }
//
// void UEditorTransformComponent::SetSnappingValue(const ETransformType TransformationType, const float SnappingValue) {
// 	SnappingValues.Add(TransformationType, SnappingValue);
// }

// void UEditorTransformComponent::GetSelectedComponents(TArray<USceneComponent*>& OutComponentList, USceneComponent*& OutGizmoPlacedComponent) const {
// 	OutComponentList = SelectedComponents;
// 	if (Gizmo.IsValid())
// 		OutGizmoPlacedComponent = Gizmo->GetParentComponent();
// }
//
// TArray<USceneComponent*> UEditorTransformComponent::GetSelectedComponents() const {
// 	return SelectedComponents;
// }
//
// void UEditorTransformComponent::CloneSelected(const bool bSelectNewClones, const bool bAppendToList) {
// 	if (GetLocalRole() < ROLE_Authority) {
// 		UE_LOG(LogTemp, Warning, TEXT("Cloning in a Non-Authority! Please use the Clone RPCs instead"));
// 	}
//
//
// 	auto CloneComponents = CloneFromList(SelectedComponents);
//
// 	// if (bSelectNewClones)
// 	// SelectMultipleComponents(CloneComponents, bAppendToList);
// }
//
// TArray<USceneComponent*> UEditorTransformComponent::CloneFromList(const TArray<USceneComponent*>& ComponentList) {
//
// 	TArray<USceneComponent*> OutClones;
// 	// if (bComponentBased) {
// 	// 	TArray<USceneComponent*> Components;
// 	//
// 	// 	for (auto& i : ComponentList) {
// 	// 		if (i) Components.Add(i);
// 	// 	}
// 	// 	OutClones = CloneComponents(Components);
// 	// } else {
// 	TArray<AActor*> Actors;
// 	for (auto& i : ComponentList) {
// 		if (i)
// 			Actors.Add(i->GetOwner());
// 	}
// 	OutClones = CloneActors(Actors);
// 	// }
//
// 	if (CurrentTransformAxes != ETransformationAxes::None && Gizmo.IsValid())
// 		Gizmo->SetTransformProgressState(true, CurrentTransformAxes);
//
// 	return OutClones;
// }
//
// TArray<USceneComponent*> UEditorTransformComponent::CloneActors(const TArray<AActor*>& Actors) {
// 	TArray<USceneComponent*> OutClones;
//
// 	UWorld* World = GetWorld();
// 	if (!World) return OutClones;
//
// 	TSet<AActor*> ActorsProcessed;
// 	TArray<AActor*> ActorsToSelect;
// 	for (auto* TemplateActor : Actors) {
// 		if (!TemplateActor) continue;
// 		bool bAlreadyProcessed;
// 		ActorsProcessed.Add(TemplateActor, &bAlreadyProcessed);
// 		if (bAlreadyProcessed) continue;
//
// 		FTransform SpawnTransform;
// 		FActorSpawnParameters SpawnParams;
//
// 		SpawnParams.Template = TemplateActor;
// 		if (TemplateActor)
// 			TemplateActor->bNetStartup = false;
//
// 		if (const AActor* Actor = World->SpawnActor(TemplateActor->GetClass(), &SpawnTransform, SpawnParams)) {
// 			OutClones.Add(Actor->GetRootComponent());
// 		}
// 	}
// 	return OutClones;
// }
//
// TArray<USceneComponent*> UEditorTransformComponent::CloneComponents(const TArray<USceneComponent*>& Components) {
// 	TArray<class USceneComponent*> outClones;
//
// 	UWorld* world = GetWorld();
// 	if (!world) return outClones;
//
// 	TMap<USceneComponent*, USceneComponent*> OcCc; //Original component - Clone component
// 	TMap<USceneComponent*, USceneComponent*> CcOp; //Clone component - Original parent
//
// 	//clone components phase
// 	for (auto& templateComponent : Components) {
// 		if (!templateComponent) continue;
// 		AActor* owner = templateComponent->GetOwner();
// 		if (!owner) continue;
//
// 		if (USceneComponent* clone = Cast<USceneComponent>(
// 			StaticDuplicateObject(templateComponent, owner))) {
// 			// PostCreateBlueprintComponent(clone);
// 			// todo: check if we need to check that component clone should be added to the owner's replicated components set
// 			clone->CreationMethod = EComponentCreationMethod::UserConstructionScript;
// 			clone->OnComponentCreated();
//
// 			clone->RegisterComponent();
// 			clone->SetRelativeTransform(templateComponent->GetRelativeTransform());
//
// 			outClones.Add(clone);
// 			//Add to these two maps for reparenting in next phase
// 			OcCc.Add(templateComponent, clone); //Original component - Clone component
//
// 			if (templateComponent == owner->GetRootComponent())
// 				CcOp.Add(clone, owner->GetRootComponent()); //this will cause a loop in the maps, so we must check for this!
// 			else
// 				CcOp.Add(clone, templateComponent->GetAttachParent()); //Clone component - Original parent
// 		}
// 	}
//
// 	TArray<USceneComponent*> componentsToSelect;
// 	//reparenting phase
// 	FAttachmentTransformRules attachmentRule(EAttachmentRule::KeepWorld, false);
// 	for (auto& cp : CcOp) {
// 		//original parent
// 		USceneComponent* parent = cp.Value;
//
// 		AActor* actorOwner = cp.Value->GetOwner();
//
// 		//find if we cloned the original parent
// 		USceneComponent** cloneParent = OcCc.Find(parent);
//
// 		if (cloneParent) {
// 			if (*cloneParent != cp.Key) //make sure comp is not its own parent
// 				parent = *cloneParent;
// 		} else {
// 			//couldn't find its parent, so find the parent of the parent and see if it's in the list.
// 			//repeat until found or root is reached
// 			while (1) {
// 				//if parent is root, then no need to find parents above it. (none)
// 				//attach to original parent, since there are no cloned parents.
// 				if (parent == actorOwner->GetRootComponent()) {
// 					parent = cp.Value;
// 					break;
// 				}
//
// 				//check if parents have been cloned
// 				cloneParent = OcCc.Find(parent->GetAttachParent());
// 				if (cloneParent) {
// 					//attach to cloned parent if found
// 					parent = *cloneParent;
// 					break;
// 				}
// 				parent = parent->GetAttachParent(); //move up in the hierarchy
// 			}
// 		}
//
// 		cp.Key->AttachToComponent(parent, attachmentRule);
//
// 		//Selecting childs and parents can cause weird issues 
// 		// so only select the topmost clones (those that do not have cloned parents!)
// 		//only select those that have an "original parent". 
// 		//if ((parent == cp.Value 
// 		//	|| parent == actorOwner->GetRootComponent())) 
// 		//	//check if the parent of the cloned is original (means it's topmost)
// 		//	outParents.Add(cp.Key);
// 	}
//
// 	return outClones;
// }

// void UEditorTransformComponent::SelectComponent(USceneComponent* Component, bool bAppendToList) {
// 	if (!Component) return;
//
// 	if (ShouldSelect(Component->GetOwner(), Component)) {
// 		if (false == bAppendToList)
// 			DeselectAll();
// 		AddComponent_Internal(SelectedComponents, Component);
// 		UpdateGizmoPlacement();
// 	}
// }
//
// void UEditorTransformComponent::ReplicatedMouseTraceByObjectTypes(float TraceDistance, TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels, bool bAppendToList) {
// 	FVector start, end;
// 	if (GetMouseTraceWorldRay(TraceDistance, start, end)) {
// 		bool bTraceSuccessful = TraceByObjectTypes(start, end, CollisionChannels, TArray<AActor*>(), bAppendToList);
//
// 		//Server
// 		if (GetLocalRole() == ROLE_Authority)
// 			LocalServer_ReplicateTraceResults(bTraceSuccessful, bAppendToList);
// 		//Client
// 		else {
// 			if (!bTraceSuccessful && !bAppendToList)
// 				RemoteServer_DeselectAll();
// 			else {
// 				// If a Local Trace was on a Gizmo, just tell the Server that we 
// 				// have hit our Gizmo and just change the Domain there.
// 				// Else, do the Server Trace
// 				if (CurrentTransformDomain == ETransformationDomain::None)
// 					RemoteServer_TraceByObjectTypes(start, end, CollisionChannels, bAppendToList);
// 				else
// 					RemoteServer_SetDomain(CurrentTransformDomain);
// 			}
//
// 		}
//
//
// 	}
// }
//
// void UEditorTransformComponent::ReplicatedMouseTraceByChannel(float TraceDistance, TEnumAsByte<ECollisionChannel> CollisionChannel, bool bAppendToList) {
// 	FVector start, end;
// 	if (GetMouseTraceWorldRay(TraceDistance, start, end)) {
// 		bool bTraceSuccessful = TraceByChannel(start, end
// 			, CollisionChannel
// 			, TArray<AActor*>(), bAppendToList);
//
// 		//Server
// 		if (GetLocalRole() == ROLE_Authority)
// 			LocalServer_ReplicateTraceResults(bTraceSuccessful, bAppendToList);
// 		//Client
// 		else {
// 			if (!bTraceSuccessful && !bAppendToList)
// 				RemoteServer_DeselectAll();
//
// 			// If a Local Trace was on a Gizmo, just tell the Server that we 
// 			// have hit our Gizmo and just change the Domain there.
// 			// Else, do the Server Trace
// 			if (CurrentTransformDomain == ETransformationDomain::None)
// 				RemoteServer_TraceByChannel(start, end, CollisionChannel, bAppendToList);
// 			else
// 				RemoteServer_SetDomain(CurrentTransformDomain);
// 		}
// 	}
// }
//
// void UEditorTransformComponent::ReplicatedMouseTraceByProfile(float TraceDistance, const FName& ProfileName, bool bAppendToList) {
// 	FVector start, end;
// 	if (GetMouseTraceWorldRay(TraceDistance, start, end)) {
// 		bool bTraceSuccessful = TraceByProfile(start, end
// 			, ProfileName
// 			, TArray<AActor*>(), bAppendToList);
//
// 		//Server
// 		if (GetLocalRole() == ROLE_Authority)
// 			LocalServer_ReplicateTraceResults(bTraceSuccessful, bAppendToList);
// 		//Client
// 		else {
// 			if (!bTraceSuccessful && !bAppendToList)
// 				RemoteServer_DeselectAll();
//
// 			// If a Local Trace was on a Gizmo, just tell the Server that we 
// 			// have hit our Gizmo and just change the Domain there.
// 			// Else, do the Server Trace
// 			if (CurrentTransformDomain == ETransformationDomain::None)
// 				RemoteServer_TraceByProfile(start, end, ProfileName, bAppendToList);
// 			else
// 				RemoteServer_SetDomain(CurrentTransformDomain);
// 		}
// 	}
// }

#pragma endregion

void UEditorComponent::SetTransformSpace(const ETransformSpace InTransformSpace) {

	ensureMsgf(true, TEXT("deprecated"));
	VeLogFunc("Set transform space");
	TransformSpace = InTransformSpace;

	{
		UpdateGizmoActor();

		OnTransformSpaceChanged.Broadcast(TransformSpace);
	}
}

void UEditorComponent::SetCloneObject() {
	// GET_MODULE_SUBSYSTEM(GameStateSubsystem, GameFramework, State);
	// if (!GameStateSubsystem) {
	// 	return;
	// }
	//
	// const auto* GameState = GameStateSubsystem->GetGameState();
	// if (!GameState) {
	// 	VeLogErrorFunc("failed to get game state");
	// 	return;
	// }
	//
	// // No Space (Error)
	// const auto SpaceId = GameState->GetSpaceId();
	// if (!SpaceId.IsValid()) {
	// 	VeLogWarningFunc("invalid space id, spawning temporary placeable actor");
	// 	FVePlaceableMetadata Metadata;
	// 	Metadata.PlaceableClassMetadata = FVePlaceableClassMetadata{ClassMetadata};
	// 	CreateTemporaryPlaceable(Metadata);
	// 	return;
	// }
	//
	// GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api, Placeable);
	// if (!PlaceableSubsystem) {
	// 	return;
	// }
	//
	// const auto Callback = MakeShared<FOnPlaceableRequestCompleted>();
	// Callback->BindLambda([=](const FApiPlaceableMetadata& InMetadata, const EApiResponseCode InResponseCode, const FString& InError) {
	// 	if (InResponseCode != EApiResponseCode::Ok) {
	// 		VeLogErrorFunc("failed to place an actor %s", *InError);
	// 		return;
	// 	}
	//
	// 	const FVePlaceableMetadata Metadata{InMetadata};
	//
	// 	if (GetNetMode() == NM_Standalone) {
	// 		LocalServer_CreatePlaceable(InMetadata.Id);
	// 		// LocalClient_CreatePlaceable(InMetadata.Id);
	// 	} else {
	// 		RemoteServer_CreatePlaceable(InMetadata.Id);
	// 	}
	// });
	//
	// FApiPlaceableMetadata PlaceableMetadata;
	// PlaceableMetadata.Position = Location;
	// PlaceableMetadata.Rotation = Rotation;
	// PlaceableMetadata.Scale = FVector::OneVector;
	// PlaceableMetadata.Type = TEXT("");
	// PlaceableMetadata.PlaceableClassId = ClassMetadata.Id;
	//
	// // Create placeable
	// PlaceableSubsystem->UpdatePlaceable(SpaceId, PlaceableMetadata, Callback);
}

void UEditorComponent::SetTransformTool(const ETransformType InTransformTool) {

	ensureMsgf(true, TEXT("deprecated"));
	VeLogFunc("Set transform tool");

	CurrentTransformTool = InTransformTool;

	{
		ResetDeltaTransform(AccumulatedDeltaTransform);
		UpdateGizmoPlacement();

		OnTransformToolChanged.Broadcast(CurrentTransformTool);
	}
}

void UEditorComponent::SetSnapping(const ETransformType Tool, const bool bEnable) {

	ensureMsgf(true, TEXT("deprecated"));
	VeLogFunc("Set snapping");

	switch (Tool) {
	case ETransformType::Translation:
		bSnapTranslation = bEnable;
		OnTransformSnapChanged.Broadcast(ETransformType::Translation, bSnapTranslation, TranslationSnapValue);
		break;
	case ETransformType::Rotation:
		bSnapRotation = bEnable;
		OnTransformSnapChanged.Broadcast(ETransformType::Rotation, bSnapRotation, RotationSnapValue);
		break;
	case ETransformType::Scale:
		bSnapScale = bEnable;
		OnTransformSnapChanged.Broadcast(ETransformType::Scale, bSnapScale, ScaleSnapValue);
		break;
	default:
		break;
	}
}

void UEditorComponent::SetCloning() {
	
}

void UEditorComponent::SetSnappingValue(const ETransformType Tool, const float Value) {

	ensureMsgf(true, TEXT("deprecated"));
	VeLogFunc("Set snapping value");

	switch (Tool) {
	case ETransformType::Translation:
		TranslationSnapValue = Value;
		OnTransformSnapChanged.Broadcast(ETransformType::Translation, bSnapTranslation, TranslationSnapValue);
		break;
	case ETransformType::Rotation:
		RotationSnapValue = Value;
		OnTransformSnapChanged.Broadcast(ETransformType::Rotation, bSnapRotation, RotationSnapValue);
		break;
	case ETransformType::Scale:
		ScaleSnapValue = Value;
		OnTransformSnapChanged.Broadcast(ETransformType::Scale, bSnapScale, ScaleSnapValue);
		break;
	default:
		break;
	}
}

#pragma region Create Placeable Object

void UEditorComponent::Owner_OnDrop(const FVePlaceableClassMetadata& ClassMetadata, const FVector2D& ScreenPosition) {
	auto* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController)) {
		return;
	}

	// Prepare world location and direction.
	FVector WorldLocation, WorldDirection;
	if (!PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection)) {
		UGameplayStatics::DeprojectScreenToWorld(PlayerController, ScreenPosition, WorldLocation, WorldDirection);
		VeLogWarningFunc("failed to get mouse world position");
	}

	// Rotate the new actor face to the player.
	FRotator WorldRotation = PlayerController->GetControlRotation().Add(0.f, 180.f, 0.f);

	// Trace forward by 100 meters, if no object found, spawn object in 5 meters in front of the editor pawn.

	if (auto* World = PlayerController->GetWorld(); IsValid(World)) {
		// Request api to check if player can actually create a placeable in the current space.
		if (FHitResult HitResult; World->LineTraceSingleByChannel(HitResult, WorldLocation, WorldLocation + WorldDirection * TraceDistance, ECC_Visibility)) {
			const FRotator Rotation = FRotationMatrix::MakeFromZ(HitResult.ImpactNormal).Rotator();
			Owner_RequestCreatePlaceable(ClassMetadata, HitResult.Location, Rotation);
		} else {
			FVector PlaceLocation = WorldLocation + WorldDirection * (TraceDistance / 2);
			Owner_RequestCreatePlaceable(ClassMetadata, PlaceLocation, WorldRotation);
		}
	}
}

void UEditorComponent::Owner_RequestCreatePlaceable(const FVePlaceableClassMetadata& ClassMetadata, const FVector& Location, const FRotator& Rotation) {
	FGuid WorldId;
	if (const auto* World = GetWorld()) {
		if (const auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			WorldId = VeWorldSettings->GetWorldId();
		}
	}

	// No WorldId (Error)
	if (!WorldId.IsValid()) {
		VeLogWarningFunc("Invalid world id, spawning temporary placeable actor");
		FVePlaceableMetadata Metadata;
		Metadata.PlaceableClassMetadata = FVePlaceableClassMetadata{ClassMetadata};
		CreateTemporaryPlaceable(Metadata);
		return;
	}

	GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api, Placeable);
	if (!PlaceableSubsystem) {
		return;
	}

	const auto Callback = MakeShared<FOnPlaceableRequestCompleted>();
	Callback->BindLambda([=](const FApiPlaceableMetadata& InMetadata, const EApiResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode != EApiResponseCode::Ok) {
			VeLogErrorFunc("failed to place an actor %s", *InError);
			return;
		}

		const FVePlaceableMetadata Metadata{InMetadata};

		if (GetNetMode() == NM_Standalone) {
			LocalServer_CreatePlaceable(InMetadata.Id);
			// LocalClient_CreatePlaceable(InMetadata.Id);
		} else {
			RemoteServer_CreatePlaceable(InMetadata.Id);
		}
	});

	FApiPlaceableMetadata PlaceableMetadata;
	PlaceableMetadata.Position = Location;
	PlaceableMetadata.Rotation = Rotation;
	PlaceableMetadata.Scale = FVector::OneVector;
	PlaceableMetadata.Type = TEXT("");
	PlaceableMetadata.PlaceableClassId = ClassMetadata.Id;

	// Create placeable
	PlaceableSubsystem->UpdatePlaceable(WorldId, PlaceableMetadata, Callback);
}

void UEditorComponent::RemoteServer_CreatePlaceable_Implementation(const FGuid& PlaceableId) {
	LocalServer_CreatePlaceable(PlaceableId);
}

void UEditorComponent::LocalServer_CreatePlaceable(const FGuid& PlaceableId) {
	GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api, Placeable);
	if (!PlaceableSubsystem) {
		return;
	}

	if (!PlaceableId.IsValid()) {
		return;
	}

	const auto Callback = MakeShared<FOnPlaceableRequestCompleted>();
	Callback->BindLambda([=](const FApiPlaceableMetadata& ApiMetadata, const EApiResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode != EApiResponseCode::Ok) {
			VeLogError("failed to create placeable actor: %s", *InError);
			return;
		}

		const FVePlaceableMetadata VePlaceableMetadata = FVePlaceableMetadata(ApiMetadata);
		AActor* PlaceableActor = UPlaceableLib::SpawnPlaceable(this, VePlaceableMetadata);
		if (auto* PlaceableComponent = UPlaceableLib::GetPlaceableComponent(PlaceableActor)) {
			PlaceableComponent->LocalServer_OnPlaced(VePlaceableMetadata);
		}
	});

	PlaceableSubsystem->GetPlaceable(PlaceableId, Callback);
}

// void UEditorTransformComponent::LocalClient_CreatePlaceable(const FVePlaceableMetadata& Metadata) {
// 	
// }

void UEditorComponent::CreatePlaceable(const FVePlaceableMetadata& Metadata) {}

void UEditorComponent::CreateTemporaryPlaceable(const FVePlaceableMetadata& Metadata) {
	auto PlayerController = GetPlayerController();
	if (!IsValid(PlayerController)) {
		return;
	}

	AActor* PlaceableActor = UPlaceableLib::SpawnPlaceable(this, Metadata);
	// if (IPlaceable* PlaceableInterface = Cast<IPlaceable>(PlaceableActor)) {
	// 	PlaceableInterface->Execute_OnPlaced(PlaceableActor);
	// }
}

#pragma endregion Create Placeable Object

#pragma region Transform Placeable Object

void UEditorComponent::Owner_OnTransformComplete() {
	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (!EditorSubsystem) {
		return;
	}

	auto* SelectedActor = GetSelectedActor();
	if (!IsValid(SelectedActor)) {
		return;
	}

	auto* SelectedPlaceableComponent = SelectedActor->FindComponentByClass<UPlaceableComponent>();
	if (!IsValid(SelectedPlaceableComponent)) {
		return;
	}

	// FGuid WorldId;
	// if (const UWorld* World = GetWorld()) {
	// 	if (const auto* VeGameState = World->GetGameState<AVeGameStateBase>()) {
	// 		WorldId = VeGameState->GetWorldId();
	// 	}
	// }
	//
	// if (!WorldId.IsValid()) {
	// 	VeLogWarningFunc("Invalid world id, spawning temporary placeable actor");
	// 	return;
	// }

	GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api, Placeable);
	if (!PlaceableSubsystem) {
		return;
	}

	FTransform Transform = SelectedActor->GetActorTransform();

	const auto Callback = MakeShared<FOnPlaceableRequestCompleted>();
	Callback->BindLambda([=](const FApiPlaceableMetadata& InMetadata, const EApiResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode != EApiResponseCode::Ok) {
			VeLogErrorFunc("Failed to place an actor: %s", *InError);
			return;
		}

		const FVePlaceableMetadata Metadata{InMetadata};

		if (IsValid(SelectedActor)) {
			if (GetNetMode() == NM_Standalone) {
				LocalServer_OnTransformComplete(SelectedActor, InMetadata.GetTransform());
				// LocalClient_OnTransformComplete(SelectedActor, Transform);
			} else {
				RemoteServer_OnTransformComplete(SelectedActor, InMetadata.GetTransform());
			}
		}
	});

	FApiUpdatePlaceableTransformMetadata ApiUpdatePlaceableTransformMetadata;
	ApiUpdatePlaceableTransformMetadata.Id = SelectedPlaceableComponent->GetMetadata().Id;
	ApiUpdatePlaceableTransformMetadata.Position = Transform.GetLocation();
	ApiUpdatePlaceableTransformMetadata.Rotation = Transform.GetRotation().Rotator();
	ApiUpdatePlaceableTransformMetadata.Scale = Transform.GetScale3D();

	PlaceableSubsystem->UpdatePlaceableTransform(ApiUpdatePlaceableTransformMetadata, Callback);
}

void UEditorComponent::RemoteServer_OnTransformComplete_Implementation(AActor* Actor, const FTransform& Transform) {
	LocalServer_OnTransformComplete(Actor, Transform);
}

void UEditorComponent::LocalServer_OnTransformComplete(AActor* Actor, const FTransform& Transform) {
	Actor->GetRootComponent()->SetMobility(EComponentMobility::Type::Movable);
	Actor->SetActorTransform(Transform);

	if (auto* PlaceableComponent = GetPlaceableComponent(Actor)) {
		const FString ActorName = PlaceableComponent->GetMetadata().Id.ToString(EGuidFormats::DigitsWithHyphensLower);
		VeLogFunc("Placeable transformed: {%s}", *ActorName);
	} else {
		const FString ActorName = Actor->GetName();
		VeLogFunc("Actor transformed: {%s}", *ActorName);
	}
}

#pragma endregion Transform Placeable Object

void UEditorComponent::DebugHitHelper(const FHitResult& HitResult) {
	FVector WorldRayStart, WorldRayEnd;
	Owner_GetMouseTraceWorldRay(WorldRayStart, WorldRayEnd);
	FVector const LineStart = WorldRayStart;
	FVector const LineEnd = HitResult.Location;
	FColor const Color = FColor::Red;
	bool bPersistentLines = false;
	float LifeTime = 10.0f;
	uint8 DepthPriority = 0;
	float Thickness = 1.0f;
	DrawDebugLine(GetWorld(), LineStart, LineEnd, Color, bPersistentLines, LifeTime, DepthPriority, Thickness);

	auto* Actor = HitResult.GetActor();
	if (Actor) {
		VeLog(">>> HIT Actor = %p (%s)", Actor, *Actor->GetFName().ToString());
	}
}
