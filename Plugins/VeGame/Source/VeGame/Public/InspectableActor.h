// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#pragma once

#include "PlaceableActor.h"
#include "Inspector/IInspectable.h"
#include "InspectableActor.generated.h"

class UPlaceableComponent;

/** Base framework for any placeable actors. */
UCLASS(Blueprintable, BlueprintType)
class VEGAME_API AInspectableActor : public APlaceableActor, public IInspectable {
	GENERATED_BODY()

public:
	AInspectableActor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inspectable")
	bool bUseCustomForwardVector = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inspectable")
	FVector CustomForwardVector = FVector::ForwardVector;
	
	virtual FVector GetInspectableForwardVector_Implementation() override;

};
