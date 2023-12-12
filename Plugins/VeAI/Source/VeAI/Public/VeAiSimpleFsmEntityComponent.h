// 

#pragma once

#include "CoreMinimal.h"
#include "VeAiEnums.h"
#include "Components/ActorComponent.h"
#include "VeAiSimpleFsmEntityComponent.generated.h"

class UVeAiSimpleFsmBehaviorComponent;

/**
 * @brief Represents the AI entity used to categorize and describe the AI actors. This component is used to add the AI entity to the actor.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UVeAiSimpleFsmEntityComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UVeAiSimpleFsmEntityComponent();

	/**
	 * @brief The type of the AI entity.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EVeAiSimpleFsmEntityType Type = EVeAiSimpleFsmEntityType::Object;

	/**
	 * @brief ID of the AI entity.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FString Id = {};

	void SetId(const FString& InId) {
		this->Id = InId;
	}

	void SetDescription(const FString& InDescription) {
		this->Description = InDescription;
	}

	/**
	 * @brief Description of the AI entity.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FString Description = {};

	/**
	 * @brief Returns the ID of the AI entity.
	 */
	virtual FString GetId() const {
		return Id;
	}

	/**
	 * @brief Returns the description of the AI entity.
	 */
	virtual FString GetDescription() const {
		return Description;
	}
};
