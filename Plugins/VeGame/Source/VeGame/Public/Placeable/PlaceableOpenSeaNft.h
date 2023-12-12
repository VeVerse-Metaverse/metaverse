// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "InspectableActor.h"
#include "PlaceableComponent.h"
#include "ApiOpenSeaMetadata.h"
#include "PlaceableOpenSeaNft.generated.h"

/** Base class for placeable actors which have files. */
UCLASS(Blueprintable, BlueprintType)
class VEGAME_API APlaceableOpenSeaNft : public AInspectableActor {
	GENERATED_BODY()

public:
	APlaceableOpenSeaNft();

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIPlaceablePropertiesWidget> PlaceablePropertiesClass;

	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;

	/** Class to use when creating a inspect properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inspectable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIInspectorImagePreviewWidget> InspectableImagePreviewClass;

	/** Class to use when creating a inspect properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inspectable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIInspectorModelPreviewWidget> InspectableModelPreviewClass;

	virtual UUserWidget* GetInspectPreviewWidget_Implementation(APlayerController* PlayerController) override;

	/** Class to use when creating a inspect properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inspectable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIInspectorOpenSeaAssetPropertiesWidget> InspectablePropertiesClass;

	virtual UUserWidget* GetInspectPropertiesWidget_Implementation(APlayerController* PlayerController) override;

protected:
	virtual void PostInitializeComponents() override;

	FApiOpenSeaAssetMetadata OpenSeaAssetMetadata;

	void OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange);
	void SetOpenSeaAssetMetadata(const FApiOpenSeaAssetMetadata& InMetadata);

	UPROPERTY()
	TWeakObjectPtr<class UUIEditorOpenSeaAssetsPropertiesWidget> PlaceableProperties;
	void UpdatePlaceableProperties();

	/** Save callback */
	virtual void OnPropertiesSaved(const FApiOpenSeaAssetMetadata& InMetadata);

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UImageMeshComponent> ImageMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGltfMeshComponent> GltfMeshComponent;

#pragma region Widget

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayPriority=0))
	TSubclassOf<class UUIPlaceableStateWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayPriority=0))
	FVector2D WidgetDrawSize = {160.0f, 40.0f};

private:
	UPROPERTY()
	TObjectPtr<class UWidgetComponent> WidgetComponent;

	UPROPERTY()
	TWeakObjectPtr<class UUIPlaceableStateWidget> StateWidget;

#pragma endregion Widget

};
