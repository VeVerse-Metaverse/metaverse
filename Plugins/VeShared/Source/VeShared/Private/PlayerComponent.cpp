// 

#include "PlayerComponent.h"

#include "GameFramework/PlayerController.h"


// Sets default values for this component's properties
UPlayerComponent::UPlayerComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPlayerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	SetPlayerController(nullptr);
	Super::EndPlay(EndPlayReason);
}

void UPlayerComponent::SetPlayerController(APlayerController* NewPlayerController) {
	if (MyPlayerController == NewPlayerController) {
		return;
	}

	auto* OldPlayerController = MyPlayerController.Get();
	MyPlayerController = NewPlayerController;

	NativeOnPlayerControllerChanged(NewPlayerController, OldPlayerController);
}

void UPlayerComponent::NativeOnPlayerControllerChanged(APlayerController* NewPlayerController, APlayerController* OldPlayerController) {
	OnPlayerControllerChanged.Broadcast(NewPlayerController, OldPlayerController);
}

APawn* UPlayerComponent::GetPawn() const {
	if (auto* Owner = GetOwner(); IsValid(Owner)) {
		if (auto* Pawn = Cast<APawn>(Owner); IsValid(Pawn)) {
			return Pawn;
		}

		if (const auto* PlayerController = Cast<APlayerController>(Owner); IsValid(PlayerController)) {
			return PlayerController->GetPawn();
		}
	}

	return nullptr;
}

AController* UPlayerComponent::GetController() const {
	if (auto* Owner = GetOwner(); IsValid(Owner)) {
		if (auto* PlayerController = Cast<AController>(Owner); IsValid(PlayerController)) {
			return PlayerController;
		}

		if (const auto* Pawn = Cast<APawn>(Owner); IsValid(Pawn)) {
			return Pawn->GetController();
		}
	}

	return nullptr;
}

ENetRole UPlayerComponent::GetLocalRole() const {
	if (const auto* Owner = GetOwner(); IsValid(Owner)) {
		return Owner->GetLocalRole();
	}
	return ROLE_None;
}

bool UPlayerComponent::HasPlayer() const {
	if (const auto* Owner = GetOwner(); IsValid(Owner)) {
		return static_cast<bool>(Cast<APawn>(Owner)) || static_cast<bool>(Cast<APlayerController>(Owner));
	}
	return false;
}

bool UPlayerComponent::IsAttachedToPawn() const {
	return static_cast<bool>(GetOwner<APawn>());
}

bool UPlayerComponent::IsAttachedToPlayerController() const {
	return static_cast<bool>(GetOwner<APlayerController>());
}

bool UPlayerComponent::IsLocallyControlled() const {
	if (const auto* PlayerController = GetPlayerController(); IsValid(PlayerController)) {
		return PlayerController->IsLocalController();
	}

	if (const auto* Pawn = GetPawn(); IsValid(Pawn)) {
		return Pawn->IsLocallyControlled();
	}

	return false;
}

bool UPlayerComponent::HasAuthority() const {
	if (const auto* Owner = GetOwner(); IsValid(Owner)) {
		return Owner->HasAuthority();
	}
	return false;
}

FTimerManager* UPlayerComponent::GetWorldTimerManager() const {
	if (const auto* World = GetWorld()) {
		return &World->GetTimerManager();
	}
	return nullptr;
}
