// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2SharedAccessMetadataObject.h"

TArray<UObject*> UApi2SharedAccessMetadataObject::BatchToUObjectArray(TArray<TSharedPtr<FApi2SharedAccessMetadata>> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const TSharedPtr<FApi2SharedAccessMetadata>& Entity : Entities) {
		UApi2SharedAccessMetadataObject* Object = NewObject<UApi2SharedAccessMetadataObject>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}
