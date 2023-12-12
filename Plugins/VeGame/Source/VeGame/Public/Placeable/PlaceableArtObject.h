// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "InspectableActor.h"
#include "PlaceableComponent.h"
#include "ApiObjectMetadata.h"
#include "PlaceableArtObject.generated.h"

/** Base class for placeable actors which have files. */
UCLASS(Blueprintable, BlueprintType)
class VEGAME_API APlaceableArtObject : public AInspectableActor {
	GENERATED_BODY()

public:
	APlaceableArtObject();

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIPlaceableArtObjectPropertiesWidget> PlaceablePropertiesClass;

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
	TSubclassOf<class UUIInspectorArtObjectPropertiesWidget> InspectablePropertiesClass;

	virtual UUserWidget* GetInspectPropertiesWidget_Implementation(APlayerController* PlayerController) override;

protected:
	virtual void PostInitializeComponents() override;

	FApiObjectMetadata ArtObjectMetadata;

	void OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange);
	void SetObjectMetadata(const FApiObjectMetadata& InMetadata);

	UPROPERTY()
	TWeakObjectPtr<class UUIPlaceableArtObjectPropertiesWidget> PlaceableProperties;
	void UpdatePlaceableProperties();

	/** Save callback */
	virtual void OnPropertiesSaved(const FApiObjectMetadata& InMetadata);

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
