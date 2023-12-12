// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#pragma once

#include "ManagedTextureRequester.h"
#include "InspectableActor.h"
#include "PlaceableUrlActor.generated.h"

class UMediaTexture;
class UMediaPlayer;
class UStreamMediaSource;
class UMediaSoundComponent;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class VEGAME_API APlaceableUrlActor : public AInspectableActor, public IManagedTextureRequester {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlaceableUrlActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
#pragma region Canvas

	/** Painting canvas mesh component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="InteractivePainting")
	TObjectPtr<UStaticMeshComponent> CanvasMeshComponent;

	/** Painting frame mesh component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="InteractivePainting")
	TObjectPtr<UStaticMeshComponent> FrameMeshComponent;

	/** Painting canvas mesh, should be 1x1 cm X-positive oriented plane mesh or 1x1x1 cube with correctly set UVs. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="InteractivePainting")
	TObjectPtr<UStaticMesh> CanvasMesh;

	/** Painting frame, should be 1x1x1 cm cube. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="InteractivePainting")
	TObjectPtr<UStaticMesh> FrameMesh;

	/** Material to use for the painting canvas. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="InteractivePainting")
	TObjectPtr<UMaterialInterface> CanvasMaterial;

	/** Material to use for the painting canvas. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="InteractivePainting")
	TObjectPtr<UMaterialInterface> CanvasVideoMaterial;

	/** Material to use for the painting frame. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="InteractivePainting")
	TObjectPtr<UMaterialInterface> FrameMaterial;

	/** Dynamic material instance created for the painting canvas. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractivePainting")
	TObjectPtr<UMaterialInstanceDynamic> CanvasMaterialInstance;

	/** Dynamic material instance created for the painting frame. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractivePainting")
	TObjectPtr<UMaterialInstanceDynamic> FrameMaterialInstance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USoundAttenuation> StreamAudioAttenuationSettings = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UMediaSoundComponent> StreamAudioComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStreamMediaSource> StreamMediaSource;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UMediaPlayer> StreamMediaPlayer;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UMediaTexture> StreamMediaTexture;

	/** Painting height */
	UPROPERTY(BlueprintReadOnly)
	float Height = 128.f;

	/** Painting width */
	UPROPERTY(BlueprintReadOnly)
	float Width = 128.f;

	/** Painting frame color. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractivePainting")
	FLinearColor FrameColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	/** Painting frame material color parameter name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractivePainting")
	FName FrameMaterialColorParameterName = TEXT("C");

	/** Painting frame material color parameter name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractivePainting")
	FName CanvasMaterialTextureParameterName = TEXT("T");

	/** Painting frame material color parameter name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractivePainting")
	FName CanvasMaterialEmissivePowerParameterName = TEXT("E");

	/** Emissive power for the painting canvas material, use to adjust for scene environment if required. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractivePainting")
	float EmissivePower = 0.0f;

	/** Used to request video streaming source by its url. */
	void SetStreamMediaSourceUrl(const FString& InUrl);

	/** Callback to set up the canvas texture once it was loaded. */
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

#pragma endregion

	/** Callback for the placeable component placed/loaded events. */
	void OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata);

	/** Callback for the properties widget save event. */
	void OnSaved(const FString& Url);

	/** Requests API to update a placeable URL. */
	void UpdatePlaceableUrl(const struct FPlaceableAssetMetadata& InMetadata);

	// UPROPERTY(BlueprintReadWrite, EditAnywhere)
	// TSubclassOf<class UUIUrlPropertiesWidget> PropertiesWidgetClass;

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIUrlPropertiesWidget> PlaceablePropertiesClass;
	
	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;
};
