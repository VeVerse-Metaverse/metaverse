// Author: Egor A. Pristavka

#include "VeNetworkSubsystem.h"

#include "Engine/GameEngine.h"

bool FVeNetworkSubsystem::IsStandalone(bool& bOutResult) {
	bOutResult = false;

	if (!GEngine) {
		return false;
	}

	if (const UGameEngine* const GameEngine = Cast<UGameEngine>(GEngine)) {
		if (const UWorld* const World = GameEngine->GetWorld()) {
			if (World->GetNetMode() == NM_Standalone) {
				bOutResult = true;
				return true;
			}
		}
	}

	return false;
}

bool FVeNetworkSubsystem::IsServer(bool& bOutResult) {
	bOutResult = false;

	if (!GEngine) {
		return false;
	}

	if (const UGameEngine* const GameEngine = Cast<UGameEngine>(GEngine)) {
		if (const UWorld* const World = GameEngine->GetWorld()) {
			if (World->IsNetMode(NM_DedicatedServer) || World->IsNetMode(NM_ListenServer)) {
				bOutResult = true;
				return true;
			}
		}
	}

	return false;
}

bool FVeNetworkSubsystem::IsClient(bool& bOutResult) {
	bOutResult = false;

	if (!GEngine) {
		return false;
	}

	if (const UGameEngine* const GameEngine = Cast<UGameEngine>(GEngine)) {
		if (const UWorld* const World = GameEngine->GetWorld()) {
			if (World->IsNetMode(NM_Client)) {
				bOutResult = true;
				return true;
			}
		}
	}

	return false;
}
