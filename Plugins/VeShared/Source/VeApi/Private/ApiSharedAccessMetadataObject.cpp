// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiSharedAccessMetadataObject.h"

TArray<UObject*> UApiSharedAccessMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiSharedAccessMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiSharedAccessMetadata>& Entity : Entities) {
		UApiSharedAccessMetadataObject* Object = NewObject<UApiSharedAccessMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}
