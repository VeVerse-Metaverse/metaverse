// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlaceableActor.h"
#include "PlaceableText.generated.h"

class UWidget;
class UBoxComponent;

UCLASS()
class VEGAME_API APlaceableText : public APlaceableActor {
	GENERATED_BODY()

	struct FPlaceableTextParams {
		FString Text = "Text";
		FColor BaseColor = FColor::White;
	};

public:
	// Sets default values for this actor's properties
	APlaceableText();

	virtual void PostInitializeComponents() override;
	void OnPlaceableMetadataUpdated(const FVePlaceableMetadata& InMetadata);

	FPlaceableTextParams DefaultTextParams;
	FPlaceableTextParams CurrentTextParams;
	void SetTextParams(FPlaceableTextParams InMaterialParams);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayPriority=0))
	TObjectPtr<UWidget> WidgetComponent;

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIPlaceableTextPropertiesWidget> PlaceablePropertiesClass;
	
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	class UTextRenderComponent* PlaceableTextRenderClass;
	
	UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController);
	
	void Properties_OnSave(UUIPlaceableTextPropertiesWidget* PropertiesWidget);
	void Properties_OnReset(UUIPlaceableTextPropertiesWidget* PropertiesWidget);
	void Properties_OnChanged(UUIPlaceableTextPropertiesWidget* PropertiesWidget);

};
