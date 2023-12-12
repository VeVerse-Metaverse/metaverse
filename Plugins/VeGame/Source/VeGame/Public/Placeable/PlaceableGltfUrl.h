// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "DownloadChunkSubsystem.h"
#include "InspectableActor.h"
#include "PlaceableComponent.h"
#include "Components/BoxComponent.h"
#include "PlaceableGltfUrl.generated.h"

struct FDownload1ResultMetadata;
class FVeDownload1;
class UglTFRuntimeAsset;


/** Base class for placeable actors which have files. */
UCLASS(Blueprintable, BlueprintType)
class VEGAME_API APlaceableGltfUrl : public AInspectableActor {
	GENERATED_BODY()

public:
	APlaceableGltfUrl();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	/** Save callback */
	virtual void OnPropertiesSaved(const FString& Url);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EApiFileType FileType;

private:
	/** Remote URL processing. */
	void OnPropertiesSaved_Http(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& Url);

	void ReplacePlaceableFile(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& InUrl, const FString& InMimeType);

	/** Local file processing. */
	void OnPropertiesSaved_File(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& Url);
	void OnPropertiesSaved_OpenSeaHttp(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& Url);

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIEditorGltfPropertiesWidget> PlaceablePropertiesClass;

	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;

	/** Class to use when creating a inspect properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inspectable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIInspectorModelPreviewWidget> InspectableModelPreviewClass;

	virtual UUserWidget* GetInspectPreviewWidget_Implementation(APlayerController* PlayerController) override;

protected:
	void OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<class UGltfMeshComponent> GltfMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placeable", meta=(DisplayPriority=0))
	TMap<int32, USceneComponent*> NodeComponents;

	UPROPERTY()
	TMap<int32, UStaticMesh*> ClientGltfMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placeable", meta=(DisplayPriority=0))
	TObjectPtr<UBoxComponent> BoxComponent;

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
