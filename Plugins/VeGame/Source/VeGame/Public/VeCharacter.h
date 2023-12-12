// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ICallbackInterface.h"
#include "Components/InteractionObjectComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "GameFramework/Character.h"
#include "Pawns/InteractiveCharacter.h"
#include "Sound/SampleBufferIO.h"
#include "VeCharacter.generated.h"

class UCogAiPerceptibleComponent;
class UAIPerceptionComponent;
class UAIPerceptionStimuliSourceComponent;

UCLASS()
class VEGAME_API AVeCharacter : public AInteractiveCharacter, public ICallbackInterface {
	GENERATED_BODY()

public:
	AVeCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void NativeOnPlayerStateChanged(APlayerState* InPlayerState) override;

	/** User information widget displayed over remote pawns. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UUserInfoWidgetComponent* UserInfoWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Metaverse")
	TSubclassOf<UUserWidget> HUDWidgetClass;

protected:
	void OnVivoxVoiceStateChangedCallback(const FGuid& UserId, bool bIsSpeaking);

#pragma region AI

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSourceComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UCogAiPerceptibleComponent> CogAiPerceptibleComponent = nullptr;

#pragma endregion
};
