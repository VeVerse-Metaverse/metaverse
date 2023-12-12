// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeAiPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "VeShared.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"

AVeAiPlayerState::AVeAiPlayerState(const FObjectInitializer& ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = false;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	NetUpdateFrequency = 1;
	bNetLoadOnClient = false;
}

void AVeAiPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVeAiPlayerState, UserMetadata);
	DOREPLIFETIME(AVeAiPlayerState, bIsSpeaking);
}

void AVeAiPlayerState::PostInitializeComponents() {
	Super::PostInitializeComponents();
}

void AVeAiPlayerState::BeginPlay() {
	Super::BeginPlay();
}

void AVeAiPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnChangeAuthorizationState().RemoveAll(this);
	}
}

void AVeAiPlayerState::Login_SetUserMetadata(const FVeUserMetadata& InUserMetadata) {
	if (!HasAuthority()) {
		return;
	}

	UserMetadata = InUserMetadata;
	OnRep_UserMetadata();
}

void AVeAiPlayerState::OnRep_UserMetadata() const {
	OnAiUserMetadataUpdated.Broadcast(UserMetadata);
	OnUserMetadataUpdated.Broadcast(UserMetadata);
}

AVeAiPlayerState::FOnAiUserMetadataUpdated& AVeAiPlayerState::GetOnAiUserMetadataUpdated() {
	return OnAiUserMetadataUpdated;
}

void AVeAiPlayerState::Authority_SetIsSpeaking(const bool bInIsSpeaking) {
	if (!HasAuthority()) {
		return;
	}

	bIsSpeaking = bInIsSpeaking;
	OnRep_IsSpeaking();
}

void AVeAiPlayerState::OnRep_IsSpeaking() const {
	OnIsSpeakingChanged.Broadcast(bIsSpeaking);
}

void AVeAiPlayerState::OnPawnPrivateDestroyed(AActor* InActor) {
	if (InActor == PawnPrivate) {
		PawnPrivate = nullptr;
	}
}

void AVeAiPlayerState::SetPawnPrivate(APawn* InPawn) {
	if (InPawn != PawnPrivate) {
		if (PawnPrivate) {
			PawnPrivate->OnDestroyed.RemoveDynamic(this, &AVeAiPlayerState::OnPawnPrivateDestroyed);
		}
		PawnPrivate = InPawn;
		if (PawnPrivate) {
			PawnPrivate->OnDestroyed.AddDynamic(this, &AVeAiPlayerState::OnPawnPrivateDestroyed);
		}
	}
}

APawn* AVeAiPlayerState::GetPawn() {
	return PawnPrivate;
}

void AVeAiPlayerState::SetPawn(APawn* InPawn) {
	APawn* OldPawn = PawnPrivate;
	SetPawnPrivate(InPawn);
	OnPawnSet.Broadcast(this, PawnPrivate, OldPawn);
}
