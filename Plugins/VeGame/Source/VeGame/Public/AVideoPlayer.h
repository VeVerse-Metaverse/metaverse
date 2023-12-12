// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.



#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AVideoPlayer.generated.h"

UCLASS()
class VEGAME_API AAVideoPlayer : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAVideoPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
