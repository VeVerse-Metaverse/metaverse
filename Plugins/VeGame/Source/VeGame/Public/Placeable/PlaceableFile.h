// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "InspectableActor.h"
#include "ApiRequest.h"
#include "PlaceableFile.generated.h"

/** Base class for placeable actors which have files. */
UCLASS(Blueprintable, BlueprintType)
class VEGAME_API APlaceableFile : public AInspectableActor {
	GENERATED_BODY()

public:
	APlaceableFile();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EApiFileType FileType;

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIEditorUrlPropertiesWidget> PlaceablePropertiesClass;

	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;

	/** Class to use when creating a inspect properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inspectable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIInspectorImagePreviewWidget> InspectablePreviewClass;

	virtual UUserWidget* GetInspectPreviewWidget_Implementation(APlayerController* PlayerController) override;

	/** Class to use when creating a inspect properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inspectable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIInspectorUrlPropertiesWidget> InspectablePropertiesClass;

	virtual UUserWidget* GetInspectPropertiesWidget_Implementation(APlayerController* PlayerController) override;

protected:
	/** Save callback */
	virtual void OnPropertiesSaved(const FString& Url);

private:
	/** Remote URL processing. */
	void OnPropertiesSaved_Http(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& Url);

	void ReplacePlaceableFile(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& InUrl, const FString& InMimeType);

	/** Local file processing. */
	void OnPropertiesSaved_File(TSharedRef<FOnGenericRequestCompleted> InCallback, const FString& Url);

};
