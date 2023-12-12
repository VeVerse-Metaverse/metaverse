// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.



#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VeGameModeBase.generated.h"

UCLASS()
class VEGAMEFRAMEWORK_API AVeGameModeBase : public AGameModeBase {
	GENERATED_BODY()

	DECLARE_EVENT_TwoParams(AVeGameMode, OnPlayerControllerEvent, APlayerController* InPlayerController, const FGuid& UserId);

public:
	AVeGameModeBase();

	OnPlayerControllerEvent OnUserLogin;
	OnPlayerControllerEvent OnUserLogout;

protected:
	virtual void NativeOnUserLogin(APlayerController* InPlayerController, const FGuid& UserId);
	virtual void NativeOnUserLogout(APlayerController* InPlayerController, const FGuid& UserId);

};
