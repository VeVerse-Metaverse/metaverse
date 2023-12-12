// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiCharacterBase.h"

#include "CogAiNpcSettingsComponent.h"
#include "VeAiController.h"
#include "VeAiPlayerState.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ACogAiCharacterBase::ACogAiCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	// Character should tick.
	PrimaryActorTick.bCanEverTick = true;

	// Character relevancy.
	bAlwaysRelevant = true;

	// Set the default character replication distance to 1km (for the case when always relevant is false).
	NetCullDistanceSquared = FMath::Square(10000.0f);
}

void ACogAiCharacterBase::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	// Run only on server.
	if (HasAuthority()) {

		// Check if the controller has a correct type.
		if (auto* AIController = Cast<AVeAiController>(NewController); IsValid(AIController)) {

			// Tell the AI controller what user ID to use. This will make controller to request user metadata from the API and set it on the player state (including persona).
			if (UserId.IsValid()) {
				AIController->SetUserId(UserId);
			}

			// Register with AI player state received from the controller. The controller should have a valid player state at this point.
			if (auto* State = AIController->GetAiPlayerState()) {
				SetAiPlayerState(State);
			}
		}
	}
}

void ACogAiCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate the AI player state.
	DOREPLIFETIME(ACogAiCharacterBase, AiPlayerState);
}

void ACogAiCharacterBase::SetAiPlayerState(AVeAiPlayerState* NewPlayerState) {
	// Unregister from the old AI player state.
	if (AiPlayerState && AiPlayerState->GetPawn() == this) {
		AiPlayerState->SetPawn(nullptr);
	}

	// Set the new AI player state.
	AiPlayerState = NewPlayerState;

	// Register with the new AI player state, notifying that it is now associated with this pawn.
	if (AiPlayerState) {
		AiPlayerState->SetPawn(this);
	}

	// Notify that the AI player state has changed.
	OnAiPlayerStateChange();
}

void ACogAiCharacterBase::OnRep_AiPlayerState() {
	// todo: Check if we really need to call this on the client or it would be replicated from the server correctly.
}

void ACogAiCharacterBase::OnAiPlayerStateChange() {
	// Notify blueprints that the AI player state has changed.
	OnAiPlayerStateChangedNative.Broadcast(AiPlayerState);

	// Run the code only on the server.
	if (HasAuthority()) {

		// Validate the player state.
		if (IsValid(AiPlayerState)) {

			// Subscribe to the user metadata updated event.
			AiPlayerState->GetOnAiUserMetadataUpdated().AddWeakLambda(this, [&](const FVeUserMetadata& InUserMetadata) {
				if (HasAuthority()) {
					OnAiUserMetadataUpdated(InUserMetadata);
				}
			});
		}
	}
}

void ACogAiCharacterBase::OnAiUserMetadataUpdated_Implementation(const FVeUserMetadata& InUserMetadata) {
	if (auto* SettingsComponent = FindComponentByClass<UCogAiNpcSettingsComponent>(); IsValid(SettingsComponent)) {
		SettingsComponent->OnAiUserMetadataUpdated(InUserMetadata);
	}
}
