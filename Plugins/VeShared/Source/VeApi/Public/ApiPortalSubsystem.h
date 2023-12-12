// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiPortalMetadata.h"
#include "ApiPortalRequest.h"
#include "ApiRequest.h"
#include "VeShared.h"

class VEAPI_API FApiPortalSubsystem : public IModuleSubsystem{
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	
	DECLARE_EVENT_OneParam(FApiPortalSubsystem, FOnPortalCreated, const FApiPortalMetadata& /*InMetadata*/);

	DECLARE_EVENT_OneParam(FApiPortalSubsystem, FOnPortalUpdated, const FApiPortalMetadata& /*InMetadata*/);

	DECLARE_EVENT(FApiPortalSubsystem, FOnPortalDeleted);

	/**
	 * Requests the Portal metadata by the id.
	 * @param InId Portal ID.
	 * @param InCallback Complete callback delegate.
	 */
	// void GetPortal(const FGuid& InId, TSharedRef<FOnPortalRequestCompleted> InCallback);

	/**
	 * Requests the Portal metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	// bool GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnPortalBatchRequestCompleted> InCallback);

	/**
	 * Requests the Portal metadata batch for specific user.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	//bool GetBatchForUser(const IApiUserBatchRequestMetadata& InMetadata, TSharedPtr<FOnPortalBatchRequestCompleted> InCallback);

	/**
	 * Requests the portal create.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	// void Create(const FApiUpdatePortalMetadata& InMetadata, TSharedRef<FOnPortalRequestCompleted> InCallback);

	/**
	 * Requests the portal update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void Update(const FApiUpdatePortalMetadata& InMetadata, TSharedRef<FOnPortalRequestCompleted> InCallback);

	/**
	 * Requests the portal delete.
	 * @param InId Entity ID to be deleted.
	 */
	bool Delete(const FGuid& InId) const;

	/**
	 * Requests the portal delete.
	 * @param InId Entity ID to be deleted.
	 * @param InCallback Complete callback delegate.
	 */
	bool Delete(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback);

};

typedef TSharedPtr<FApiPortalSubsystem> FApiPortalSubsystemPtr;