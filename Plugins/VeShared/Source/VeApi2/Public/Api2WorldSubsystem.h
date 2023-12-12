// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeShared.h"
#include "Api2WorldRequest.h"
#include "Api2PortalRequest.h"


class VEAPI2_API FApi2WorldSubsystem final : public IModuleSubsystem {
public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/**
	 * Requests the space metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	void Index(const FApiWorldIndexRequestMetadata& InMetadata, TSharedRef<FOnWorldBatchRequestCompleted2> InCallback) const;

	/**
	 * Requests the space metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	void IndexV2(const FApi2BatchSortRequestMetadata& InMetadata, TSharedRef<FOnWorldBatchRequestCompleted2> InCallback) const;

	/**
	 * Requests the space metadata by the id.
	 * @param InWorldId World ID.
	 * @param InCallback Complete callback delegate.
	 */
	void Get(const FGuid& InWorldId, TSharedRef<FOnWorldRequestCompleted2> InCallback) const;

	/**
	 * Requests the space metadata by the id.
	 * @param InWorldId World ID.
	 * @param InCallback Complete callback delegate.
	 */
	void GetV2(const FGuid& InWorldId,const FApi2GetRequestMetadata& InMetadata, TSharedRef<FOnWorldRequestCompleted2> InCallback) const;

	/**
	 * Requests the space create.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void Create(const FApiUpdateSpaceMetadata& InMetadata, TSharedRef<FOnWorldRequestCompleted2> InCallback) const;

	/**
	 * Requests the space update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void Update(const FApiUpdateSpaceMetadata& InMetadata, TSharedRef<FOnWorldRequestCompleted2> InCallback) const;

	/**
	 * Requests the world delete.
	 * @param InWorldId Entity ID to be deleted.
	 * @param InCallback Complete callback delegate.
	 */
	void Delete(const FGuid& InWorldId, TSharedRef<FOnWorldRequestCompleted2> InCallback) const;

	void GetPortalBatch(const FGuid& WorldId, const IApiBatchQueryRequestMetadata& QueryMetadata, TSharedRef<FOnPortalBatchRequestCompleted2> InCallback) const;
};
