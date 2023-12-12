// 

#pragma once

#include "VeAiSimpleFsmAction.generated.h"

/**
 * @brief Supported traits for actions.
 */
UENUM()
enum class EVeAiSimpleFsmActionTrait : uint8 {
	None UMETA(DisplayName="None", ToolTip="No trait"),
	Target UMETA(DisplayName="Target", ToolTip="Target to perform the action on, e.g. another character to say something to"),
	Context UMETA(DisplayName="Context", ToolTip="Context object that is used with the action, e.g. a item to give to some target"),
	Metadata UMETA(DisplayName="Metadata", ToolTip="Metadata for the action, e.g. a message to say to some target")
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FVeAiSimpleFsmAction {
	GENERATED_BODY()

	/**
	 * @brief Name of the action. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	FString Name;

	/**
	 * @brief Description of the action.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI", meta=(MultiLine=true))
	FString Description;

	/**
	 * @brief List of traits supported by the action.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	TSet<EVeAiSimpleFsmActionTrait> Traits;
};
