// Author: Egor A. Pristavka

#include "VeWorldSubsystem.h"
#include "Engine/GameEngine.h"
#include "GameFramework/PlayerController.h"

#if WITH_EDITORONLY_DATA
#include "Editor.h"
#endif

UWorld* FVeWorldSubsystem::GetWorld() {
#if WITH_EDITORONLY_DATA

	// if (GIsEditor && GEditor) {
	// 	return GEditor->GetEditorWorldContext().World();
	// }

	// if (GIsEditor && GEngine) {
	// 	TArray<APlayerController*> PlayerList;
	// 	GEngine->GetAllLocalPlayerControllers(PlayerList);
	// 	if (PlayerList.Num()) {
	// 		return PlayerList[0]->GetWorld();
	// 	}
	// }

	if (GIsEditor && GEngine) {
		FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
		return WorldContext->World();
	}

#endif

	if (const auto* GameEngine = Cast<UGameEngine>(GEngine)) {
		if (GameEngine->GetWorldContexts().Num()) {
			return GameEngine->GetWorldContexts()[0].World();
		}
	}

	return nullptr;
}

// UGameInstance* FVeWorldSubsystem::GetGameInstance() {
// 	if (const auto* World = GetWorld()) {
// 		return World->GetGameInstance();
// 	}
// 	return nullptr;
// }

// ENetMode FVeWorldSubsystem::GetNetMode() {
// 	if (const auto* const World = GetWorld()) {
// 		return World->GetNetMode();
// 	}
// 	return NM_Standalone;
// }

// APlayerController* FVeWorldSubsystem::GetLocalPlayerController() {
// 	if (const auto* World = GetWorld()) {
// 		return World->GetFirstPlayerController();
// 	}
// 	return nullptr;
// }

// APawn* FVeWorldSubsystem::GetLocallyControlledPawn() {
// 	if (const auto* PlayerController = GetLocalPlayerController()) {
// 		return PlayerController->GetPawn();
// 	}
// 	return nullptr;
// }

// AGameModeBase* FVeWorldSubsystem::GetLocalGameMode() {
// 	if (const auto* World = GetWorld()) {
// 		return World->GetAuthGameMode();
// 	}
// 	return nullptr;
// }

// AGameStateBase* FVeWorldSubsystem::GetGameState() {
// 	if (const auto* World = GetWorld()) {
// 		return World->GetGameState();
// 	}
// 	return nullptr;
// }

// APlayerState* FVeWorldSubsystem::GetLocalPlayerState() {
// 	if (const auto* PlayerController = GetLocalPlayerController()) {
// 		return PlayerController->PlayerState;
// 	}
// 	return nullptr;
// }
