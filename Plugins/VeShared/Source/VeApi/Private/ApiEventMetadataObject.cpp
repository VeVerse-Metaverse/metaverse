// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiEventMetadataObject.h"

TArray<UObject*> UApiEventMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiEventMetadata>> InEntities, UObject* InParent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiEventMetadata>& Entity : InEntities) {
		UApiEventMetadataObject* Object = NewObject<UApiEventMetadataObject>(InParent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}
