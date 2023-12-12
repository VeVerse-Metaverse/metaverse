// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CogAiCharacterBase.generated.h"

class AVeAiPlayerState;
struct FVeUserMetadata;

UCLASS()
class VEAI_API ACogAiCharacterBase : public ACharacter {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(ACogAiCharacterBase, FOnAiPlayerStateChangedNative, AVeAiPlayerState* /* PlayerState */);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAiPlayerStateChangedBP, AVeAiPlayerState*, InAiPlayerState);

public:
	ACogAiCharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	/** @brief Called when possessed by the AI controller, inits player state and user metadata. */
	virtual void PossessedBy(AController* NewController) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region AI Player State

	/** @brief The AI player state, stores the AI related settings. */
	UPROPERTY(replicatedUsing=OnRep_AiPlayerState, VisibleAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AVeAiPlayerState> AiPlayerState = nullptr;

	/** @brief Returns the AI player state. */
	AVeAiPlayerState* GetAiPlayerState() const {
		return AiPlayerState;
	}

	/** @brief Returns the AI player state casted to the specified type. */
	template <class T>
	T* GetAiPlayerState() const {
		return Cast<T>(AiPlayerState);
	}

	/** @brief Called when the AI player state is set at server or client. */
	void SetAiPlayerState(AVeAiPlayerState* NewPlayerState);

	/** @brief Called when the client receives the player state. */
	UFUNCTION()
	void OnRep_AiPlayerState();

	/** @brief Called when the pawn receives the player state. Notifies other components or actors in code. */
	FOnAiPlayerStateChangedNative OnAiPlayerStateChangedNative;

	/** @brief Called when the pawn receives the player state. Notifies other blueprints. */
	UPROPERTY(BlueprintAssignable, Category="AI")
	FOnAiPlayerStateChangedBP OnAiPlayerStateChangedBPDelegate;

	/** @brief Called when the pawn receives the player state. Notifies child blueprints. */
	UFUNCTION(BlueprintImplementableEvent, Category="AI")
	void OnAiPlayerStateChangedBP(AVeAiPlayerState* InAiPlayerState);

	/** @brief Called when the pawn receives the player state. Triggers events. */
	virtual void OnAiPlayerStateChange();

	/** @brief Called when the pawn receives the user metadata from the AI player state. */
	UFUNCTION(BlueprintNativeEvent, Category="AI")
	void OnAiUserMetadataUpdated(const FVeUserMetadata& InUserMetadata);

	/** @brief User id, used to identify the AI character, including the persona avatar mesh. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FGuid UserId = {};

#pragma endregion

};
