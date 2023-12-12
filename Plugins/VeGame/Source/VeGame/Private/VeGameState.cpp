// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeGameState.h"

#include "VeShared.h"
#include "VeGameModule.h"
#include "VePakExtraFilesGameStateComponent.h"


AVeGameState::AVeGameState() {
	VePakExtraFileGameStateComponent = CreateDefaultSubobject<UVePakExtraFilesGameStateComponent>(TEXT("VePakExtraFileGameState"));
}

void AVeGameState::BeginInitialization() {
	Super::BeginInitialization();
	
	auto PakFilesInitializationComplete = [=]() {
		ClientPakFilesInitializationComplete();
	};

	if (VePakExtraFileGameStateComponent->IsComplete()) {
		PakFilesInitializationComplete();
	} else {
		VePakExtraFileGameStateComponent->OnComplete.AddWeakLambda(this, PakFilesInitializationComplete);
		VePakExtraFileGameStateComponent->BeginInitialization();
	}
}

void AVeGameState::ClientPakFilesInitializationComplete() {
	bPakFilesInitializationComplete = true;
	ClientInitializationComplete();
}

void AVeGameState::ServerPlaceableInitializationComplete() {
	bPlaceableInitializationComplete = true;
	ServerInitializationComplete();
}

bool AVeGameState::NativeCheckClientInitializationComplete() {
	if (!bPakFilesInitializationComplete) {
		return false;
	}
	return Super::NativeCheckClientInitializationComplete();
}

bool AVeGameState::NativeCheckServerInitializationComplete() {
	// if (!bPlaceableInitializationComplete) {
	// 	return false;
	// }
	return Super::NativeCheckServerInitializationComplete();
}
