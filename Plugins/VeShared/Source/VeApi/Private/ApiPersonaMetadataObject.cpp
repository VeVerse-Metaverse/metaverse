// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPersonaMetadataObject.h"


TArray<UObject*> UApiPersonaMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiPersonaMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiPersonaMetadata>& Entity : Entities) {
		UApiPersonaMetadataObject* Object = NewObject<UApiPersonaMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}
