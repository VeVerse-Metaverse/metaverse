// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VePlayerStateBase.h"

#include "Net/UnrealNetwork.h"
#include "VeShared.h"
#include "VeGameFramework.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"

void AVePlayerStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVePlayerStateBase, UserMetadata);
	DOREPLIFETIME(AVePlayerStateBase, bIsSpeaking);
	DOREPLIFETIME(AVePlayerStateBase, SpeechRecognitionMessage);
}

void AVePlayerStateBase::PostInitializeComponents() {
	Super::PostInitializeComponents();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
			if (AuthSubsystem->IsAuthorized()) {
				Login_SetUserMetadata(FVeUserMetadata(AuthSubsystem->GetUserMetadata()));
			} else {
				Login_SetUserMetadata(FVeUserMetadata());
			}
		};

		AuthSubsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
		OnChangeAuthorizationStateCallback(AuthSubsystem->GetAuthorizationState());
	}
}

void AVePlayerStateBase::BeginPlay() {
	Super::BeginPlay();
}

void AVePlayerStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnChangeAuthorizationState().RemoveAll(this);
	}
}

//------------------------------------------------------------------------------
#pragma region InitializationComplete
#if 0

bool AVePlayerStateBase::IsClientInitializationComplete() const {
	return bClientInitializationComplete;
}

bool AVePlayerStateBase::IsServerInitializationComplete() const {
	return bServerInitializationComplete;
}

bool AVePlayerStateBase::NativeCheckClientInitializationComplete() {
	return CheckClientInitializationComplete();
}

bool AVePlayerStateBase::NativeCheckServerInitializationComplete() {
	return CheckServerInitializationComplete();
}

bool AVePlayerStateBase::CheckClientInitializationComplete_Implementation() {
	return true;
}

bool AVePlayerStateBase::CheckServerInitializationComplete_Implementation() {
	return true;
}

void AVePlayerStateBase::ClientInitializationComplete() {
	if (bClientInitializationComplete) {
		return;
	}

	if (!NativeCheckClientInitializationComplete()) {
		return;
	}

	Server_InitializationComplete();

	if (!bClientInitializationComplete) {
		bClientInitializationComplete = true;
		NativeOnClientInitializationComplete();
	}
}

void AVePlayerStateBase::ServerInitializationComplete() {
	if (bServerInitializationComplete) {
		return;
	}

	if (!NativeCheckServerInitializationComplete()) {
		return;
	}

	Client_InitializationComplete();

	if (!bServerInitializationComplete) {
		bServerInitializationComplete = true;
		NativeOnServerInitializationComplete();
	}
}

void AVePlayerStateBase::NativeOnClientInitializationComplete() {
	OnClientInitializationComplete_BP();
	OnClientInitializationComplete.Broadcast(this);
}

void AVePlayerStateBase::NativeOnServerInitializationComplete() {
	OnServerInitializationComplete_BP();
	OnServerInitializationComplete.Broadcast(this);
}

void AVePlayerStateBase::Client_InitializationComplete_Implementation() {
	bServerInitializationComplete = true;
	NativeOnServerInitializationComplete();
}

void AVePlayerStateBase::Server_InitializationComplete_Implementation() {
	bClientInitializationComplete = true;
	NativeOnClientInitializationComplete();
}

#endif
#pragma endregion InitializationComplete
//------------------------------------------------------------------------------

void AVePlayerStateBase::Login_SetUserMetadata(const FVeUserMetadata& InUserMetadata) {
	if (!HasAuthority()) {
		return;
	}

	UserMetadata = InUserMetadata;
	NativeOnUserMetadataUpdated();
}

void AVePlayerStateBase::OnRep_UserMetadata() {
	NativeOnUserMetadataUpdated();
}

void AVePlayerStateBase::NativeOnUserMetadataUpdated() {
	OnUserMetadataUpdated.Broadcast(UserMetadata);
}

void AVePlayerStateBase::Authority_SetIsSpeaking(const bool bInIsSpeaking) {
	if (!HasAuthority()) {
		return;
	}

	bIsSpeaking = bInIsSpeaking;
	OnRep_IsSpeaking();
}

void AVePlayerStateBase::OnRep_IsSpeaking() const {
	LogF(">>> AVePlayerState::OnRep_IsSpeaking");
	OnIsSpeakingChanged.Broadcast(bIsSpeaking);
}

#pragma region Speech Recognition

void AVePlayerStateBase::OnSpeechRecognitionCompleted(const FString& InResult) {
	SpeechRecognitionMessage = InResult;

	// Call delegates on the server
	SpeechRecognizedEvent.Broadcast(SpeechRecognitionMessage);
	OnSpeechRecognized.Broadcast(SpeechRecognitionMessage);
}

void AVePlayerStateBase::OnRep_SpeechRecognitionMessage() const {
	// Call only on the remote clients
	if (!HasAuthority()) {
		ClientSpeechRecognitionComplete();
	}
}

void AVePlayerStateBase::ClientSpeechRecognitionComplete() const {
	SpeechRecognizedEvent.Broadcast(SpeechRecognitionMessage);
	OnSpeechRecognized.Broadcast(SpeechRecognitionMessage);
}

#pragma endregion 