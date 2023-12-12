// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "PlaceableComponent.h"

#include "VeApi.h"
#include "VeShared.h"
#include "VeGameFramework.h"
#include "Components/ComboBoxKey.h"
#include "Net/UnrealNetwork.h"
#include "GameFrameworkEditorSubsystem.h"
#include "EditorComponent.h"


// Sets default values for this component's properties
UPlaceableComponent::UPlaceableComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UPlaceableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlaceableComponent, MetadataContainer);
}

void UPlaceableComponent::OnRep_MetadataContainer() {
	LocalClient_OnUpdated(MetadataContainer.Metadata);
}

void UPlaceableComponent::LocalServer_OnPlaced(const FVePlaceableMetadata& InMetadata) {
	MetadataContainer.SetMetadata(InMetadata);
	if (GetNetMode() == NM_Standalone) {
		LocalClient_OnPlaced(InMetadata);
	} else {
		OnPlaceableMetadataChanged.Broadcast(InMetadata, EPlaceableChangeType::Placed);
	}
}

void UPlaceableComponent::LocalServer_OnLoaded(const FVePlaceableMetadata& InMetadata) {
	MetadataContainer.SetMetadata(InMetadata);
	if (GetNetMode() == NM_Standalone) {
		LocalClient_OnLoaded(InMetadata);
	} else {
		OnPlaceableMetadataChanged.Broadcast(InMetadata, EPlaceableChangeType::Loaded);
	}
}

void UPlaceableComponent::LocalServer_OnUpdated(const FVePlaceableMetadata& InMetadata) {
	MetadataContainer.SetMetadata(InMetadata);
	if (GetNetMode() == NM_Standalone) {
		LocalClient_OnUpdated(InMetadata);
	} else {
		OnPlaceableMetadataChanged.Broadcast(InMetadata, EPlaceableChangeType::Updated);
	}
}

void UPlaceableComponent::LocalClient_OnPlaced(const FVePlaceableMetadata& InMetadata) {
	OnPlaceableMetadataChanged.Broadcast(InMetadata, EPlaceableChangeType::Placed);
}

void UPlaceableComponent::LocalClient_OnUpdated(const FVePlaceableMetadata& InMetadata) {
	OnPlaceableMetadataChanged.Broadcast(InMetadata, EPlaceableChangeType::Updated);
}

void UPlaceableComponent::LocalClient_OnLoaded(const FVePlaceableMetadata& InMetadata) {
	OnPlaceableMetadataChanged.Broadcast(InMetadata, EPlaceableChangeType::Loaded);
}

void UPlaceableComponent::RemoteClient_OnPlaced_Implementation(const FVePlaceableMetadata& InMetadata) {
	LocalClient_OnPlaced(InMetadata);
}

void UPlaceableComponent::RemoteClient_OnUpdated_Implementation(const FVePlaceableMetadata& InMetadata) {
	LocalClient_OnUpdated(InMetadata);
}

void UPlaceableComponent::RemoteClient_OnLoaded_Implementation(const FVePlaceableMetadata& InMetadata) {
	LocalClient_OnLoaded(InMetadata);
}

void UPlaceableComponent::OnTransformed(const FTransform& Transform) {
	if (auto* Owner = GetOwner(); IsValid(Owner)) {
		// todo: check if we can actually move it (check permissions)
		// todo: send request saving transform to the api
		VeLogFunc("\nPrevious Transform:\n%s\nNew Transform:\n%s", *Owner->GetActorTransform().ToHumanReadableString(), *Transform.ToHumanReadableString());
		// Component->SetWorldTransform(Transform);
		// Owner->SetActorTransform(Transform);
	}
}

void UPlaceableComponent::OnSelected() {
	VeLogFunc("selected");
	// todo: implement event for the owning actor
}

void UPlaceableComponent::OnDeselected() {
	VeLogFunc("deselected");
	// todo: implement event for the owning actor
}

void UPlaceableComponent::OnDestroyed() {
	VeLogFunc("destroyed");
	// todo: implement event to sent http to api
}

void UPlaceableComponent::BeginPlay() {
	Super::BeginPlay();
}

void UPlaceableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
