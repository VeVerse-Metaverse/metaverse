// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeShared.h"
#include "Api2PortalMetadata.h"
#include "Api2PortalRequest.h"


class VEAPI2_API FApi2PortalSubsystem final : public IModuleSubsystem {
public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/**
	 * Requests the Portal metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	void Index(const FApiPortalIndexRequestMetadata& InMetadata, TSharedRef<FOnPortalBatchRequestCompleted2> InCallback) const;

	/**
	 * Requests the Portal metadata by the id.
	 * @param Id Portal ID.
	 * @param InCallback Complete callback delegate.
	 */
	void Get(const FGuid& Id, TSharedRef<FOnPortalRequestCompleted2> InCallback) const;

	/**
	 * Requests the portal create.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void Create(const FApiUpdatePortalMetadata& InMetadata, TSharedRef<FOnPortalRequestCompleted2> InCallback) const;

};
