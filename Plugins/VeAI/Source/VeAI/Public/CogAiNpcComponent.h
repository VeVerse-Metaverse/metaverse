// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2CogAiRequest.h"
#include "CogAiController.h"
#include "CogAiMessage.h"
#include "CogAiPerceptibleComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "CogAiNpcComponent.generated.h"

class UCogAiPersonalityDataAsset;
class UCogAiPersonalityDefinitionDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionTakenSignature, UCogAiActionMessage*, ActionMessage);

/** @brief CogAI NPC component. Used to request decisions from the CogAI. Attached to the AI Controller. Uses the NPC settings component attached to the pawn. */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UCogAiNpcComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UCogAiNpcComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * @brief Called when the perception component in AI Controller is updated.
	 * @param Actors 
	 */
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& Actors);

	/** @brief Checks if the component's owner has network authority. Usually must be true as AI runs on server. */
	UFUNCTION(BlueprintCallable)
	bool HasAuthority() const;

public:
#pragma region AI

	/** @brief Registers the component with the AI controller, caching the references to the controller, perception component and blackboard. */
	void InitializeWithController(ACogAiController* CogAiController);

	/** @brief Registers the component with the pawn, caching the references to the NPC settings component. */
	void InitializeWithPawn(APawn* Pawn);

	/** @brief AI controller that controls the character. */
	UPROPERTY(BlueprintReadOnly, Category="AI")
	AAIController* ControllerRef = nullptr;

	/** @brief Reference to the perception component. Received from the AI controller and cached here. */
	UPROPERTY(BlueprintReadOnly, Category="AI")
	UAIPerceptionComponent* PerceptionComponentRef = nullptr;

	/** @brief Reference to the NPC settings component. Received from the AI controller's pawn and cached here. */
	UPROPERTY(BlueprintReadOnly, Category="AI")
	UCogAiNpcSettingsComponent* NpcSettingsComponentRef = nullptr;

	/** @brief Blackboard used for AI logic. Received from the AI controller and cached here. */
	UPROPERTY(BlueprintReadOnly, Category="AI")
	UBlackboardComponent* BlackboardRef = nullptr;

	/** @brief Behavior tree used for AI logic. Must be set in the editor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	UBehaviorTree* BehaviorTreeRef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bEnableSight = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bEnableHearing = true;

	FString LastMessage;

	/** @brief Other actor who currently speaks so we won't interrupt. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<AActor> Speaker = nullptr;

	/** @brief Max time to keep the pending say message before it becomes unimportant. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	float PendingSayTimeout = 10.0f;

	/** @brief Messages that the character is going to say after previous speaker finishes, and timestamp when the message has been added. */
	TArray<TPair<UCogAiActionMessage*, float>> PendingSayActions;

#pragma endregion

#pragma region Debug
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI Debug")
	FString DebugAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI Debug")
	TMap<FString, FString> DebugActionParameters;
#pragma endregion

#pragma region Actions

	/**
	 * @brief Makes the character take the specified action.
	 * @param ActionMessage Action message received from the CogAI backend or created manually.
	 */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void TakeAction(UCogAiActionMessage* ActionMessage);

	/** @brief Event called when the character takes an action. */
	UPROPERTY(BlueprintAssignable, Category = "AI")
	FOnActionTakenSignature OnActionTaken;

	/** @brief Set the target that the character is currently perceiving. */
	AActor* SetPerceptibleTarget(const FString& Name) const;

	/** @brief Set the target that the character is currently perceiving. */
	void SetPerceptibleTarget(AActor* Actor) const;

#pragma endregion

#pragma region Perception

	/** @brief The maximum number of perceptions to keep in the buffer. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Perception")
	int32 MaxPerceptionsToKeep = 10;

	/** @brief Accumulates the perceptions, reset when perceptions are sent to the CogAI backend. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Perception")
	TArray<FCogAiPerception> PendingPerceptions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Perception")
	TArray<FCogAiPerception> ProcessedPerceptions;

	/** @brief Updates the perception of the character by queuing the current state of the world to the CogAI backend. */
	void UpdatePerception(const FCogAiPerception& Perception);

	UFUNCTION(BlueprintCallable)
	void ForceUpdateContext();

#pragma endregion

