// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#pragma once

#include "Actors/InteractiveActor.h"
#include "IPlaceable.h"
#include "PlaceableActor.generated.h"

class UPlaceableComponent;

/** Base framework for any placeable actors. */
UCLASS(Blueprintable, BlueprintType)
class VEGAME_API APlaceableActor : public AInteractiveActor, public IPlaceable {
	GENERATED_BODY()

public:
	APlaceableActor();

	virtual void PreInitializeComponents() override;

	/** Return placeable metadata. */
	UFUNCTION(BlueprintPure, Category="Placeable")
	const FVePlaceableMetadata& GetPlaceableMetadata() const;

	/** Placeable component to handle placeable. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UPlaceableComponent> PlaceableComponent;

protected:
	/* Update placeable actor on server. */
	UFUNCTION(BlueprintCallable, Category="Placeable")
	void UpdatePlaceableOnServer();

	UFUNCTION(BlueprintCallable, Category="Placeable")
	void SetPlaceableState(EPlaceableState State, float Ratio = 0.0f);
	
};
