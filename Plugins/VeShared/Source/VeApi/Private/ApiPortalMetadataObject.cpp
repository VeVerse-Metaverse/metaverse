// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPortalMetadataObject.h"

TArray<UObject*> UApiPortalMetadataObject::BatchToUObjectArray(TArray<FApiPortalMetadata> InEntities, UObject* InParent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const FApiPortalMetadata& Entity : InEntities) {
		UApiPortalMetadataObject* Object = NewObject<UApiPortalMetadataObject>(InParent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}

UObject* UApiPortalMetadataObject::ToUObject(const FApiPortalMetadata InEntity, UObject* InParent) {
	UApiPortalMetadataObject* Object = NewObject<UApiPortalMetadataObject>(InParent);
	Object->Metadata = InEntity;
	return Object;
}
