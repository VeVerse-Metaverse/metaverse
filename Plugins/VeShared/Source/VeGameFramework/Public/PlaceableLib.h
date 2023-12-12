// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "UObject/Object.h"
#include "VePlaceableMetadata.h"
#include "PlaceableLib.generated.h"


/**
 * 
 */
UCLASS()
class VEGAMEFRAMEWORK_API UPlaceableLib : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	static AActor* SpawnPlaceable(const UObject* WorldContextObject, const FVePlaceableMetadata& InVeMetadata, UClass* DefaultClass = nullptr);

	/** Simple function that allows to determine common mime types using file extensions. */
	UFUNCTION(BlueprintCallable)
	static FString GetFileMimeType(const FString& FilePath);

	/** Provides access to the actor placeable component. */
	UFUNCTION(BlueprintCallable)
	static class UPlaceableComponent* GetPlaceableComponent(const AActor* PlaceableActor);

	/** Creates and registers a new component at runtime. Can be called from another scene component. */
	UFUNCTION(BlueprintCallable)
	static USceneComponent* CreateRuntimeComponent(USceneComponent* Outer, const TSubclassOf<USceneComponent> Class, const FName Name, const EComponentCreationMethod& CreationMethod = EComponentCreationMethod::Instance);

	/** Deletes existing runtime component. Can be called from another scene component. */
	UFUNCTION(BlueprintCallable)
	static void DestroyRuntimeComponent(USceneComponent* Outer, USceneComponent* RuntimeComponent);
};
