// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "PlaceableActor.h"
#include "PlaceableShape.generated.h"

/**
 * 
 */
UCLASS()
class VEGAME_API APlaceableShape : public APlaceableActor {
	GENERATED_BODY()

	struct FPlaceableShapeMaterialParams {
		FColor BaseColor = FColor::White;
		float Metallic = 0.0f;
		float Roughness = 0.5f;
	};

public:
	APlaceableShape();

	virtual void PostInitializeComponents() override;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent;

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIPlaceableShapePropertiesWidget> PlaceablePropertiesClass;

	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;

protected:
	void Properties_OnSave(class UUIPlaceableShapePropertiesWidget* PropertiesWidget);
	void Properties_OnReset(class UUIPlaceableShapePropertiesWidget* PropertiesWidget);
	void Properties_OnChanged(class UUIPlaceableShapePropertiesWidget* PropertiesWidget);

	void OnPlaceableMetadataUpdated(const FVePlaceableMetadata& InMetadata);

	FPlaceableShapeMaterialParams DefaultMaterialParams;
	FPlaceableShapeMaterialParams CurrentMaterialParams;
	void SetMaterialparams(FPlaceableShapeMaterialParams InMaterialParams);

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> DynMaterial;

};
