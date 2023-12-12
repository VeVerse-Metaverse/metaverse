// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "ApiTemplateMetadataObject.h"


TArray<UObject*> UApiTemplateMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApiTemplateMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApiTemplateMetadata>& Entity : Entities) {
		UApiTemplateMetadataObject* Object = NewObject<UApiTemplateMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}
