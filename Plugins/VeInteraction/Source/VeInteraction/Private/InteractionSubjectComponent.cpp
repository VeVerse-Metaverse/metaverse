// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "Components/InteractionSubjectComponent.h"

#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Components/InteractionObjectComponent.h"
#include "Pawns/InteractiveMotionController.h"
#include "VeShared.h"
#include "Interaction.h"
#include "InteractionNames.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Serialization/BufferArchive.h"
#include "InteractionFunctionLibrary.h"
#include "VePlayerControllerBase.h"


TArray<uint8> FInteractionEventPayload::Serialize() {
	FBufferArchive Buffer{};
	StaticStruct()->SerializeBin(Buffer, this);
	auto Bytes = static_cast<TArray<uint8>>(Buffer);
	return Bytes;
}

void FInteractionEventPayload::Deserialize(const TArray<uint8> InData) {
	FMemoryReader ArReader(InData);
	StaticStruct()->SerializeBin(ArReader, this);
}

UInteractionSubjectComponent::UInteractionSubjectComponent() {
	SetIsReplicatedByDefault(true);

	UActorComponent::SetAutoActivate(true);

	bWantsInitializeComponent = true;

	// See: UInteractionSubjectComponent::InitializeComponent
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.125f;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
}

void UInteractionSubjectComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UInteractionSubjectComponent::InitializeComponent() {
	Super::InitializeComponent();

	SetComponentTickEnabled(false);

	if (ControlMode == EInteractionControlMode::Unknown) {
		if (IsMotionController()) {
			ControlMode = EInteractionControlMode::MotionController;
		} else if (IsAttachedToPawn()) {
			ControlMode = EInteractionControlMode::Pawn;
		} else if (IsAttachedToPlayerController()) {
			ControlMode = EInteractionControlMode::Controller;
		}
	}

	const bool IsAuthority = (GetOwner()) ? GetOwner()->HasAuthority() : false;
	if (IsAuthority) {
		FindTriggerComponent();
		if (TriggerComponent) {
			TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &UInteractionSubjectComponent::OnOverlapBegin);
			TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &UInteractionSubjectComponent::OnOverlapEnd);
		}
	}

	if (!IsRunningDedicatedServer()) {
		CurrentMappingContexts.Append(GameMappingContexts);
	}
}

void UInteractionSubjectComponent::NativeOnPlayerControllerChanged(APlayerController* NewPlayerController, APlayerController* OldPlayerController) {
	if (!IsRunningDedicatedServer()) {
		if (OldPlayerController) {
			for (auto& MappingContext : GameMappingContexts) {
				UnbindInputActions(OldPlayerController->InputComponent.Get(), MappingContext);
			}

			if (auto* BasePlayerController = Cast<AVePlayerControllerBase>(OldPlayerController)) {
				BasePlayerController->UnregisterMappingContextsAll(EAppMode::Game, this);
			}
		}

		if (NewPlayerController) {
			for (auto& MappingContext : GameMappingContexts) {
				BindInputActions(NewPlayerController->InputComponent.Get(), MappingContext);
			}

			if (auto* BasePlayerController = Cast<AVePlayerControllerBase>(NewPlayerController)) {
				BasePlayerController->RegisterMappingContextArray(EAppMode::Game, this, CurrentMappingContexts.Array());
			}
		}
	}

	// Tick enable for authority only. UpdateTarget()
	const bool IsAuthority = (GetOwner()) ? GetOwner()->HasAuthority() : false;
	SetComponentTickEnabled(IsAuthority && NewPlayerController);

	Super::NativeOnPlayerControllerChanged(NewPlayerController, OldPlayerController);
}

void UInteractionSubjectComponent::RegisterMappingContext(UInputMappingContext* MappingContext) {
	if (IsRunningDedicatedServer()) {
		VeLogWarningFunc("Failed to register MappingContext for dedicated server.");
		return;
	}

	if (!MappingContext) {
		return;
	}

	// MappingContext already added
	if (CurrentMappingContexts.Contains(MappingContext)) {
		return;
	}

	CurrentMappingContexts.Emplace(MappingContext);

	if (GetPlayerController()) {
		BindInputActions(GetPlayerController()->InputComponent.Get(), MappingContext);

		if (auto* BasePlayerController = Cast<AVePlayerControllerBase>(GetPlayerController())) {
			BasePlayerController->RegisterMappingContext(EAppMode::Game, this, MappingContext);
		}
	}
}

void UInteractionSubjectComponent::UnregisterMappingContext(UInputMappingContext* MappingContext) {
	if (IsRunningDedicatedServer()) {
		VeLogWarningFunc("Failed to unregister MappingContext for dedicated server.");
		return;
	}

	if (!MappingContext) {
		return;
	}

	if (!CurrentMappingContexts.Remove(MappingContext)) {
		return;
	}

	if (GetPlayerController()) {
		UnbindInputActions(GetPlayerController()->InputComponent.Get(), MappingContext);

		auto* BasePlayerController = Cast<AVePlayerControllerBase>(GetPlayerController());
		if (BasePlayerController) {
			BasePlayerController->UnregisterMappingContext(EAppMode::Game, this, MappingContext);
		}
	}
}

void UInteractionSubjectComponent::BindInputActions(UInputComponent* InInputComponent, UInputMappingContext* MappingContext) {
	if (!InInputComponent || !MappingContext) {
		VeLogErrorFunc("Failed to bind InputActions");
		return;
	}

	auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InInputComponent);
	if (!EnhancedInputComponent) {
		VeLogErrorFunc("Requires Enhanced Input System to be activated in project settings to function properly.");
		return;
	}

	TSet<const UInputAction*> UniqueActions;
	for (const FEnhancedActionKeyMapping& Keymapping : MappingContext->GetMappings()) {
		if (Keymapping.Action) {
			UniqueActions.Add(Keymapping.Action);
		}
	}

	for (const auto* Action : UniqueActions) {
		if (ActionBindingHandles.Find(Action)) {
			continue;
		}

		VeLogVerboseFunc("Bind input action: %s from %s", *Action->GetName(), *MappingContext->GetName());
		auto& EnhancedInputActionEventBinding = EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &ThisClass::OnInputActionTriggered);
		ActionBindingHandles.Emplace(Action, EnhancedInputActionEventBinding.GetHandle());
	}
}

