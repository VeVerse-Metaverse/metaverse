#pragma once

#include "CoreMinimal.h"
#include "VeAiSimpleFsmEntityComponent.h"
#include "VeAiSimpleFsmLocationComponent.generated.h"

/**
 * @brief Represents the AI subject used to categorize and describe the AI actors. This component is used to add the AI subject features to the actor.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UVeAiSimpleFsmLocationComponent : public UVeAiSimpleFsmEntityComponent {
	GENERATED_BODY()

public:
	UVeAiSimpleFsmLocationComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief List of linked locations.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<struct FVeAiSimpleFsmLocationLink> LinkedLocations;
};
