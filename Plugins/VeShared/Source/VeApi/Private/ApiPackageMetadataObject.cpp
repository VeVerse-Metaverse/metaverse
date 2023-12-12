// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPackageMetadataObject.h"

TArray<UObject*> UApiPackageMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiPackageMetadata>> InEntities, UObject* InParent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiPackageMetadata>& Entity : InEntities) {
		UApiPackageMetadataObject* Object = NewObject<UApiPackageMetadataObject>(InParent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}
