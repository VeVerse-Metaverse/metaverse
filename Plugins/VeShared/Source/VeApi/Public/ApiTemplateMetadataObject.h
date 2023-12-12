// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "ApiTemplateMetadata.h"
#include "ApiTemplateMetadataObject.generated.h"

/** Space metadata object. */
UCLASS()
class VEAPI_API UApiTemplateMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApiTemplateMetadata> Metadata;
	TWeakObjectPtr<UObject> Item;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApiTemplateMetadata>> Entities, UObject* Parent = nullptr);
};

//typedef TApiBatchMetadata<class FApiTemplateMetadata/*, class UApiSpaceMetadataObject*/> FApiTemplateBatchMetadata;
