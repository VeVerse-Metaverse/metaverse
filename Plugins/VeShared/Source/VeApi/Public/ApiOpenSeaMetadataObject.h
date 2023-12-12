#pragma once
#include "ApiOpenSeaMetadata.h"

#include "ApiOpenSeaMetadataObject.generated.h"

/** Space metadata object. */
UCLASS()
class VEAPI_API UApiOpenSeaMetadataObject final : public UObject {
	GENERATED_BODY()

public:
	FApiOpenSeaAssetMetadata Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<FApiOpenSeaAssetMetadata> Entities, UObject* Parent = nullptr);
};

//typedef TApiBatchMetadata<class FOpenSeaAssetMetadata/*, class UApiObjectMetadataObject*/> FOpenSeaAssetBatchMetadata;