void UInteractionSubjectComponent::UnbindInputActions(UInputComponent* InInputComponent, UInputMappingContext* MappingContext) {
	if (!InInputComponent || !MappingContext) {
		VeLogErrorFunc("Failed to unbind InputActions");
		return;
	}

	auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InInputComponent);
	if (!EnhancedInputComponent) {
		VeLogErrorFunc("Requires Enhanced Input System to be activated in project settings to function properly.");
		return;
	}

	// Unbind Actions

	TSet<const UInputAction*> UniqueActions;
	for (const FEnhancedActionKeyMapping& Keymapping : MappingContext->GetMappings()) {
		if (Keymapping.Action) {
			UniqueActions.Add(Keymapping.Action);
		}
	}

	for (const auto* Action : UniqueActions) {
		auto* Handle = ActionBindingHandles.Find(Action);
		if (!Handle) {
			continue;
		}

		VeLogVerboseFunc("Unbind input action: %s from %s", *Action->GetName(), *MappingContext->GetName());
		EnhancedInputComponent->RemoveBindingByHandle(*Handle);
		ActionBindingHandles.Remove(Action);
	}
}

UPrimitiveComponent* UInteractionSubjectComponent::FindTriggerComponent() {
	if (const auto* Owner = GetOwner(); IsValid(Owner)) {
		if (TriggerComponent) {
			return TriggerComponent;
		}

		TriggerComponent = Owner->FindComponentByClass<UCapsuleComponent>();
		if (TriggerComponent) {
			return TriggerComponent;
		}

		TriggerComponent = Owner->FindComponentByClass<USphereComponent>();
		if (TriggerComponent) {
			return TriggerComponent;
		}

		TriggerComponent = Owner->FindComponentByClass<UBoxComponent>();
		if (TriggerComponent) {
			return TriggerComponent;
		}

		TriggerComponent = Owner->FindComponentByClass<UPrimitiveComponent>();
	}

	return TriggerComponent;
}

void UInteractionSubjectComponent::BeginPlay() {
	Super::BeginPlay();

}

void UInteractionSubjectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (const auto* Owner = GetOwner(); IsValid(Owner)) {
		FindTriggerComponent();

		if (TriggerComponent) {
			TriggerComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UInteractionSubjectComponent::OnOverlapBegin);
			TriggerComponent->OnComponentEndOverlap.RemoveDynamic(this, &UInteractionSubjectComponent::OnOverlapEnd);
		}
	}

	Super::EndPlay(EndPlayReason);
}

// Authority only
void UInteractionSubjectComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateTarget();
}

FInteractionObjectEventInfo UInteractionSubjectComponent::GetPawnFocusedTarget() const {
	FInteractionObjectEventInfo Info;

	const auto* Owner = GetOwner();
	if (!IsValid(Owner)) {
		VeLogErrorFunc("no owner");
		return Info;
	}

	if (Owner->GetLocalRole() == ROLE_SimulatedProxy) {
		VeLogWarningFunc("skipped on simulated proxy");
		return Info;
	}

	const auto PlayerController = GetPlayerController();
	if (!IsValid(PlayerController)) {
		VeLogErrorFunc("no player controller");
		return Info;
	}

	const auto World = GetWorld();
	if (!IsValid(World)) {
		VeLogErrorFunc("no world");
		return Info;
	}

	FVector CameraLocation;
	FRotator CameraRotation;

	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	const auto TraceStart = CameraLocation;
	const auto TraceDirection = CameraRotation.Vector();
	const auto TraceEnd = TraceStart + (TraceDirection * MaxFocusDistance);

	Info.Origin = TraceStart;

	// Do a collision line trace ignoring owner.
	FCollisionQueryParams CollisionQueryParams(NAME_TraceInteractionFocus, true, Owner);
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bTraceComplex = true;

	TArray<FHitResult> Hits;
	World->LineTraceMultiByChannel(Hits, TraceStart, TraceEnd, ECC_InteractionTraceChannel, CollisionQueryParams);

	for (auto Hit : Hits) {
		const auto* Actor = Hit.GetActor();

		if (!IsValid(Actor)) {
			continue;
		}

		auto* Object = Actor->FindComponentByClass<UInteractionObjectComponent>();

		if (!IsValid(Object)) {
			continue;
		}

		if (Object->FocusReplication == EInteractionReplication::None) {
			continue;
		}

		Info.Object = Object;
		Info.Location = Hit.ImpactPoint;
		Info.Normal = Hit.ImpactNormal;
	}

	return Info;
}

FInteractionObjectEventInfo UInteractionSubjectComponent::GetMotionControllerFocusedTarget() const {
	FInteractionObjectEventInfo Info;

	const auto* Owner = GetOwner();
	if (!IsValid(Owner)) {
		VeLogErrorFunc("no owning motion controller");
		return Info;
	}

	const auto World = GetWorld();
	if (!IsValid(World)) {
		VeLogErrorFunc("no world");
		return Info;
	}

	const auto MotionControllerComponent = Cast<UMotionControllerComponent>(Owner->GetComponentByClass(UMotionControllerComponent::StaticClass()));
	if (!IsValid(MotionControllerComponent)) {
		VeLogErrorFunc("no motion controller component");
		return Info;
	}

	const auto TraceStart = MotionControllerComponent->GetComponentLocation();
	const auto TraceDirection = MotionControllerComponent->GetForwardVector();
	const auto TraceEnd = TraceStart + (TraceDirection * MaxFocusDistance);

	Info.Origin = TraceStart;

	// Do a collision line trace ignoring owner.
	FCollisionQueryParams CollisionQueryParams(NAME_TraceInteractionFocus, true, Owner);
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bTraceComplex = false;

	TArray<FHitResult> Hits;
	World->LineTraceMultiByChannel(Hits, TraceStart, TraceEnd, ECC_InteractionTraceChannel, CollisionQueryParams);

	for (auto Hit : Hits) {
		const auto* Actor = Hit.GetActor();

		if (!IsValid(Actor)) {
			continue;
		}

		auto* Object = Actor->FindComponentByClass<UInteractionObjectComponent>();

		if (!IsValid(Object)) {
			continue;
		}

		if (Object->FocusReplication == EInteractionReplication::None) {
			continue;
		}

		Info.Object = Object;
		Info.Location = Hit.ImpactPoint;
		Info.Normal = Hit.ImpactNormal;
	}

	return Info;
}

