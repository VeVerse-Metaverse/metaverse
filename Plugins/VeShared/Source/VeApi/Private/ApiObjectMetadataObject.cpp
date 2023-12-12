// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiObjectMetadataObject.h"

TArray<UObject*> UApiObjectMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiObjectMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiObjectMetadata>& Entity : Entities) {
		UApiObjectMetadataObject* Object = NewObject<UApiObjectMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}
