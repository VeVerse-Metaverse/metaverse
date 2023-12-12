// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "Api2SharedAccessMetadata.h"
#include "Api2SharedAccessMetadataObject.generated.h"

/** SharedAccess metadata object. */
UCLASS()
class VEAPI2_API UApi2SharedAccessMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	TSharedPtr<FApi2SharedAccessMetadata> Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<TSharedPtr<FApi2SharedAccessMetadata>> Entities, UObject* Parent = nullptr);
};