FRotator UInteractionSubjectComponent::GetOwnerRotation() const {
	switch (ControlMode) {
	case EInteractionControlMode::PawnFP:
	case EInteractionControlMode::PawnTP:
	case EInteractionControlMode::PawnVR:
	case EInteractionControlMode::PawnMobile:
	case EInteractionControlMode::Pawn:
		if (const auto* PlayerController = GetPlayerController()) {
			return PlayerController->PlayerCameraManager->GetCameraRotation();
		}
	default:
		if (const auto* Owner = GetOwner()) {
			return Owner->GetActorRotation();
		}
	}

	return FRotator::ZeroRotator;
}

FInteractionObjectEventInfo UInteractionSubjectComponent::GetPawnOverlappedTarget() {
	FInteractionObjectEventInfo Info;

	if (!OverlappingObjects.Num()) {
		return Info;
	}

	const auto* Owner = GetOwner();
	if (!IsValid(Owner)) {
		VeLogErrorFunc("no owner");
		return Info;
	}

	UInteractionObjectComponent* Closest = nullptr;

	// Optimize the 360 degree overlap direction case.
	if (MinOverlapDirection == -1) {
		Closest = OverlappingObjects[0];
	} else {
		auto MaxDirectionDotProduct = MinOverlapDirection;

		FVector OwnerDirection = GetOwnerRotation().Vector();

		for (const auto& Object : OverlappingObjects) {
			const auto* ObjectOwner = Object->GetOwner();
			if (!IsValid(ObjectOwner)) {
				continue;
			}

			// Check if actor is in forward direction of the character.
			auto ActorDirection = ObjectOwner->GetActorLocation() - Owner->GetActorLocation();
			ActorDirection.Normalize();

			const auto DirectionDotProduct = FVector::DotProduct(OwnerDirection, ActorDirection);

			if (DirectionDotProduct > MaxDirectionDotProduct) {
				MaxDirectionDotProduct = DirectionDotProduct;
				Closest = Object;
			}
		}
	}

	if (!Closest) {
		return Info;
	}

	Info.Object = Closest;

	FVector TraceStart;
	[[maybe_unused]] FVector BoxExtentStart;
	Owner->GetActorBounds(false, TraceStart, BoxExtentStart);

	FVector TraceEnd;
	[[maybe_unused]] FVector BoxExtentEnd;
	const auto ObjectOwner = Closest->GetOwner();

	const auto World = GetWorld();
	if (!IsValid(World)) {
		VeLogErrorFunc("no world");
		return Info;
	}

	if (!IsValid(ObjectOwner)) {
		return Info;
	}

	ObjectOwner->GetActorBounds(false, TraceEnd, BoxExtentEnd);

	Info.Origin = TraceStart;

	// Do a collision line trace ignoring owner.
	FCollisionQueryParams CollisionQueryParams(NAME_TraceInteractionOverlap, true, Owner);
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_InteractionTraceChannel, CollisionQueryParams);

	const auto* Actor = Hit.GetActor();

	if (!IsValid(Actor)) {
		return Info;
	}

	auto* Object = Actor->FindComponentByClass<UInteractionObjectComponent>();

	if (!IsValid(Object) || Object != Closest) {
		return Info;
	}

	Info.Object = Object;
	Info.Location = Hit.ImpactPoint;
	Info.Normal = Hit.ImpactNormal;

	return Info;
}

FInteractionObjectEventInfo UInteractionSubjectComponent::GetMotionControllerOverlappedTarget() {
	FInteractionObjectEventInfo Info;

	const auto* Owner = GetOwner();
	if (!IsValid(Owner)) {
		VeLogErrorFunc("no owning motion controller");
		return Info;
	}

	const auto World = GetWorld();
	if (!IsValid(World)) {
		VeLogErrorFunc("no world");
		return Info;
	}

	const auto MotionControllerComponent = Cast<UMotionControllerComponent>(Owner->GetComponentByClass(UMotionControllerComponent::StaticClass()));
	if (!IsValid(MotionControllerComponent)) {
		VeLogErrorFunc("no motion controller component");
		return Info;
	}

	if (OverlappingObjects.Num() == 0) {
		return Info;
	}

	UInteractionObjectComponent* Closest = nullptr;

	// Optimize the 360 degree overlap direction case.
	if (MinOverlapDirection == -1) {
		Closest = OverlappingObjects[0];
	} else {
		auto MaxDirectionDotProduct = MinOverlapDirection;

		for (const auto& Object : OverlappingObjects) {
			const auto* ObjectOwner = Object->GetOwner();
			if (!ObjectOwner) {
				continue;
			}

			// Check if actor is in forward direction of the motion controller.
			auto Direction = ObjectOwner->GetActorLocation() - Owner->GetActorLocation();
			Direction.Normalize();

			const auto DirectionDotProduct = FVector::DotProduct(Owner->GetActorForwardVector(), Direction);

			if (DirectionDotProduct >= MaxDirectionDotProduct) {
				MaxDirectionDotProduct = DirectionDotProduct;
				Closest = Object;
			}
		}
	}

	const auto ObjectOwner = Closest->GetOwner();
	if (!IsValid(ObjectOwner)) {
		return Info;
	}

	FVector TraceStart = MotionControllerComponent->GetComponentLocation();
	FVector TraceEnd;
	[[maybe_unused]] FVector BoxExtentEnd;

	ObjectOwner->GetActorBounds(false, TraceEnd, BoxExtentEnd);

	Info.Origin = TraceStart;

	// Do a collision line trace ignoring self.
	FCollisionQueryParams CollisionQueryParams(NAME_TraceInteractionOverlap, true, Owner);
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_InteractionTraceChannel, CollisionQueryParams);

	const auto Actor = Hit.GetActor();

	if (!Actor) {
		return Info;
	}

	const auto Object = Actor->FindComponentByClass<UInteractionObjectComponent>();
	if (!IsValid(Object) || Object != Closest) {
		return Info;
	}

	Info.Object = Object;
	Info.Location = Hit.ImpactPoint;
	Info.Normal = Hit.ImpactNormal;

	return Info;
}

