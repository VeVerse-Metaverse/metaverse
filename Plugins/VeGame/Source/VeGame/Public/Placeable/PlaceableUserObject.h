// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "PlaceableActor.h"
#include "PlaceableUserObject.generated.h"

enum class EPlaceableChangeType : uint8;

UCLASS(Blueprintable, BlueprintType)
class VEGAME_API APlaceableUserObject : public APlaceableActor {
	GENERATED_BODY()

public:
	APlaceableUserObject();

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<UUIPlaceablePropertiesWidget> PlaceablePropertiesClass;

	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;

protected:
	virtual void PostInitializeComponents() override;
	void OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange);

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;

};
