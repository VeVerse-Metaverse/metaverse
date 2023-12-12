// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CogAiPerceptibleActor.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UCogAiPerceptibleComponent;

UCLASS(BlueprintType, Blueprintable)
class VEAI_API ACogAiPerceptibleActor : public AActor {
	GENERATED_BODY()

public:
	ACogAiPerceptibleActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UCogAiPerceptibleComponent> CogAiPerceptibleComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSourceComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bAlertOnPerceptiblePropertyChange = true;

	UFUNCTION(BlueprintCallable, Category="AI")
	void SetPerceptibleProperty(const FString& InName, const FString& InValue) ;

	UFUNCTION(BlueprintCallable, Category="AI")
	void Alert(const FString& InValue);

	UFUNCTION(BlueprintCallable, Category="AI")
	FString GetPerceptibleProperty(const FString& InName) const;
};