AActor* UInteractionSubjectComponent::GetMotionController() const {
	return Cast<AInteractiveMotionController>(GetOwner());
}

APawn* UInteractionSubjectComponent::GetPawn() const {
	if (const auto* MotionController = GetMotionController(); IsValid(MotionController)) {
		return Cast<APawn>(MotionController->GetOwner());
	}

	return Super::GetPawn();
}

bool UInteractionSubjectComponent::IsMotionController() const {
	return static_cast<bool>(Cast<AInteractiveMotionController>(GetOwner()));
}

// Called from the TickComponent.
void UInteractionSubjectComponent::UpdateTarget() {
	const auto NewFocusObject = (ControlMode == EInteractionControlMode::MotionController) ? GetMotionControllerFocusedTarget() : GetPawnFocusedTarget();

	if (NewFocusObject.Object != CurrentFocusedObject.Object) {
		OnFocusChanged(NewFocusObject, CurrentFocusedObject);
		CurrentFocusedObject = NewFocusObject;

		if (CurrentFocusedObject.Object.IsValid() && CurrentFocusedObject.Object != CurrentTargetedObject.Object) {
			OnTargetChanged(CurrentFocusedObject, CurrentTargetedObject);
			CurrentTargetedObject = CurrentFocusedObject;
			return;
		}
	} else if (NewFocusObject.Object.IsValid()) {
		return;
	}

	const auto NewOverlappedTargetObject = GetPawnOverlappedTarget();

	if (NewOverlappedTargetObject.Object != CurrentTargetedObject.Object) {
		OnTargetChanged(NewOverlappedTargetObject, CurrentTargetedObject);
		CurrentTargetedObject = NewOverlappedTargetObject;
	}
}

#pragma region Focus

void UInteractionSubjectComponent::OnFocusChanged(const FInteractionObjectEventInfo& NewObjectInfo, const FInteractionObjectEventInfo& OldObjectInfo) {
	if (OldObjectInfo.Object.IsValid()) {
		auto* Object = OldObjectInfo.Object.Get();
		switch (Object->FocusReplication) {
		case EInteractionReplication::None: {
			break;
		}
		case EInteractionReplication::Server: {
			if (IsServer()) {
				LocalServer_FocusEnd(Object);
			} else if (IsStandalone()) {
				LocalServer_FocusEnd(Object);
			}
			break;
		}
		case EInteractionReplication::Owner: {
			if (IsServer()) {
				LocalServer_FocusEnd(Object);
				RemoteOwner_FocusEnd(Object);
			} else if (IsStandalone()) {
				LocalServer_FocusEnd(Object);
				LocalClient_FocusEnd(Object);
			}
			break;
		}
		case EInteractionReplication::Multicast: {
			if (IsServer()) {
				LocalServer_FocusEnd(Object);
				RemoteMulticast_FocusEnd(Object);
			} else if (IsStandalone()) {
				LocalServer_FocusEnd(Object);
				RemoteMulticast_FocusEnd(Object);
			}
			break;
		}
		}
	}

	if (NewObjectInfo.Object.IsValid()) {
		auto* Object = NewObjectInfo.Object.Get();
		auto& Origin = NewObjectInfo.Origin;
		auto& Location = NewObjectInfo.Location;
		auto& Normal = NewObjectInfo.Normal;

		switch (Object->FocusReplication) {
		case EInteractionReplication::None: {
			break;
		}
		case EInteractionReplication::Server: {
			if (IsServer()) {
				LocalServer_FocusBegin(Object, Origin, Location, Normal);
			} else if (IsStandalone()) {
				LocalServer_FocusBegin(Object, Origin, Location, Normal);
			}
			break;
		}
		case EInteractionReplication::Owner: {
			if (IsServer()) {
				LocalServer_FocusBegin(Object, Origin, Location, Normal);
				RemoteOwner_FocusBegin(Object, Origin, Location, Normal);
			} else if (IsStandalone()) {
				LocalServer_FocusBegin(Object, Origin, Location, Normal);
				LocalClient_FocusBegin(Object, Origin, Location, Normal);
			}
			break;
		}
		case EInteractionReplication::Multicast: {
			if (IsServer()) {
				LocalServer_FocusBegin(Object, Origin, Location, Normal);
				RemoteMulticast_FocusBegin(Object, Origin, Location, Normal);
			} else if (IsStandalone()) {
				LocalServer_FocusBegin(Object, Origin, Location, Normal);
				RemoteMulticast_FocusBegin(Object, Origin, Location, Normal);
			}
			break;
		}
		}
	}
}


void UInteractionSubjectComponent::RemoteOwner_FocusBegin_Implementation(UInteractionObjectComponent* Object, const FVector_NetQuantize& Origin, const FVector_NetQuantize& Location, const FVector_NetQuantizeNormal& Normal) {
	LocalClient_FocusBegin(Object, Origin, Location, Normal);
}

