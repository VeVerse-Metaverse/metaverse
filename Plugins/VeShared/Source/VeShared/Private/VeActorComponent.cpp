// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeActorComponent.h"
#include "VeShared.h"


// Sets default values for this component's properties
UVeActorComponent::UVeActorComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

bool UVeActorComponent::IsLocallyControlled() const {
	if (AActor* Owner = GetOwner()) {
		// Try to cast to a controller.
		if (const AController* OwnerController = Cast<AController>(Owner)) {
			return OwnerController->IsLocalController();
		}

		// Try to cast to a pawn. 
		if (const APawn* OwnerPawn = Cast<APawn>(Owner)) {
			return OwnerPawn->IsLocallyControlled();
		}

		LogErrorF("owner is not pawn or controller");
		return false;
	}

	LogErrorF("failed to get owner actor");
	return false;
}

UGameInstance* UVeActorComponent::GetGameInstance() const {
	if (const UWorld* const World = GetWorld()) {
		return World->GetGameInstance();
	}

	LogErrorF("failed to get world");
	return nullptr;
}
