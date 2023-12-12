#pragma once

#include "Api2AiSimpleFsmRequest.h"
#include "VeAiEnums.h"
#include "VeAiSimpleFsmState.generated.h"

class UVeAiSimpleFsmEntityComponent;
class UVeAiSimpleFsmSubjectComponent;

/**
 * @brief AI Simple Finite State Machine state.
 */
USTRUCT(BlueprintType)
struct VEAI_API FVeAiSimpleFsmState {
	GENERATED_BODY()

	/**
	 * @brief Equality operator.
	 */
	bool operator==(const FVeAiSimpleFsmState& Other) const;

	/**
	 * @brief Subject ID that should perform the action.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAiSimpleFsmState")
	FString NPC = {};

	/**
	 * @brief Action to perform.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAiSimpleFsmState")
	FString Action = {};

	/**
	 * @brief Target ID of the action.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAiSimpleFsmState")
	FString Target = {};

	/**
	 * @brief Context object ID to perform the action with.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAiSimpleFsmState")
	FString Context = {};

	/**
	 * @brief Emotion to use for the action.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAiSimpleFsmState")
	FString Emotion = {};

	/**
	 * @brief Metadata for the action.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAiSimpleFsmState")
	FString Metadata = {};

	/**
	 * @brief The type of action to perform.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAiSimpleFsmState")
	EVeAiSimpleFsmActionType ActionType = EVeAiSimpleFsmActionType::None;

	/**
	 * @brief Actor that should perform the action.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAiSimpleFsmState")
	UVeAiSimpleFsmSubjectComponent* SubjectEntity = nullptr;

	/**
	 * @brief Actor that should be the target of the action.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAiSimpleFsmState")
	UVeAiSimpleFsmEntityComponent* TargetEntity = nullptr;

	/**
	 * @brief Actor that should be the context of the action.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAiSimpleFsmState")
	UVeAiSimpleFsmEntityComponent* ContextEntity = nullptr;

	/**
	 * @brief Convert from an API state metadata object.
	 */
	void FromApiStateMetadata(const FApi2AiSimpleFsmStateMetadata& InApiStateMetadata);
};
