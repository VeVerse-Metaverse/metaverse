// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "ManagedTextureRequester.h"
#include "ApiFileMetadata.h"
#include "VeFileMetadata.h"
#include "ImageMeshComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEGAME_API UImageMeshComponent : public USceneComponent, public IManagedTextureRequester {
	GENERATED_BODY()

	DECLARE_EVENT(UPictureMeshComponent, FPictureMeshComponentEvent);

public:
	// Sets default values for this component's properties
	UImageMeshComponent();

	bool ShowImage(const FString& InURL, TEnumAsByte<enum TextureGroup> TextureGroup);

	/** Sets up the canvas dimensions using metadata. */
	void SetSize(float Height, float Width);

	void SetIsVisible(bool IsVisible);

	FPictureMeshComponentEvent OnDownloadingBegin;
	FPictureMeshComponentEvent OnDownloadingEnd;

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="PictureMeshComponent|Canvas", meta=(DisplayPriority=0))
	TObjectPtr<UStaticMesh> CanvasStaticMesh;

	/** Base material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="PictureMeshComponent|Canvas", meta=(DisplayPriority=0))
	TObjectPtr<UMaterial> CanvasMaterial;

	// UPROPERTY(Transient, VisibleAnywhere, Category="PlaceableArtObject|Canvas", meta=(DisplayPriority=0))
	// TObjectPtr<class UMediaTexture> StreamMediaTexture;

	/** Canvas frame material color parameter name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PictureMeshComponent|Canvas", meta=(DisplayPriority=0))
	FName CanvasMaterialTextureParameterName = TEXT("T");


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="PictureMeshComponent|Frame", meta=(DisplayPriority=0))
	TObjectPtr<UStaticMesh> FrameStaticMesh;

	/** Material to use for the canvas frame. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="PictureMeshComponent|Frame", meta=(DisplayPriority=0))
	TObjectPtr<UMaterialInterface> FrameMaterial;

	/** Canvas frame material color parameter name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PictureMeshComponent|Frame", meta=(DisplayPriority=0))
	FName FrameMaterialColorParameterName = TEXT("C");

	/** Canvas frame color. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PictureMeshComponent|Frame", meta=(DisplayPriority=0))
	FLinearColor FrameColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);


	/** Default canvas height */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="PictureMeshComponent", meta=(DisplayPriority=0))
	float DefaultHeight = 100.0f;

	/** Default canvas width */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="PictureMeshComponent", meta=(DisplayPriority=0))
	float DefaultWidth = 100.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="PictureMeshComponent")
	UTexture2D* DefaultTexture;

	void SetIsLoading(bool Value);
	void SetTexture(UTexture* Texture) const;

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

private:
	bool bIsVisible = false;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CanvasMaterialInstance;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> FrameMaterialInstance;

	FString CurrentURL;

	/** Canvas frame mesh component. */
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> FrameMeshComponent;

	/** Canvas mesh component. */
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> CanvasMeshComponent;

};
