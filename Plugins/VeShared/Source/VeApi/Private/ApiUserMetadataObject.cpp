// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "ApiUserMetadataObject.h"

TArray<UObject*> UApiUserMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiUserMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiUserMetadata>& Entity : Entities) {
		UApiUserMetadataObject* Object = NewObject<UApiUserMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}
