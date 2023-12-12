// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CogPerceptible.h"
#include "Components/ActorComponent.h"
#include "CogAiPerceptibleComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UCogAiPerceptibleComponent : public UActorComponent, public ICogPerceptible {
	GENERATED_BODY()

public:
	UCogAiPerceptibleComponent();

	void SetName(const FString& Name);
	void SetDescription(const FString& Description);
	void SetInspectDescription(const FString& InspectDescription);

	virtual FString GetName_Implementation() override;
	virtual FString GetDescription_Implementation() override;
	virtual FString GetInspectDescription_Implementation() override;

	/**
	 * @brief Perceptible properties of the component.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TMap<FString, FString> PerceptibleProperties;
};
