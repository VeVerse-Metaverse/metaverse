// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "InspectableActor.h"
#include "PlaceableUrlOpenSea.generated.h"

/** Base class for placeable actors which have files. */
UCLASS(Blueprintable, BlueprintType)
class VEGAME_API APlaceableUrlOpenSea : public AInspectableActor {
	GENERATED_BODY()

public:
	APlaceableUrlOpenSea();

	FString OriginalUrl;

	virtual void BeginPlay() override;

	/** Save callback */
	virtual void OnPropertiesSaved(const FString& Url);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EApiFileType FileType;

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIEditorOpenSeaUrlPropertiesWidget> PlaceablePropertiesClass;

	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;

	/** Class to use when creating a inspect properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inspectable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIInspectorImagePreviewWidget> InspectablePreviewClass;

	virtual UUserWidget* GetInspectPreviewWidget_Implementation(APlayerController* PlayerController) override;

	/** Class to use when creating a inspect properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inspectable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIInspectorUrlPropertiesWidget> InspectablePropertiesClass;

	virtual UUserWidget* GetInspectPropertiesWidget_Implementation(APlayerController* PlayerController) override;

private:
	/** Remote URL processing. */
	void OnPropertiesSaved_Http(const FString& Url);

	void ReplacePlaceableFile(const FString& InUrl, const FString& InMimeType, const FString& InOriginalUrl = FString{});

	void OnPropertiesSaved_OpenSeaHttp(const FString& Url);

	/** Local file processing. */
	void OnPropertiesSaved_File(const FString& Url);

};
