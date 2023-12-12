// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "VeAiEnums.h"
#include "VeAiSimpleFsmState.h"
#include "VeAiSimpleFsmBehaviorComponent.generated.h"

class AAIController;
class UBehaviorTree;
class ACharacter;
class AVeAiSimpleFsmManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UVeAiSimpleFsmBehaviorComponent final : public UActorComponent {
	GENERATED_BODY()

public:
	UVeAiSimpleFsmBehaviorComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief Called when the target actor is perceived.
	 */
	void OnTargetPerceptionUpdated(AActor* Actor, const FAIStimulus& Stimulus);

	/**
	 * @brief Behavior tree used for AI logic.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VeAI")
	UBehaviorTree* BehaviorTreeRef = nullptr;

	/**
	 * @brief Character that this component is attached to.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VeAI")
	ACharacter* CharacterRef = nullptr;

	/**
	 * @brief AI controller that controls the character.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VeAI")
	AAIController* ControllerRef = nullptr;

	/**
	 * @brief The manager that handles the state machine.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VeAI")
	AVeAiSimpleFsmManager* FsmManagerRef = nullptr;

	/**
	 * @brief Current state of the AI.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VeAI")
	FVeAiSimpleFsmState CurrentState;

	/**
	 * @brief Set the controller for this component at initialization (begin play).
	 */
	void InitializeWithController(AAIController* NewController);

	/**
	 * @brief Get the FSM manager of this component.
	 */
	AVeAiSimpleFsmManager* GetFsmManager() const;

	/**
	 * @brief Set the FSM manager for this component at initialization (begin play).
	 */
	void SetFsmManager(AVeAiSimpleFsmManager* NewManager);

	/**
	 * @brief Returns the current state in the blackboard. 
	 */
	FVeAiSimpleFsmState GetState() const;

	/**
	 * @brief Set the current state in the blackboard.
	 */
	void SetState(FVeAiSimpleFsmState NewState);

	/**
	 * @brief Set the current subject in the blackboard.
	 */
	void SetSubject(const FString& Subject) const;

	/**
	 * @brief Set the current action in the blackboard.
	 */
	void SetAction(const FString& Action) const;

	/**
	 * @brief Set the current context in the blackboard.
	 */
	void SetContext(const FString& Context) const;

	/**
	 * @brief Set the current target in the blackboard.
	 */
	void SetTarget(const FString& Target) const;

	/**
	 * @brief Set the current metadata in the blackboard.
	 */
	void SetMetadata(const FString& Metadata) const;

	/**
	 * @brief Set the current emotion in the blackboard.
	 */
	void SetEmotion(const FString& Emotion) const;

	/**
	 * @brief Set the current action in the blackboard.
	 */
	void SetActionType(EVeAiSimpleFsmActionType NewAction) const;

	/**
	 * @brief Set the current subject entity (self) in the blackboard.
	 */
	void SetSubjectEntity(UVeAiSimpleFsmSubjectComponent* NewSubject) const;

	/**
	 * @brief Set the current target entity in the blackboard.
	 */
	void SetTargetEntity(UVeAiSimpleFsmEntityComponent* NewTarget) const;

	/**
	 * @brief Set the current context entity in the blackboard.
	 */
	void SetContextEntity(UVeAiSimpleFsmEntityComponent* NewContext) const;

	/**
	 * @brief Set the current subject actor (self) in the blackboard.
	 */
	void SetSubjectActor(AActor* NewSubject) const;

	/**
	 * @brief Set the current target actor in the blackboard.
	 */
	void SetTargetActor(AActor* NewTarget) const;

	/**
	 * @brief Set the current context actor in the blackboard.
	 */
	void SetContextActor(AActor* NewContext) const;

	/**
	 * @brief Returns the subject set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	FString GetSubject() const;

	/**
	 * @brief Returns the action set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	FString GetAction() const;

	/**
	 * @brief Returns the context set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	FString GetContext() const;

	/**
	 * @brief Returns the target set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	FString GetTarget() const;

	/**
	 * @brief Returns the metadata set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	FString GetMetadata() const;

	/**
	 * @brief Returns the action type set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	EVeAiSimpleFsmActionType GetActionType() const;

	/**
	 * @brief Returns the subject entity set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	UVeAiSimpleFsmSubjectComponent* GetSubjectEntity() const;

	/**
	 * @brief Returns the target entity set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	UVeAiSimpleFsmEntityComponent* GetTargetEntity() const;

	/**
	 * @brief Returns the context entity set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	UVeAiSimpleFsmEntityComponent* GetContextEntity() const;

	/**
	 * @brief Returns the subject actor set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	AActor* GetSubjectActor() const;

	/**
	 * @brief Returns the target actor set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	AActor* GetTargetActor() const;

	/**
	 * @brief Returns the context actor set in the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category="VeAI")
	AActor* GetContextActor() const;
};
