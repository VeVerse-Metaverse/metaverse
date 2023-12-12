// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeGameStateBase.h"

#include "VeShared.h"
#include "VeGameFramework.h"
#include "VeWorldSettings.h"

#undef VE_LOG_MODULE
#define VE_LOG_MODULE Initialization


AVeGameStateBase::AVeGameStateBase()
	: Super() {
	// PrimaryActorTick.bCanEverTick = true;
}

void AVeGameStateBase::BeginInitialization() {
	if (!IsNetMode(NM_Client)) {
		VeLog("[%p] Begin initialization: Server", this);
		BeginServerInitialization();
		ServerInitializationComplete();
	}
	if (!IsNetMode(NM_DedicatedServer)) {
		VeLog("[%p] Begin initialization: Client", this);
		BeginClientInitialization();
		ClientInitializationComplete();
	}
}

//------------------------------------------------------------------------------
#pragma region InitializationComplete


void AVeGameStateBase::ClientInitializationComplete() {
	if (bClientInitializationComplete) {
		return;
	}

	const bool Check = NativeCheckClientInitializationComplete();
	VeLog("[%p] GameState check initialization: Client; %d", this, Check);
	if (!Check) {
		return;
	}

	bClientInitializationComplete = true;
	NativeOnClientInitializationComplete();
}

void AVeGameStateBase::ServerInitializationComplete() {
	if (bServerInitializationComplete) {
		return;
	}

	const bool Check = NativeCheckServerInitializationComplete();
	VeLog("[%p] GameState check initialization: Server; %d", this, Check);
	if (!Check) {
		return;
	}

	bServerInitializationComplete = true;
	NativeOnServerInitializationComplete();
}

bool AVeGameStateBase::NativeCheckClientInitializationComplete() {
	return CheckClientInitializationComplete();
}

bool AVeGameStateBase::NativeCheckServerInitializationComplete() {
	return CheckServerInitializationComplete();
}

bool AVeGameStateBase::CheckClientInitializationComplete_Implementation() {
	return true;
}

bool AVeGameStateBase::CheckServerInitializationComplete_Implementation() {
	return true;
}

void AVeGameStateBase::NativeOnClientInitializationComplete() {
	VeLog("[%p] GameState initialization complete: Client", this);

	for (auto& Callback : ClientInitializationCallbacks) {
		Callback.ExecuteIfBound();
	}
	ClientInitializationCallbacks.Empty();
}

void AVeGameStateBase::NativeOnServerInitializationComplete() {
	VeLog("[%p] GameState initialization complete: Server", this);

	for (auto& Callback : ServerInitializationCallbacks) {
		Callback.ExecuteIfBound();
	}
	ServerInitializationCallbacks.Empty();
}

void AVeGameStateBase::OnReady_ClientInitialization(const FGameStateDynamicDelegate& Callback) {
	if (bClientInitializationComplete) {
		Callback.ExecuteIfBound();
		return;
	}
	ClientInitializationCallbacks.Emplace(Callback);
}

void AVeGameStateBase::OnReady_ServerInitialization(const FGameStateDynamicDelegate& Callback) {
	if (bServerInitializationComplete) {
		Callback.ExecuteIfBound();
		return;
	}
	ServerInitializationCallbacks.Emplace(Callback);
}

#pragma endregion InitializationComplete
//------------------------------------------------------------------------------


#undef LOG_TEST
