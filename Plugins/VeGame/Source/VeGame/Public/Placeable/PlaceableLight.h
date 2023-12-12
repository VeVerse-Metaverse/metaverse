// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlaceableActor.h"
#include "PlaceableLight.generated.h"

class UBoxComponent;
class UPointLightComponent;

UCLASS()
class VEGAME_API APlaceableLight : public APlaceableActor {
	GENERATED_BODY()

	struct FPlaceableLightParams {
		FColor BaseColor = FColor::White;
		float Intensity = 5000.0f;
		float AttenuationRadius = 1000.0f;
	};

public:
	// Sets default values for this actor's properties
	APlaceableLight();

	virtual void PostInitializeComponents() override;
	void OnPlaceableMetadataUpdated(const FVePlaceableMetadata& InMetadata);

	FPlaceableLightParams DefaultLightParams;
	FPlaceableLightParams CurrentLightParams;
	void SetLightParams(FPlaceableLightParams InMaterialParams);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayPriority=0))
	TObjectPtr<UPointLightComponent> PointLightComponent;

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIPlaceableLightPropertiesWidget> PlaceablePropertiesClass;
	
	UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController);
	
	void Properties_OnSave(UUIPlaceableLightPropertiesWidget* PropertiesWidget);
	void Properties_OnReset(UUIPlaceableLightPropertiesWidget* PropertiesWidget);
	void Properties_OnChanged(UUIPlaceableLightPropertiesWidget* PropertiesWidget);

};
