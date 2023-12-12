// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "MappingContextCollection.h"

#include "InputMappingContext.h"


bool FMappingContextCollection::AddMappingContext(UObject* Object, UInputMappingContext* MappingContext) {
	if (!Object || !MappingContext) {
		return false;
	}

	// MappingContext already exists
	if (auto* MappingContextObjects = Collection.Find(MappingContext)) {
		MappingContextObjects->Objects.Emplace(Object);
		return false;
	}

	// Add MappingContext
	FObjectSetStruct MappingContextObjects;
	MappingContextObjects.Objects.Emplace(Object);
	Collection.Emplace(MappingContext, MappingContextObjects);

	return true;
}

TSet<UInputMappingContext*> FMappingContextCollection::AddMappingContextArray(UObject* Object, const TArray<TObjectPtr<UInputMappingContext>>& MappingContexts) {
	TSet<UInputMappingContext*> ResultMappingContexts;
	
	if (!Object) {
		return ResultMappingContexts;
	}

	for (auto& MappingContext : MappingContexts) {
		if (!MappingContext) {
			continue;
		}

		// MappingContext already exists
		if (auto* MappingContextObjects = Collection.Find(MappingContext)) {
			MappingContextObjects->Objects.Emplace(Object);
			continue;
		}

		// Add MappingContext
		FObjectSetStruct MappingContextObjects;
		MappingContextObjects.Objects.Emplace(Object);
		Collection.Emplace(MappingContext, MappingContextObjects);

		ResultMappingContexts.Emplace(MappingContext);
	}

	return ResultMappingContexts;
}

bool FMappingContextCollection::RemoveMappingContext(UObject* Object, UInputMappingContext* MappingContext) {
	if (!Object || !MappingContext) {
		return false;
	}

	// MappingContext does not exist
	auto* MappingContextObjects = Collection.Find(MappingContext);
	if (!MappingContextObjects) {
		return false;
	}

	// Object does not exist
	if (!MappingContextObjects->Objects.Remove(Object)) {
		return false;
	}

	// Objects is not empty
	if (MappingContextObjects->Objects.Num()) {
		return false;
	}

	// Remove MappingContext
	Collection.Remove(MappingContext);
	return true;
}

TSet<UInputMappingContext*> FMappingContextCollection::RemoveMappingContextArray(UObject* Object, const TArray<TObjectPtr<UInputMappingContext>>& MappingContexts) {
	TSet<UInputMappingContext*> ResultMappingContexts;

	if (!Object) {
		return ResultMappingContexts;
	}

	for (auto& MappingContext : MappingContexts) {
		if (!MappingContext) {
			continue;
		}

		// MappingContext does not exist
		auto* MappingContextObjects = Collection.Find(MappingContext);
		if (!MappingContextObjects) {
			continue;
		}

		// Object does not exist
		if (!MappingContextObjects->Objects.Remove(Object)) {
			continue;
		}

		// Objects is not empty
		if (MappingContextObjects->Objects.Num()) {
			continue;
		}

		// Remove MappingContext
		Collection.Remove(MappingContext);
		ResultMappingContexts.Emplace(MappingContext);
	}

	return ResultMappingContexts;
}

TSet<UInputMappingContext*> FMappingContextCollection::ClearMappingContext(UObject* Object) {
	TSet<UInputMappingContext*> ResultMappingContexts;

	if (!Object) {
		return ResultMappingContexts;
	}

	for (auto& Elem : Collection) {
		if (Elem.Value.Objects.Remove(Object)) {
			if (!Elem.Value.Objects.Num()) {
				ResultMappingContexts.Emplace(Elem.Key);
			}
		}
	}

	for (auto& MappingContext : ResultMappingContexts) {
		Collection.Remove(MappingContext);
	}

	return ResultMappingContexts;
}
