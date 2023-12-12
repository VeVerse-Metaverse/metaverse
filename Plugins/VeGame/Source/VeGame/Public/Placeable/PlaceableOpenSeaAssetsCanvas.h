// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ManagedTextureRequester.h"
#include "PlaceableComponent.h"
#include "PlaceableUrlOpenSea.h"
#include "PlaceableOpenSeaAssetsCanvas.generated.h"

class UMediaTexture;
class UMediaPlayer;
class UStreamMediaSource;
class UMediaSoundComponent;

UCLASS(Blueprintable, NotBlueprintType)
class VEGAME_API APlaceableOpenSeaAssetsCanvas final : public APlaceableUrlOpenSea, public IManagedTextureRequester {
	GENERATED_BODY()

public:
	APlaceableOpenSeaAssetsCanvas();

protected:
	void OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange);

	virtual void BeginPlay() override;

#pragma region Canvas

	/** Canvas mesh component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UStaticMeshComponent> CanvasMeshComponent;

	/** Canvas frame mesh component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UStaticMeshComponent> FrameMeshComponent;

	/** Canvas mesh, should be 1x1 cm X-positive oriented plane mesh or 1x1x1 cube with correctly set UVs. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UStaticMesh> CanvasMesh;

	/** Canvas frame, should be 1x1x1 cm cube. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UStaticMesh> FrameMesh;

	/** Material to use for the canvas. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UMaterialInterface> CanvasMaterial;

	/** Material to use for the canvas. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UMaterialInterface> CanvasVideoMaterial;

	/** Material to use for the canvas frame. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UMaterialInterface> FrameMaterial;

	/** Dynamic material instance created for the canvas. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UMaterialInstanceDynamic> CanvasMaterialInstance;

	/** Dynamic material instance created for the canvas frame. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UMaterialInstanceDynamic> FrameMaterialInstance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<USoundAttenuation> StreamAudioAttenuationSettings = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UMediaSoundComponent> StreamAudioComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UStreamMediaSource> StreamMediaSource;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UMediaPlayer> StreamMediaPlayer;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UMediaTexture> StreamMediaTexture;

	/** Canvas height */
	UPROPERTY(BlueprintReadOnly, Category="Placeable", meta=(DisplayPriority=0))
	float Height = 256.f;

	/** Canvas width */
	UPROPERTY(BlueprintReadOnly, Category="Placeable", meta=(DisplayPriority=0))
	float Width = 256.f;

	/** Default canvas height */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Placeable", meta=(DisplayPriority=0))
	float DefaultHeight = 256.f;

	/** Default canvas width */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Placeable", meta=(DisplayPriority=0))
	float DefaultWidth = 256.f;

	/** Canvas frame color. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placeable", meta=(DisplayPriority=0))
	FLinearColor FrameColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	/** Canvas frame material color parameter name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placeable", meta=(DisplayPriority=0))
	FName FrameMaterialColorParameterName = TEXT("C");

	/** Canvas frame material color parameter name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placeable", meta=(DisplayPriority=0))
	FName CanvasMaterialTextureParameterName = TEXT("T");

	/** Canvas frame material color parameter name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placeable", meta=(DisplayPriority=0))
	FName CanvasMaterialEmissivePowerParameterName = TEXT("E");

	/** Emissive power for the canvas material, use to adjust for scene environment if required. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placeable", meta=(DisplayPriority=0))
	float EmissivePower = 0.0f;

	/** Used to request video streaming source by its url. */
	void SetStreamMediaSourceUrl(const FString& InUrl);

	/** Callback to set up the canvas texture once it was loaded. */
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	/** Sets up the canvas dimensions using metadata. */
	UFUNCTION(BlueprintCallable)
	void UpdateCanvasDimensions() const;

#pragma endregion
};
