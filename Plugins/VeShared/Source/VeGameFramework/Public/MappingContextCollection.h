// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

// #include "UObject/Object.h"
#include "MappingContextCollection.generated.h"

class UInputMappingContext;

USTRUCT()
struct FObjectSetStruct {
	GENERATED_BODY()

	UPROPERTY()
	TSet<TWeakObjectPtr<UObject>> Objects;
};

/**
 * 
 */
USTRUCT()
struct VEGAMEFRAMEWORK_API FMappingContextCollection {
	GENERATED_BODY()

	// DECLARE_DELEGATE_OneParam(FMappingContextCollectionChanged, TArray<UInputMappingContext*> MappingContexts);

public:
	int32 GetMappingContexts(TArray<TObjectPtr<UInputMappingContext>>& MappingContexts) const { return Collection.GetKeys(MappingContexts); }

	/**
	 * Add MappingContext.
	 * @return True if MappingContext added
	 **/
	bool AddMappingContext(UObject* Object, UInputMappingContext* MappingContext);

	/**
	 * Add MappingContext list.
	 * @return Array of added MappingContext
	 **/
	TSet<UInputMappingContext*> AddMappingContextArray(UObject* Object, const TArray<TObjectPtr<UInputMappingContext>>& MappingContexts);

	/**
	 * Remove MappingContext.
	 * @return True if MappingContext removed
	 **/
	bool RemoveMappingContext(UObject* Object, UInputMappingContext* MappingContext);

	/**
	 * Remove MappingContext list.
	 * @return Array of removed MappingContext
	 **/
	TSet<UInputMappingContext*> RemoveMappingContextArray(UObject* Object, const TArray<TObjectPtr<UInputMappingContext>>& MappingContexts);

	/**
	 * Remove all MappingContext by Object.
	 * @return Array of removed MappingContext
	 **/
	TSet<UInputMappingContext*> ClearMappingContext(UObject* Object);

protected:
	UPROPERTY()
	TMap<TObjectPtr<UInputMappingContext>, FObjectSetStruct> Collection;

};
