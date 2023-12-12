// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiOpenSeaMetadataObject.h"

TArray<UObject*> UApiOpenSeaMetadataObject::BatchToUObjectArray(TArray<FApiOpenSeaAssetMetadata> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const FApiOpenSeaAssetMetadata& Entity : Entities) {
		UApiOpenSeaMetadataObject* Asset = NewObject<UApiOpenSeaMetadataObject>(Parent);
		Asset->Metadata = Entity;
		Objects.Add(Asset);
	}
	return Objects;
}
