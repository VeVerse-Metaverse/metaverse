// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPlaceableMetadataObject.h"

TArray<UObject*> UApiPlaceableMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiPlaceableMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiPlaceableMetadata>& Entity : Entities) {
		UApiPlaceableMetadataObject* Object = NewObject<UApiPlaceableMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}

TArray<UObject*> UApiPlaceableClassMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiPlaceableClassMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiPlaceableClassMetadata>& Entity : Entities) {
		UApiPlaceableClassMetadataObject* Object = NewObject<UApiPlaceableClassMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}