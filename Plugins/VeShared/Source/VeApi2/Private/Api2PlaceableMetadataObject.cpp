// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2PlaceableMetadataObject.h"

TArray<UObject*> UApi2PlaceableMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiPlaceableMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiPlaceableMetadata>& Entity : Entities) {
		UApi2PlaceableMetadataObject* Object = NewObject<UApi2PlaceableMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}

TArray<UObject*> UApi2PlaceableClassMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiPlaceableClassMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiPlaceableClassMetadata>& Entity : Entities) {
		UApi2PlaceableClassMetadataObject* Object = NewObject<UApi2PlaceableClassMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}