// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiPortalMetadata.h"
#include "ApiRequest.h"

// class VEAPI_API FApiPortalBatchRequest : public TApiBatchRequest<FApiPortalMetadata> {
// public:
// 	virtual bool Process() override;
//
// 	IApiBatchQueryRequestMetadata RequestMetadata;
// 	FApiPortalBatchMetadata ResponseMetadata;
//
// protected:
// 	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
// };

// class VEAPI_API FApiGetPortalRequest : public FApiRequest {
// public:
// 	virtual bool Process() override;
//
// 	FGuid RequestEntityId = FGuid();
// 	FApiPortalMetadata ResponseMetadata;
//
// protected:
// 	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
// };

// class VEAPI_API FApiCreatePortalRequest final : public FApiRequest {
// public:
// 	virtual bool Process() override;
//
// 	FApiUpdatePortalMetadata RequestMetadata;
// 	FApiPortalMetadata ResponseMetadata;
//
// protected:
// 	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
// };

/** Used to update mods. */
class VEAPI_API FApiUpdatePortalRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdatePortalMetadata RequestMetadata;
	FApiPortalMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Mod batch request delegate. */
typedef TDelegate<void(const FApiPortalBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnPortalBatchRequestCompleted;
/** Mod entity request delegate. */
typedef TDelegate<void(const FApiPortalMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnPortalRequestCompleted;