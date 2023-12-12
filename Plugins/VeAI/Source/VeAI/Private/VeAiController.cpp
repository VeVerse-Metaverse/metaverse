// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeAiController.h"

#include "Api2UserSubsystem.h"
#include "VeAiPlayerState.h"
#include "VeApi2.h"
#include "VeShared.h"

void AVeAiController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	// Destroy the pawn when the controller is destroyed.
	if (GetPawn()) {
		GetPawn()->Destroy();
	}

	// Destroy the player state when the controller is destroyed.
	if (IsValid(AiPlayerState)) {
		AiPlayerState->Destroy();
	}
}

void AVeAiController::PostInitializeComponents() {
	Super::PostInitializeComponents();

	auto* const World = GetWorld();

	if (IsValid(this) && HasAuthority()) {
		// Select the player state class to spawn.
		auto* AiPlayerStateClassToSpawn = AiStateClass.Get();
		if (!IsValid(AiPlayerStateClassToSpawn)) {
			// Use the default player state class if none is specified.
			AiPlayerStateClassToSpawn = AVeAiPlayerState::StaticClass();
		}

		// Prepare the spawn parameters.
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.ObjectFlags |= RF_Transient;

		// Spawn the player state for the controller.
		AiPlayerState = World->SpawnActor<AVeAiPlayerState>(AiPlayerStateClassToSpawn);
	}
}

AVeAiPlayerState* AVeAiController::GetAiPlayerState() const {
	return AiPlayerState;
}

FGuid AVeAiController::GetUserId() const {
	if (const auto* VePlayerState = GetAiPlayerState<AVeAiPlayerState>(); IsValid(VePlayerState)) {
		return VePlayerState->GetUserId();
	}
	return FGuid();
}

void AVeAiController::SetUserId(const FGuid& InUserId) {
	if (!InUserId.IsValid()) {
		LogErrorF("[AVeAiController] invalid user id");
		return;
	}

	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (!UserSubsystem) {
		return;
	}

	const auto Callback = MakeShared<FOnUserRequestCompleted2>();
	Callback->BindWeakLambda(this, [=](const FApiUserMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		// Handle the error.
		if (InResponseCode != EApi2ResponseCode::Ok) {
			LogErrorF("[AVeAiController] failed to get user metadata: %s", *InError);
			return;
		}

		// Set the user metadata.
		if (auto* State = GetAiPlayerState<AVeAiPlayerState>(); IsValid(State)) {
			State->Login_SetUserMetadata(FVeUserMetadata(InMetadata));
		}
	});

	// Request user metadata.
	UserSubsystem->GetUser(InUserId, Callback);
}

void AVeAiController::StartSpeaking() {
	if (auto* State = GetAiPlayerState<AVeAiPlayerState>(); IsValid(State)) {
		State->Authority_SetIsSpeaking(true);
	}
}

void AVeAiController::StopSpeaking() {
	if (auto* State = GetAiPlayerState<AVeAiPlayerState>(); IsValid(State)) {
		State->Authority_SetIsSpeaking(false);
	}
}

void AVeAiController::OnActionTaken_Implementation(UCogAiActionMessage* ActionMessage) {
	
}
