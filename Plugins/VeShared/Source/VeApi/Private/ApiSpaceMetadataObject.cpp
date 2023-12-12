// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiSpaceMetadataObject.h"

TArray<UObject*> UApiSpaceMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiSpaceMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiSpaceMetadata>& Entity : Entities) {
		UApiSpaceMetadataObject* Object = NewObject<UApiSpaceMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}
