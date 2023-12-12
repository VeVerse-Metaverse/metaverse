// 

#pragma once

#include "VeAiSimpleFsmAction.h"
#include "GameFramework/Actor.h"
#include "VeAiSimpleFsmSubjectComponent.h"
#include "VeAiSimpleFsmState.h"
#include "VeAiSimpleFsmManager.generated.h"

class UVeAiSimpleFsmEntityComponent;
class FApi2AiSimpleFsmMetadata;
enum class EApi2ResponseCode : uint32;

/**
 * @brief This class is used to control the AI play. It requests a list of actions from the API and executes them.
 */
UCLASS()
class VEAI_API AVeAiSimpleFsmManager : public AActor {
	GENERATED_BODY()

	/**
	 * @brief Callback for the play script request.
	 */
	void OnSimpleFsmStatesRequestCompleted(const FApi2AiSimpleFsmMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InErrorMessage);

	/**
	 * @brief Requests the list of actions from the API.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	void RequestStates();

public:
	// Sets default values for this actor's properties
	AVeAiSimpleFsmManager();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStatesRequestCompleted();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStatesRequestFailed(const FString& InErrorMessage);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * @brief Returns the entity with the given ID.
	 * @param Id The ID of the entity.
	 * @param bFallbackToAny If true, returns a random entity if the ID is not found.
	 */
	UVeAiSimpleFsmEntityComponent* GetEntity(const FString& Id, bool bFallbackToAny = true);

	/**
	 * @brief Returns the subject with the given ID.
	 * @param Id The ID of the entity.
	 * @param bFallbackToAny If true, returns a random entity if the ID is not found.
	 */
	UVeAiSimpleFsmSubjectComponent* GetSubject(const FString& Id, bool bFallbackToAny = false);

	/**
	 * @brief Returns the object with the given ID.
	 * @param Id The ID of the entity.
	 * @param bFallbackToAny If true, returns a random entity if the ID is not found.
	 */
	UVeAiSimpleFsmEntityComponent* GetObject(const FString& Id, bool bFallbackToAny = true);

	/**
	 * @brief Desired number of states to request.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VeAI")
	int32 DesiredStateNum = 10;

	/**
	 * @brief List of supported actions. Not used in V2.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VeAI")
	TArray<FVeAiSimpleFsmAction> SupportedActions = {
		{
			TEXT("say"),
			TEXT("The Subject can say the metadata phrase to another Subject or to self when it doesn't want to say it to particular Subject. Phrases said by Subjects should accord with the environment and the situation."),
			{
				EVeAiSimpleFsmActionTrait::Target,
				EVeAiSimpleFsmActionTrait::Metadata
			}
		}
	};

	/**
	 * @brief Description of the context for the FSM states request.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VeAI", meta=(MultiLine=true))
	FString ContextDescription = TEXT("The script is a theatre scene play in its middle act, it has no logical beginning and end and should be entertaining to the audience.");

	/**
	 * @brief Cached list of all the known entities.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VeAI")
	TArray<UVeAiSimpleFsmEntityComponent*> KnownEntities;

	/**
	 * @brief Cached list of all the known locations.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VeAI")
	TArray<UVeAiSimpleFsmEntityComponent*> KnownLocations;

	/**
	 * @brief Cached list of all the known subjects.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VeAI")
	TArray<UVeAiSimpleFsmSubjectComponent*> KnownSubjects;

	/**
	 * @brief Cached list of all the known players.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VeAI")
	TArray<UVeAiSimpleFsmEntityComponent*> KnownPlayers;

	/**
	 * @brief Cached list of all the known objects.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VeAI")
	TArray<UVeAiSimpleFsmEntityComponent*> KnownObjects;

	/**
	 * @brief Stack of current states. Received from the API.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VeAI")
	TArray<FVeAiSimpleFsmState> States;

	/**
	 * @brief If true, the FSM will not stop after the last state has been executed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VeAI")
	bool bUnlimited;

	/**
	 * @brief Called by a subject when it has completed a state.
	 */
	void OnStateCompleted();

	/**
	 * @brief Executes the next state.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	void ExecuteNextState();

	/**
	 * @brief Executes a state.
	 */
	void ExecuteState(const FVeAiSimpleFsmState& State);
};
