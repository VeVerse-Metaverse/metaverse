// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiServerMetadataObject.h"

TArray<UObject*> UApiServerMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiServerMetadata>> InEntities, UObject* InParent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiServerMetadata>& Entity : InEntities) {
		UApiServerMetadataObject* Object = NewObject<UApiServerMetadataObject>(InParent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}
