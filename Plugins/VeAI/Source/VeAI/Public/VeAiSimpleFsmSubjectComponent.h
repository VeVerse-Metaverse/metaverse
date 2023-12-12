// 

#pragma once

#include "CoreMinimal.h"
#include "VeAiSimpleFsmEntityComponent.h"
#include "VeAiSimpleFsmSubjectComponent.generated.h"

/**
 * @brief Represents the AI subject used to categorize and describe the AI actors. This component is used to add the AI subject features to the actor.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UVeAiSimpleFsmSubjectComponent : public UVeAiSimpleFsmEntityComponent {
	GENERATED_BODY()

public:
	UVeAiSimpleFsmSubjectComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief Returns the behavior component of the AI entity.
	 */
	UFUNCTION()
	UVeAiSimpleFsmBehaviorComponent* GetBehaviorComponent() const;

	/**
	 * @brief Sex of the AI subject.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EVeAiSimpleFsmCharacterSex Sex = EVeAiSimpleFsmCharacterSex::None;

	/**
	 * @brief Voice of the AI subject.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	ECogAiVoice Voice = ECogAiVoice::MaleA;

	virtual FString GetId() const override;
	virtual FString GetDescription() const override;
};
