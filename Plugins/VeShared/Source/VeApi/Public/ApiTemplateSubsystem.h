// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "ApiTemplateMetadata.h"
#include "ApiTemplateRequest.h"
#include "VeShared.h"

class VEAPI_API FApiTemplateSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	DECLARE_EVENT_OneParam(FApiTemplateSubsystem, FOnTemplateCreated, const FApiTemplateMetadata& /*InMetadata*/);

	DECLARE_EVENT_OneParam(FApiTemplateSubsystem, FOnTemplateUpdated, const FApiTemplateMetadata& /*InMetadata*/);

	DECLARE_EVENT(FApiTemplateSubsystem, FOnTemplateDeleted);

	/** Generic new space create event. */
	FOnTemplateCreated& GetOnCreated() { return OnTemplateCreated; }

	/** Generic space update event. */
	FOnTemplateUpdated& GetOnUpdated() { return OnTemplateUpdated; }

	/** Generic space delete event. */
	FOnTemplateDeleted& GetOnDeleted() { return OnTemplateDeleted; }

	/**
	 * Requests the space metadata by the id.
	 * @param InId Space ID.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetTemplate(const FGuid& InId, TSharedPtr<FOnTemplateRequestCompleted> InCallback);

	/**
	 * Requests the space metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnTemplateBatchRequestCompleted> InCallback) const;

	/**
	 * Requests the space metadata batch for specific user.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	//bool GetBatchForUser(const IApiUserBatchRequestMetadata& InMetadata, TSharedPtr<FOnTemplateBatchRequestCompleted> InCallback);

	/**
	 * Requests the space create.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	bool Create(const FApiUpdateTemplateMetadata& InMetadata, TSharedPtr<FOnTemplateRequestCompleted> InCallback);

	/**
	 * Requests the space update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	bool Update(const FApiUpdateTemplateMetadata& InMetadata, TSharedPtr<FOnTemplateRequestCompleted> InCallback);

	/**
	 * Requests the space delete.
	 * @param InId Entity ID to be deleted.
	 * @param InCallback Complete callback delegate.
	 */
	//bool Delete(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback);


private:
	FOnTemplateCreated OnTemplateCreated;
	FOnTemplateUpdated OnTemplateUpdated;
	FOnTemplateDeleted OnTemplateDeleted;

	void BroadcastApiTemplateCreated(const FApiTemplateMetadata& InMetadata) const;
	void BroadcastApiTemplateUpdated(const FApiTemplateMetadata& InMetadata) const;
	void BroadcastApiTemplateDeleted() const;

};

typedef TSharedPtr<FApiTemplateSubsystem> FApiTemplateSubsystemPtr;
