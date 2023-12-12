// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VeCharacterBase.generated.h"

UCLASS()
class VEGAME_API AVeCharacterBase : public ACharacter {
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FPlayerStateDelegate, APlayerState*, InPlayerState);

	DECLARE_EVENT_OneParam(AInteractiveCharacter, FOnPlayerStateChanged, APlayerState*);

public:
	AVeCharacterBase();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Metaverse")
	TSubclassOf<UUserWidget> HUDWidgetClass;

protected:
	void OnVivoxVoiceStateChangedCallback(const FGuid& UserId, bool bIsSpeaking);

#pragma region PlayerState

public:
	UFUNCTION(BlueprintCallable, Category="Metaverse")
	void OnReady_PlayerState(FPlayerStateDelegate Callback);

	FOnPlayerStateChanged OnPlayerStateChanged;

protected:
	virtual void NativeOnPlayerStateChanged(APlayerState* InPlayerState);

#pragma endregion PlayerState
};
