// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEventMetadata.h"
#include "ApiEventMetadataObject.h"
#include "ApiEventRequest.h"
#include "VeShared.h"

class VEAPI_API FApiEventSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	DECLARE_EVENT_OneParam(FApiEventSubsystem, FOnEventCreated, const FApiEventMetadata& /*InMetadata*/);

	DECLARE_EVENT_OneParam(FApiEventSubsystem, FOnEventUpdated, const FApiEventMetadata& /*InMetadata*/);

	DECLARE_EVENT(FApiEventSubsystem, FOnEventDeleted);

	/** Generic new event create event. */
	FOnEventCreated& GetOnCreated() { return OnEventCreated; }

	/** Generic event update event. */
	FOnEventUpdated& GetOnUpdated() { return OnEventUpdated; }

	/** Generic event delete event. */
	FOnEventDeleted& GetOnDeleted() { return OnEventDeleted; }

	/**
	 * Requests the event metadata by the id.
	 * @param InId Event ID.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetEvent(const FGuid& InId, TSharedPtr<FOnEventRequestCompleted> InCallback);

	/**
	 * Requests the event metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnEventBatchRequestCompleted> InCallback);

	/**
	 * Requests the event metadata batch for specific user.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatchForUser(const IApiUserBatchRequestMetadata& InMetadata, TSharedPtr<FOnEventBatchRequestCompleted> InCallback);

	/**
	 * Requests the event create.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	bool Create(const FApiUpdateEventMetadata& InMetadata, TSharedPtr<FOnEventRequestCompleted> InCallback);

	/**
	 * Requests the event update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	bool Update(const FApiUpdateEventMetadata& InMetadata, TSharedPtr<FOnEventRequestCompleted> InCallback);

	/**
	 * Requests the event delete.
	 * @param InId Entity ID to be deleted.
	 * @param InCallback Complete callback delegate.
	 */
	bool Delete(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback);

private:
	FOnEventCreated OnEventCreated;
	FOnEventUpdated OnEventUpdated;
	FOnEventDeleted OnEventDeleted;

	void BroadcastApiEventCreated(const FApiEventMetadata& InMetadata) const;
	void BroadcastApiEventUpdated(const FApiEventMetadata& InMetadata) const;
	void BroadcastApiEventDeleted() const;

};

typedef TSharedPtr<FApiEventSubsystem> FApiEventSubsystemPtr;
