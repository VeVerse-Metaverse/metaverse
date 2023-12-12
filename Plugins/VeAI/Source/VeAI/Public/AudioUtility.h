// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioUtility.generated.h"

UCLASS()
class VEAI_API AAudioUtility : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAudioUtility();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
