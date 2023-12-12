// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeCharacterBase.h"

#include "UIUserInfoWidget.h"
#include "UserInfoWidgetComponent.h"
#include "VePlayerStateBase.h"
#include "Kismet/GameplayStatics.h"

#if WITH_VIVOX
#include "Voice/VivoxService.h"
#endif


AVeCharacterBase::AVeCharacterBase() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVeCharacterBase::BeginPlay() {
	Super::BeginPlay();

#if WITH_VIVOX
	if (const auto VivoxService = GetGameInstance()->GetSubsystem<UVivoxService>()) {
		VivoxService->OnVivoxVoiceStateChanged.AddUObject(this, &AVeCharacterBase::OnVivoxVoiceStateChangedCallback);
	}
#endif
}

void AVeCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

#if WITH_VIVOX
	if (const auto VivoxService = GetGameInstance()->GetSubsystem<UVivoxService>()) {
		VivoxService->OnVivoxVoiceStateChanged.RemoveAll(this);
	}
#endif
}

void AVeCharacterBase::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	NativeOnPlayerStateChanged(GetPlayerState());
}

void AVeCharacterBase::UnPossessed() {
	Super::UnPossessed();
	NativeOnPlayerStateChanged(GetPlayerState());
}

void AVeCharacterBase::OnRep_PlayerState() {
	Super::OnRep_PlayerState();
	NativeOnPlayerStateChanged(GetPlayerState());
}

void AVeCharacterBase::OnVivoxVoiceStateChangedCallback(const FGuid& UserId, bool bIsSpeaking) {
	auto* VePlayerState = GetPlayerState<AVePlayerStateBase>();
	if (IsValid(VePlayerState)) {
		VePlayerState->Authority_SetIsSpeaking(bIsSpeaking);
	}
}

#pragma region PlayerState

void AVeCharacterBase::OnReady_PlayerState(FPlayerStateDelegate Callback) {
	OnPlayerStateChanged.AddWeakLambda(this, [Callback](APlayerState* InPlayerState) {
		Callback.ExecuteIfBound(InPlayerState);
	});
	Callback.ExecuteIfBound(GetPlayerState());
}

void AVeCharacterBase::NativeOnPlayerStateChanged(APlayerState* InPlayerState) {
	OnPlayerStateChanged.Broadcast(InPlayerState);

	if (GetNetMode() != NM_DedicatedServer) {
		const auto* UserInfoWidgetComponent = FindComponentByClass<UUserInfoWidgetComponent>();
		if (IsValid(UserInfoWidgetComponent)) {
			UserInfoWidgetComponent->SetPlayerState(InPlayerState);
		}
	}
}

#pragma endregion PlayerState