#pragma region Messages

	/** @brief The delay to process the recent perceptions after receiving at least one (for perception accumulation). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta=(ClampMin=1.0, ClampMax=10.0, UIMin=1.0, UIMax=10.0))
	float PerceptionAccumulationTime = 1.0f;
	FTimerDelegate PerceptionTimerDelegate;
	FTimerHandle PerceptionTimerHandle;

	/** @brief Max time before the character sends the idle request to the CogAI backend. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta=(ClampMin=10.0, ClampMax=60.0, UIMin=10.0, UIMax=60.0))
	float IdleTimerDelay = 30.0f;
	FTimerDelegate IdleTimerDelegate;
	FTimerHandle IdleTimerHandle;

	/** @brief Allowing the character to process the queue of messages. Enable in the blueprint to start processing the queue. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bEnableProcessing = false;

	/** @brief Allowing the character to process the queue of messages. Enable in the blueprint to start processing the queue. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bEnableIdleMessages = false;

	/**
	 * @brief Sends the request to the CogAI backend. 
	 * @param Metadata 
	 */
	void SendMessage(const FApi2CogAiRequestMetadata& Metadata);

	/** @brief Adds the history of decisions and perceived consequences made by the character to the metadata. */
	void AddMessageHistoryToMetadata(FApi2CogAiRequestMetadata& Metadata, int InMaxHistorySize);

	/** @brief Called when the response is received from the CogAI. */
	void OnMessageReceived(const FApi2CogAiResponseMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError);

	/** @brief Sends the initial request to the CogAI backend including the context information and character's initial state. */
	void SendInitialRequest();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bSentInitialRequest = false;

	/** @brief Sends the history of decisions made by the character to the CogAI backend to get the next decision. */
	void SendIdleMessage();

	/** @brief Sends the perception message along with previous history of decisions made. */
	void SendPerceptionMessage();

	/** @brief Resets the idle timer to fire the next message when delay is over. */
	void ScheduleIdleRequest();

	/** @brief Resets the perception timer to fire the next message when delay is over. */
	void SchedulePerceptionRequest();

	/** @brief Called after the perception timer fires to clear the timer until the next perception update. */
	void ResetPerceptionSchedule();

	/** @brief Is the character busy communicating with backend and taking decisions? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bBusy = false;

	/** @brief Stores the message in the history. */
	bool StoreMessage(UCogAiMessage* Message);

	/** @brief History of messages generated by the system or received from the CogAI backend. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<UCogAiMessage*> MessageHistory;

	/** @brief Maximum number of messages to keep in the history. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta=(ClampMin=5, ClampMax=100, UIMin=5, UIMax=100))
	int MaxHistorySize = 100;

	/** @brief Maximum number of messages in the history to process by sending them to the CogAI backend when idle. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta=(ClampMin=5, ClampMax=15, UIMin=5, UIMax=15))
	int MaxHistorySizeToProcess = 10;

private:
	/** @brief Static message used with every request to provide supported actions list. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCogAiActionsSystemMessage> ActionsSystemMessage = nullptr;

	/** @brief Static message used with every request to provide context information. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCogAiContextSystemMessage> ContextSystemMessage = nullptr;

	/** @brief Static message used with every request to provide character's information. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCogAiWhoamiSystemMessage> WhoamiSystemMessage = nullptr;

#pragma endregion

#pragma region Helpers

public:
	/** @brief Finds the closest context component in the world. */
	class UCogAiContextComponent* FindClosestContextInWorld() const;

	/**
	 * @brief Finds the first perceptible component in the world by its name.
	 * @param Name Name of the perceptible component to find.
	 */
	UCogAiPerceptibleComponent* FindPerceptibleInWorld(const FString& Name) const;

	/**
	 * @brief Finds an accessible location in the world.
	 * @return Random location in the world accessible by the character.
	 */
	FVector FindRandomLocationInWorld() const;

	/**
	 * @brief Debug function to save the message history to a file.
	 * @param FilePath Path to the file to save the message history to, .txt extension is added automatically.
	 */
	UFUNCTION(BlueprintCallable, Category = "AI Debug")
	void SaveMessageHistoryToFile(const FString& FilePath) const;

#pragma endregion

#pragma region Context
	void UpdateWhoAmI();

	void UpdateContext();

	void UpdateActions();
#pragma endregion

};
