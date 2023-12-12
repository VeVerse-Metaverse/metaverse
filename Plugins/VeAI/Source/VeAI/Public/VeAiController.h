// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "VeAiController.generated.h"

class AVeAiPlayerState;

/**
 * @brief Controller for an AI player.
 */
UCLASS()
class VEAI_API AVeAiController : public AAIController {
	GENERATED_BODY()

protected:
	/**
	 * @brief Destroy the AI player state and the pawn.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * @brief Called after the controller has been initialized during gameplay, creates the AI player state. 
	 */
	virtual void PostInitializeComponents() override;

	/**
	 * @brief Class of the player state to spawn for the AI.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Controller)
	TSubclassOf<AVeAiPlayerState> AiStateClass;

	/**
	 * @brief Ai player state, containing replicated information about the AI player using this controller.
	 */
	UPROPERTY(BlueprintReadOnly, Category=Controller)
	TObjectPtr<AVeAiPlayerState> AiPlayerState;

public:
	/**
	 * @brief Get the player state of the AI.
	 */
	AVeAiPlayerState* GetAiPlayerState() const;

	/**
	 * @brief Get the player state of the AI as a specific type.
	 */
	template <typename T>
	T* GetAiPlayerState() const {
		return Cast<T>(GetAiPlayerState());
	}

	/**
	 * @brief Get the user id of the AI.
	 */
	FGuid GetUserId() const;

	/**
	 * @brief Set the user id of the AI and update the player state with the user metadata.
	 */
	void SetUserId(const FGuid& InUserId);

	/**
	 * @brief Mark the AI as speaking.
	 */
	UFUNCTION(BlueprintCallable)
	void StartSpeaking();

	/**
	 * @brief Mark the AI as not speaking.
	 */
	UFUNCTION(BlueprintCallable)
	void StopSpeaking();

	/** @brief On action taken. */
	UFUNCTION(BlueprintNativeEvent, Category="AI")
	void OnActionTaken(UCogAiActionMessage* ActionMessage);

};