void UInteractionSubjectComponent::RemoteMulticast_FocusBegin_Implementation(UInteractionObjectComponent* Object, const FVector_NetQuantize& Origin, const FVector_NetQuantize& Location, const FVector_NetQuantizeNormal& Normal) {
	if (!IsRunningDedicatedServer()) {
		LocalClient_FocusBegin(Object, Origin, Location, Normal);
	}
}

void UInteractionSubjectComponent::LocalServer_FocusBegin(UInteractionObjectComponent* Object, const FVector& Origin, const FVector& Location, const FVector& Normal) {
	if (IsValid(Object)) {
		Object->Server_FocusBegin(this, Origin, Location, Normal);
	}
}

void UInteractionSubjectComponent::LocalClient_FocusBegin(UInteractionObjectComponent* Object, const FVector& Origin, const FVector& Location, const FVector& Normal) {
	if (IsValid(Object)) {
		Object->Client_FocusBegin(this, Origin, Location, Normal);
	}
}

void UInteractionSubjectComponent::RemoteOwner_FocusEnd_Implementation(UInteractionObjectComponent* Object) {
	LocalClient_FocusEnd(Object);
}

void UInteractionSubjectComponent::RemoteMulticast_FocusEnd_Implementation(UInteractionObjectComponent* Object) {
	if (!IsRunningDedicatedServer()) {
		LocalClient_FocusEnd(Object);
	}
}

void UInteractionSubjectComponent::LocalServer_FocusEnd(UInteractionObjectComponent* Object) {
	if (IsValid(Object)) {
		Object->Server_FocusEnd(this);
	}
}

void UInteractionSubjectComponent::LocalClient_FocusEnd(UInteractionObjectComponent* Object) {
	if (IsValid(Object)) {
		Object->Client_FocusEnd(this);
	}
}

#pragma endregion Focus

#pragma region Overlap

void UInteractionSubjectComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	auto* Object = OtherActor->FindComponentByClass<UInteractionObjectComponent>();
	if (!IsValid(Object)) {
		return;
	}

	const auto Owner = GetOwner();
	if (!Owner) {
		VeLogErrorFunc("no owner");
		return;
	}

	// Do not overlap self
	if (Owner == OtherActor) {
		return;
	}

	switch (Object->OverlapReplication) {
	case EInteractionReplication::None: {
		break;
	}
	case EInteractionReplication::Server: {
		if (IsServer()) {
			LocalServer_OverlapBegin(Object);
		} else if (IsStandalone()) {
			LocalServer_OverlapBegin(Object);
		}
		break;
	}
	case EInteractionReplication::Owner: {
		if (IsServer()) {
			LocalServer_OverlapBegin(Object);
			RemoteOwner_OverlapBegin(Object);
		} else if (IsStandalone()) {
			LocalServer_OverlapBegin(Object);
			LocalClient_OverlapBegin(Object);
		}
		break;
	}
	case EInteractionReplication::Multicast: {
		if (IsServer()) {
			LocalServer_OverlapBegin(Object);
			RemoteMulticast_OverlapBegin(Object);
		} else if (IsStandalone()) {
			LocalServer_OverlapBegin(Object);
			RemoteMulticast_OverlapBegin(Object);
		}
		break;
	}
	}

	if (Object->OverlapReplication != EInteractionReplication::None) {
		OverlappingObjects.AddUnique(Object);
	}
}

void UInteractionSubjectComponent::RemoteOwner_OverlapBegin_Implementation(UInteractionObjectComponent* Object) {
	LocalClient_OverlapBegin(Object);
}

void UInteractionSubjectComponent::RemoteMulticast_OverlapBegin_Implementation(UInteractionObjectComponent* Object) {
	if (!IsRunningDedicatedServer()) {
		LocalClient_OverlapBegin(Object);
	}
}

void UInteractionSubjectComponent::LocalServer_OverlapBegin(UInteractionObjectComponent* Object) {
	if (IsValid(Object)) {
		Object->Server_OverlapBegin(this);
	}
}

void UInteractionSubjectComponent::LocalClient_OverlapBegin(UInteractionObjectComponent* Object) {
	if (IsValid(Object)) {
		Object->Client_OverlapBegin(this);
	}
}

void UInteractionSubjectComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	auto* Object = OtherActor->FindComponentByClass<UInteractionObjectComponent>();
	if (!IsValid(Object)) {
		return;
	}

	const auto Owner = GetOwner();
	if (!IsValid(Owner)) {
		VeLogErrorFunc("no owner");
		return;
	}

	// Do not overlap self
	if (Owner == OtherActor) {
		return;
	}

	switch (Object->OverlapReplication) {
	case EInteractionReplication::None: {
		break;
	}
	case EInteractionReplication::Server: {
		if (IsServer()) {
			LocalServer_OverlapEnd(Object);
		} else if (IsStandalone()) {
			LocalServer_OverlapEnd(Object);
		}
		break;
	}
	case EInteractionReplication::Owner: {
		if (IsServer()) {
			LocalServer_OverlapEnd(Object);
			RemoteOwner_OverlapEnd(Object);
		} else if (IsStandalone()) {
			LocalServer_OverlapEnd(Object);
			LocalClient_OverlapEnd(Object);
		}
		break;
	}
	case EInteractionReplication::Multicast: {
		if (IsServer()) {
			LocalServer_OverlapEnd(Object);
			RemoteMulticast_OverlapEnd(Object);
		} else if (IsStandalone()) {
			LocalServer_OverlapEnd(Object);
			RemoteMulticast_OverlapEnd(Object);
		}
		break;
	}
	}

	if (Object->OverlapReplication != EInteractionReplication::None) {
		OverlappingObjects.Remove(Object);
	}
}

void UInteractionSubjectComponent::RemoteOwner_OverlapEnd_Implementation(UInteractionObjectComponent* Object) {
	LocalClient_OverlapEnd(Object);
}

void UInteractionSubjectComponent::RemoteMulticast_OverlapEnd_Implementation(UInteractionObjectComponent* Object) {
	if (!IsRunningDedicatedServer()) {
		LocalClient_OverlapEnd(Object);
	}
}

