// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiSpaceRequest.h"
#include "VeShared.h"
#include "ApiPortalRequest.h"

class VEAPI_API FApiSpaceSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	DECLARE_EVENT_OneParam(FApiSpaceSubsystem, FOnSpaceCreated, const FApiSpaceMetadata& /*InMetadata*/);

	DECLARE_EVENT_OneParam(FApiSpaceSubsystem, FOnSpaceUpdated, const FApiSpaceMetadata& /*InMetadata*/);

	DECLARE_EVENT(FApiSpaceSubsystem, FOnSpaceDeleted);

	/** Generic new space create event. */
	// FOnSpaceCreated& GetOnCreated() { return OnSpaceCreated; }

	/** Generic space update event. */
	FOnSpaceUpdated& GetOnUpdated() { return OnSpaceUpdated; }

	/** Generic space delete event. */
	FOnSpaceDeleted& GetOnDeleted() { return OnSpaceDeleted; }

	/**
	 * Requests the space metadata by the id.
	 * @param InId Space ID.
	 * @param InCallback Complete callback delegate.
	 */
	// bool GetSpace(const FGuid& InId, TSharedPtr<FOnSpaceRequestCompleted> InCallback);

	/**
	 * Requests the space metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	// bool GetBatch(const FApiSpaceBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnSpaceBatchRequestCompleted> InCallback) const;

	/**
	 * Requests the space metadata batch for specific user.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatchForUser(const IApiUserBatchRequestMetadata& InMetadata, TSharedPtr<FOnSpaceBatchRequestCompleted> InCallback);

	/**
	 * Requests the space create.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	// bool Create(const FApiUpdateSpaceMetadata& InMetadata, TSharedPtr<FOnSpaceRequestCompleted> InCallback);

	/**
	 * Requests the space update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	bool Update(const FApiUpdateSpaceMetadata& InMetadata, TSharedPtr<FOnSpaceRequestCompleted> InCallback);

	/**
	 * Requests the space delete.
	 * @param InId Entity ID to be deleted.
	 * @param InCallback Complete callback delegate.
	 */
	bool Delete(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback);
	
private:
	// FOnSpaceCreated OnSpaceCreated;
	FOnSpaceUpdated OnSpaceUpdated;
	FOnSpaceDeleted OnSpaceDeleted;

	// void BroadcastApiSpaceCreated(const FApiSpaceMetadata& InMetadata) const;
	void BroadcastApiSpaceUpdated(const FApiSpaceMetadata& InMetadata) const;
	void BroadcastApiSpaceDeleted() const;

};

typedef TSharedPtr<FApiSpaceSubsystem> FApiSpaceSubsystemPtr;
