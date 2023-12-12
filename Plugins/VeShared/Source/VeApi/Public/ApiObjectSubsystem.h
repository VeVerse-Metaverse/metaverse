// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiObjectRequest.h"
#include "VeShared.h"

class VEAPI_API FApiObjectSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	DECLARE_EVENT_OneParam(FApiObjectSubsystem, FOnObjectCreated, const FApiObjectMetadata& /*InMetadata*/);

	DECLARE_EVENT_OneParam(FApiObjectSubsystem, FOnObjectUpdated, const FApiObjectMetadata& /*InMetadata*/);

	DECLARE_EVENT(FApiObjectSubsystem, FOnObjectDeleted);

	/**
	 * Requests the object metadata by the id.
	 * @param InId Object ID.
	 * @param InCallback Complete callback delegate.
	 */
	void GetObject(const FGuid& InId, TSharedRef<FOnObjectRequestCompleted> InCallback);

	/**
	 * Requests the object metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnObjectBatchRequestCompleted> InCallback);

	/**
	 * Requests the object metadata batch for specific user.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatchForUser(const IApiUserBatchRequestMetadata& InMetadata, TSharedPtr<FOnObjectBatchRequestCompleted> InCallback);

	/**
	 * Requests the object create.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void Create(const FApiUpdateObjectMetadata& InMetadata, TSharedRef<FOnObjectRequestCompleted> InCallback);

	/**
	 * Requests the object update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void Update(const FApiUpdateObjectMetadata& InMetadata, TSharedRef<FOnObjectRequestCompleted> InCallback);

	/**
	 * Requests the object delete.
	 * @param InId Entity ID to be deleted.
	 * @param InCallback Complete callback delegate.
	 */
	bool Delete(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback);

};

typedef TSharedPtr<FApiObjectSubsystem> FApiObjectSubsystemPtr;
