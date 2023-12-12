// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "VRKeyboard.generated.h"

UCLASS()
class VETEST_API AVRKeyboard : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVRKeyboard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* KeyboardMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* KeyboardWidgetComponent;

};
