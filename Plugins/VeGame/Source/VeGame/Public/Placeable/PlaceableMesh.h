// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "InspectableActor.h"
#include "PlaceableMesh.generated.h"

class UUIPlaceablePropertiesWidget;

UCLASS()
class VEGAME_API APlaceableMesh : public AInspectableActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlaceableMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIPlaceableMeshPropertiesWidget> PlaceablePropertiesClass;

	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;

};
