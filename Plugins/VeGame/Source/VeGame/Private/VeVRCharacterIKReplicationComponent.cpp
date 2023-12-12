// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeVRCharacterIKReplicationComponent.h"

#include "Net/UnrealNetwork.h"

UVeVRCharacterIKReplicationComponent::UVeVRCharacterIKReplicationComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;

	SetIsReplicatedByDefault(true);
}

void UVeVRCharacterIKReplicationComponent::BeginPlay() {
	Super::BeginPlay();
}

void UVeVRCharacterIKReplicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Owner_SendIKTransforms();
}

void UVeVRCharacterIKReplicationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate IK transform to non-owning clients.
	DOREPLIFETIME_CONDITION(UVeVRCharacterIKReplicationComponent, IKTransform, COND_SkipOwner);
}

void UVeVRCharacterIKReplicationComponent::Owner_SendIKTransforms() {
	if (GetNetMode() != NM_Standalone) {
		if (IsLocallyControlled()) {
			ServerRPC_ReceiveIKTransform(IKTransform);
		}
	}
}

void UVeVRCharacterIKReplicationComponent::ServerRPC_ReceiveIKTransform_Implementation(const FVeVRCharacterIKTransform& InTransform) {
	IKTransform = InTransform;
}
