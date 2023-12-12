// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeGameModeBase.h"


AVeGameModeBase::AVeGameModeBase() {
	PrimaryActorTick.bCanEverTick = true;
}

void AVeGameModeBase::NativeOnUserLogin(APlayerController* InPlayerController, const FGuid& UserId) {
	OnUserLogin.Broadcast(InPlayerController, UserId);
}

void AVeGameModeBase::NativeOnUserLogout(APlayerController* InPlayerController, const FGuid& UserId) {
	OnUserLogout.Broadcast(InPlayerController, UserId);
}
