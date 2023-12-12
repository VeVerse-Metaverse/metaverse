// 

#pragma once

#include "CoreMinimal.h"
#include "VeAiCharacter.h"
#include "VeAiSimpleFsmCharacter.generated.h"

class UVeAiSimpleFsmSubjectComponent;

UCLASS(Blueprintable, BlueprintType)
class VEAI_API AVeAiSimpleFsmCharacter : public AVeAiCharacter {
	GENERATED_BODY()

public:
	AVeAiSimpleFsmCharacter(const FObjectInitializer& ObjectInitializer);

	/**
	 * @brief Subscribes for the state change to update the name and description.
	 */
	virtual void OnAiPlayerStateChange() override;

	/**
	 * @brief The AI subject entity component.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UVeAiSimpleFsmSubjectComponent> SubjectComponent = nullptr;
};
