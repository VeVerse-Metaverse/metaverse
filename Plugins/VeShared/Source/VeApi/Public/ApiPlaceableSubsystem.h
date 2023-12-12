// Author: Egor A. Pristavka & Khusan T. Yadgarov


#pragma once
#include "ApiPlaceableRequest.h"
#include "ApiSpaceRequest.h"
#include "VeShared.h"
#include "ApiPortalRequest.h"

class VEAPI_API FApiPlaceableSubsystem final : public IModuleSubsystem {
public:
	static const FName Name;
	
	virtual void Initialize() override;
	virtual void Shutdown() override;

	/** Get a placeable metadata.  */
	bool GetPlaceable(const FGuid& InPlaceableId, TSharedPtr<FOnPlaceableRequestCompleted> InCallback);

	/** Get list of placeables for the space.*/
	bool GetPlaceableBatch(const FGuid& InSpaceId, const IApiBatchRequestMetadata& InMetadata, TSharedPtr<FOnPlaceableBatchRequestCompleted> InCallback);

	/** Create or update a placeable. */
	bool UpdatePlaceable(const FGuid& InSpaceId, const FApiPlaceableMetadata& InPlaceableMetadata, TSharedPtr<FOnPlaceableRequestCompleted> InCallback);

	/** Update placeable transform. */
	bool UpdatePlaceableTransform(const FApiUpdatePlaceableTransformMetadata& InPlaceableMetadata, TSharedPtr<FOnPlaceableRequestCompleted> InCallback);

	/** Update placeable entity. */
	void UpdatePlaceableEntity(const FGuid& InPlaceableId, const FGuid& InEntityId, TSharedRef<FOnPlaceableRequestCompleted> InCallback);

	/** Delete placeable from the space. */
	bool DeletePlaceable(const FGuid& InPlaceableId, TSharedPtr<FOnGenericRequestCompleted> InCallback);

	bool GetPlaceableClassBatch(const FApiPlaceableClassBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnPlaceableClassBatchRequestCompleted> InCallback);

};

typedef TSharedPtr<FApiPlaceableSubsystem> FApiPlaceableSubsystemPtr;