void UInteractionSubjectComponent::LocalServer_OverlapEnd(UInteractionObjectComponent* Object) {
	if (IsValid(Object)) {
		Object->Server_OverlapEnd(this);
	}
}

void UInteractionSubjectComponent::LocalClient_OverlapEnd(UInteractionObjectComponent* Object) {
	if (IsValid(Object)) {
		Object->Client_OverlapEnd(this);
	}
}

#pragma endregion Overlap

#pragma region Target

void UInteractionSubjectComponent::OnTargetChanged(const FInteractionObjectEventInfo& NewObjectInfo, const FInteractionObjectEventInfo& OldObjectInfo) {

	if (OldObjectInfo.Object.IsValid()) {
		auto* Object = OldObjectInfo.Object.Get();
		switch (Object->TargetReplication) {
		case EInteractionReplication::None: {
			break;
		}
		case EInteractionReplication::Server: {
			if (IsServer()) {
				LocalServer_TargetEnd(Object);
			} else if (IsStandalone()) {
				LocalServer_TargetEnd(Object);
			}
			break;
		}
		case EInteractionReplication::Owner: {
			if (IsServer()) {
				LocalServer_TargetEnd(Object);
				RemoteOwner_TargetEnd(Object);
			} else if (IsStandalone()) {
				LocalServer_TargetEnd(Object);
				LocalClient_TargetEnd(Object);
			}
			break;
		}
		case EInteractionReplication::Multicast: {
			if (IsServer()) {
				LocalServer_TargetEnd(Object);
				RemoteMulticast_TargetEnd(Object);
			} else if (IsStandalone()) {
				LocalServer_TargetEnd(Object);
				RemoteMulticast_TargetEnd(Object);
			}
			break;
		}
		}
	}

	if (NewObjectInfo.Object.IsValid()) {
		auto* Object = NewObjectInfo.Object.Get();
		switch (Object->TargetReplication) {
		case EInteractionReplication::None: {
			break;
		}
		case EInteractionReplication::Server: {
			if (IsServer()) {
				LocalServer_TargetBegin(Object);
			} else if (IsStandalone()) {
				LocalServer_TargetBegin(Object);
			}
			break;
		}
		case EInteractionReplication::Owner: {
			if (IsServer()) {
				LocalServer_TargetBegin(Object);
				RemoteOwner_TargetBegin(Object);
			} else if (IsStandalone()) {
				LocalServer_TargetBegin(Object);
				LocalClient_TargetBegin(Object);
			}
			break;
		}
		case EInteractionReplication::Multicast: {
			if (IsServer()) {
				LocalServer_TargetBegin(Object);
				RemoteMulticast_TargetBegin(Object);
			} else if (IsStandalone()) {
				LocalServer_TargetBegin(Object);
				RemoteMulticast_TargetBegin(Object);
			}
			break;
		}
		}
	}
}

void UInteractionSubjectComponent::RemoteOwner_TargetBegin_Implementation(UInteractionObjectComponent* Object) {
	LocalClient_TargetBegin(Object);
}

void UInteractionSubjectComponent::RemoteMulticast_TargetBegin_Implementation(UInteractionObjectComponent* Object) {
	if (!IsRunningDedicatedServer()) {
		LocalClient_TargetBegin(Object);
	}
}

void UInteractionSubjectComponent::LocalServer_TargetBegin(UInteractionObjectComponent* Object) {
	if (IsValid(Object)) {
		Object->Server_TargetBegin(this);
	}
}

void UInteractionSubjectComponent::LocalClient_TargetBegin(UInteractionObjectComponent* Object) {
	if (IsValid(Object)) {
		Object->Client_TargetBegin(this);
	}
}

void UInteractionSubjectComponent::RemoteOwner_TargetEnd_Implementation(UInteractionObjectComponent* Object) {
	LocalClient_TargetEnd(Object);
}

void UInteractionSubjectComponent::RemoteMulticast_TargetEnd_Implementation(UInteractionObjectComponent* Object) {
	if (!IsRunningDedicatedServer()) {
		LocalClient_TargetEnd(Object);
	}
}

void UInteractionSubjectComponent::LocalServer_TargetEnd(UInteractionObjectComponent* Object) {
	if (IsValid(Object)) {
		Object->Server_TargetEnd(this);
	}
}

void UInteractionSubjectComponent::LocalClient_TargetEnd(UInteractionObjectComponent* Object) {
	if (IsValid(Object)) {
		Object->Client_TargetEnd(this);
	}
}

#pragma endregion Target

#pragma region EnhancedInput

void UInteractionSubjectComponent::OnInputActionTriggered(const FInputActionInstance& ActionInstance) {
	RemoteServer_InputAction(
		ActionInstance.GetTriggerEvent(),
		ActionInstance.GetValue().Get<FVector>(),
		ActionInstance.GetElapsedTime(),
		ActionInstance.GetTriggeredTime(),
		ActionInstance.GetSourceAction()
		);
}

