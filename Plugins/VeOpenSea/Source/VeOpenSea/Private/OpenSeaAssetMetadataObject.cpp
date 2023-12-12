// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "OpenSeaAssetMetadataObject.h"

TArray<UObject*> UOpenSeaAssetMetadataObject::BatchToUObjectArray(TArray<FOpenSeaAssetMetadata> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const FOpenSeaAssetMetadata& Entity : Entities) {
		UOpenSeaAssetMetadataObject* Asset = NewObject<UOpenSeaAssetMetadataObject>(Parent);
		Asset->Metadata = Entity;
		Objects.Add(Asset);
	}
	return Objects;
}