void UInteractionSubjectComponent::RemoteServer_InputAction_Implementation(ETriggerEvent TriggerEvent, FVector ActionValue, float ElapsedSeconds, float TriggeredSeconds, const UInputAction* InputAction) {
	UInteractionObjectComponent* Object = GetTargetObject();

	EInteractionReplication Object_InputActionReplication = (Object) ? Object->InputActionReplication : EInteractionReplication::None;

	// case EInteractionReplication::Server:
	if (InputActionReplication >= EInteractionReplication::Server || Object_InputActionReplication >= EInteractionReplication::Server) {
		if (IsServer()) {
			LocalServer_InputAction(Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		} else if (IsStandalone()) {
			LocalServer_InputAction(Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		}
	}

	// case EInteractionReplication::Multicast:
	if (InputActionReplication == EInteractionReplication::Multicast || Object_InputActionReplication == EInteractionReplication::Multicast) {
		if (IsServer()) {
			RemoteMulticast_InputAction(Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		} else if (IsStandalone()) {
			RemoteMulticast_InputAction(Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		}
	}

	// case EInteractionReplication::Owner:
	else if (InputActionReplication == EInteractionReplication::Owner || Object_InputActionReplication == EInteractionReplication::Owner) {
		if (IsServer()) {
			RemoteOwner_InputAction(Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		} else if (IsStandalone()) {
			LocalClient_InputAction(Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		}
	}
}

void UInteractionSubjectComponent::RemoteOwner_InputAction_Implementation(UInteractionObjectComponent* Object, ETriggerEvent TriggerEvent, FVector ActionValue, float ElapsedSeconds, float TriggeredSeconds, const UInputAction* InputAction) {
	LocalClient_InputAction(Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
}

void UInteractionSubjectComponent::RemoteMulticast_InputAction_Implementation(UInteractionObjectComponent* Object, ETriggerEvent TriggerEvent, FVector ActionValue, float ElapsedSeconds, float TriggeredSeconds, const UInputAction* InputAction) {
	if (!IsRunningDedicatedServer()) {
		LocalClient_InputAction(Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
	}
}

void UInteractionSubjectComponent::LocalServer_InputAction(UInteractionObjectComponent* Object, ETriggerEvent TriggerEvent, FVector ActionValue, float ElapsedSeconds, float TriggeredSeconds, const UInputAction* InputAction) {
	if (InputActionReplication >= EInteractionReplication::Server) {
		OnServerInputActionNative.Broadcast(this, Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		OnServerInputAction.Broadcast(this, Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
	}
	if (IsValid(Object) && Object->InputActionReplication >= EInteractionReplication::Server) {
		Object->Server_InputAction(this, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
	}
}

void UInteractionSubjectComponent::LocalClient_InputAction(UInteractionObjectComponent* Object, ETriggerEvent TriggerEvent, FVector ActionValue, float ElapsedSeconds, float TriggeredSeconds, const UInputAction* InputAction) {
	// Clients, not Owner
	if (GetLocalRole() == ROLE_SimulatedProxy) {
		if (InputActionReplication == EInteractionReplication::Multicast) {
			OnClientInputActionNative.Broadcast(this, Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
			OnClientInputAction.Broadcast(this, Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		}
		if (IsValid(Object) && Object->InputActionReplication == EInteractionReplication::Multicast) {
			Object->Client_InputAction(this, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		}
	}

	// Owner
	else {
		if (InputActionReplication >= EInteractionReplication::Owner) {
			OnClientInputActionNative.Broadcast(this, Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
			OnClientInputAction.Broadcast(this, Object, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		}
		if (IsValid(Object) && Object->InputActionReplication >= EInteractionReplication::Owner) {
			Object->Client_InputAction(this, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
		}
	}
}

#pragma endregion EnhancedInput

#pragma region Events - Binary

void UInteractionSubjectComponent::RemoteServer_Event_Binary_BP(UInteractionObjectComponent* Object, const FName Name, const TArray<uint8>& Payload, EEventReplicateToClients ReplicationMode, UInteractionSubjectComponent* Proxy) {
	RemoteServer_Event_Binary(Object, Name, Payload, ReplicationMode, Proxy);
}

void UInteractionSubjectComponent::RemoteClient_Event_Binary_BP(UInteractionObjectComponent* Object, const FName Name, const TArray<uint8>& Payload, bool Multicast) {
	if (Object) {
		if (Multicast) {
			RemoteMulticast_Event_Binary(Object, Name, Payload);
		} else {
			RemoteOwner_Event_Binary(Object, Name, Payload);
		}
	}
}

void UInteractionSubjectComponent::RemoteServer_Event_Binary_Implementation(UInteractionObjectComponent* Object, const FName Name, const TArray<uint8>& Payload, EEventReplicateToClients ReplicationMode, UInteractionSubjectComponent* Proxy) {
	if (IsValid(Proxy)) {
		Proxy->LocalServer_Event_Binary(Object, Name, Payload, ReplicationMode);
	} else {
		LocalServer_Event_Binary(Object, Name, Payload, ReplicationMode);
	}
}

void UInteractionSubjectComponent::RemoteOwner_Event_Binary_Implementation(UInteractionObjectComponent* Object, const FName Name, const TArray<uint8>& Payload) {
	LocalClient_Event_Binary(Object, Name, Payload);
}

void UInteractionSubjectComponent::RemoteMulticast_Event_Binary_Implementation(UInteractionObjectComponent* Object, const FName Name, const TArray<uint8>& Payload) {
	if (!IsRunningDedicatedServer()) {
		LocalClient_Event_Binary(Object, Name, Payload);
	}
}

void UInteractionSubjectComponent::LocalServer_Event_Binary(UInteractionObjectComponent* Object, const FName Name, const TArray<uint8>& Payload, EEventReplicateToClients ReplicationMode) {
	UInteractionObjectComponent* Target = nullptr;

	if (IsValid(Object)) {
		Target = Object;
	} else {
		Target = GetTargetObject();
	}

	if (IsValid(Target)) {
		Target->Server_Event_Binary(this, Name, Payload);
	}

	if (ReplicationMode == EEventReplicateToClients::Owning) {
		RemoteOwner_Event_Binary(Target, Name, Payload);
	} else if (ReplicationMode == EEventReplicateToClients::All) {
		RemoteMulticast_Event_Binary(Target, Name, Payload);
	}
}

void UInteractionSubjectComponent::LocalClient_Event_Binary(UInteractionObjectComponent* Object, const FName Name, const TArray<uint8>& Payload) {
	if (IsValid(Object)) {
		Object->Client_Event_Binary(this, Name, Payload);
	}
}

#pragma endregion Events - Binary

#pragma region Events - String

void UInteractionSubjectComponent::RemoteServer_Event_String_BP(UInteractionObjectComponent* Object, const FName Name, const FString& Payload, EEventReplicateToClients ReplicationMode, UInteractionSubjectComponent* Proxy) {
	RemoteServer_Event_String(Object, Name, Payload, ReplicationMode, Proxy);
}

void UInteractionSubjectComponent::RemoteClient_Event_String_BP(UInteractionObjectComponent* Object, const FName Name, const FString& Payload, bool Multicast) {
	if (Object) {
		if (Multicast) {
			RemoteMulticast_Event_String(Object, Name, Payload);
		} else {
			RemoteOwner_Event_String(Object, Name, Payload);
		}
	}
}

void UInteractionSubjectComponent::RemoteServer_Event_String_Implementation(UInteractionObjectComponent* Object, const FName Name, const FString& Payload, EEventReplicateToClients ReplicationMode, UInteractionSubjectComponent* Proxy) {
	if (IsValid(Proxy)) {
		Proxy->LocalServer_Event_String(Object, Name, Payload, ReplicationMode);
	} else {
		LocalServer_Event_String(Object, Name, Payload, ReplicationMode);
	}
}

void UInteractionSubjectComponent::RemoteOwner_Event_String_Implementation(UInteractionObjectComponent* Object, const FName Name, const FString& Payload) {
	LocalClient_Event_String(Object, Name, Payload);
}

void UInteractionSubjectComponent::RemoteMulticast_Event_String_Implementation(UInteractionObjectComponent* Object, const FName Name, const FString& Payload) {
	if (!IsRunningDedicatedServer()) {
		LocalClient_Event_String(Object, Name, Payload);
	}
}

void UInteractionSubjectComponent::LocalServer_Event_String(UInteractionObjectComponent* Object, const FName Name, const FString& Payload, EEventReplicateToClients ReplicationMode) {
	UInteractionObjectComponent* Target = nullptr;

	if (Object) {
		Target = Object;
	} else {
		Target = GetTargetObject();
	}

	if (IsValid(Target)) {
		Target->Server_Event_String(this, Name, Payload);
	}

	if (ReplicationMode == EEventReplicateToClients::Owning) {
		RemoteOwner_Event_String(Target, Name, Payload);
	} else if (ReplicationMode == EEventReplicateToClients::All) {
		RemoteMulticast_Event_String(Target, Name, Payload);
	}
}

void UInteractionSubjectComponent::LocalClient_Event_String(UInteractionObjectComponent* Object, const FName Name, const FString& Payload) {
	if (IsValid(Object)) {
		Object->Client_Event_String(this, Name, Payload);
	}
}

#pragma endregion Events - String

#pragma region Events - Object

void UInteractionSubjectComponent::RemoteServer_Event_Object_BP(UInteractionObjectComponent* Object, const FName Name, UObject* Payload, EEventReplicateToClients ReplicationMode, UInteractionSubjectComponent* Proxy) {
	if (Payload) {
		FString ClassName = Payload->GetClass()->GetName();
		TArray<uint8> Bytes = UInteractionFunctionLibrary::ObjectToBytes(Payload);
		RemoteServer_Event_Object(Object, Name, ClassName, Bytes, ReplicationMode, Proxy);
	}
}

void UInteractionSubjectComponent::RemoteClient_Event_Object_BP(UInteractionObjectComponent* Object, const FName Name, UObject* Payload, bool Multicast) {
	if (Payload) {
		FString ClassName = Payload->GetClass()->GetName();
		TArray<uint8> Bytes = UInteractionFunctionLibrary::ObjectToBytes(Payload);
		RemoteOwner_Event_Object(Object, Name, ClassName, Bytes);
	}
}

void UInteractionSubjectComponent::RemoteServer_Event_Object_Implementation(UInteractionObjectComponent* Object, const FName Name, const FString& ClassName, const TArray<uint8>& Payload, EEventReplicateToClients ReplicationMode, UInteractionSubjectComponent* Proxy) {
	if (IsValid(Proxy)) {
		Proxy->LocalServer_Event_Object(Object, Name, ClassName, Payload, ReplicationMode);
	} else {
		LocalServer_Event_Object(Object, Name, ClassName, Payload, ReplicationMode);
	}
}

void UInteractionSubjectComponent::RemoteOwner_Event_Object_Implementation(UInteractionObjectComponent* Object, const FName Name, const FString& ClassName, const TArray<uint8>& Payload) {
	LocalClient_Event_Object(Object, Name, ClassName, Payload);
}

void UInteractionSubjectComponent::RemoteMulticast_Event_Object_Implementation(UInteractionObjectComponent* Object, const FName Name, const FString& ClassName, const TArray<uint8>& Payload) {
	if (!IsRunningDedicatedServer()) {
		LocalClient_Event_Object(Object, Name, ClassName, Payload);
	}
}

void UInteractionSubjectComponent::LocalServer_Event_Object(UInteractionObjectComponent* Object, const FName Name, const FString& ClassName, const TArray<uint8>& Payload, EEventReplicateToClients ReplicationMode) {
	UInteractionObjectComponent* Target = nullptr;

	if (Object) {
		Target = Object;
	} else {
		Target = GetTargetObject();
	}

	if (IsValid(Target)) {
		UObject* PayloadObject = UInteractionFunctionLibrary::BytesToObject(ClassName, Payload);
		Target->Server_Event_Object(this, Name, PayloadObject);
	}

	if (ReplicationMode == EEventReplicateToClients::Owning) {
		RemoteOwner_Event_Object(Target, Name, ClassName, Payload);
	} else if (ReplicationMode == EEventReplicateToClients::All) {
		RemoteMulticast_Event_Object(Target, Name, ClassName, Payload);
	}
}

void UInteractionSubjectComponent::LocalClient_Event_Object(UInteractionObjectComponent* Object, const FName Name, const FString& ClassName, const TArray<uint8>& Payload) {
	if (IsValid(Object)) {
		UObject* PayloadObject = UInteractionFunctionLibrary::BytesToObject(ClassName, Payload);
		Object->Client_Event_Object(this, Name, PayloadObject);
	}
}

#pragma endregion Events - Object
