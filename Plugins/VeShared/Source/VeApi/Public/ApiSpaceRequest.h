// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiRequest.h"
#include "ApiSpaceMetadata.h"
#include "ApiSpaceMetadataObject.h"

/** Used to request spaces in batches (with pagination). */
// class VEAPI_API FApiSpaceBatchRequest final : public FApiRequest {
// public:
// 	virtual bool Process() override;
//
// 	FApiSpaceBatchQueryRequestMetadata RequestMetadata;
// 	FApiSpaceBatchMetadata ResponseMetadata;
//
// protected:
// 	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
// };

/** Used to request spaces in batches (with pagination). */
class VEAPI_API FApiUserSpaceBatchRequest final : public TApiBatchRequest<FApiSpaceMetadata/*, UApiSpaceMetadataObject*/> {
public:
	virtual bool Process() override;

	IApiUserBatchRequestMetadata RequestMetadata;
	FApiSpaceBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiMetaverseSpacesBatchRequest : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid MetaverseId;
	IApiBatchQueryRequestMetadata QueryMetadata;
	FApiSpaceBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiGetSpaceRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiSpaceMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create spaces. */
// class VEAPI_API FApiCreateSpaceRequest final : public FApiRequest {
// public:
// 	virtual bool Process() override;
//
// 	FApiUpdateSpaceMetadata RequestMetadata;
// 	FApiSpaceMetadata ResponseMetadata;
//
// protected:
// 	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
// };

/** Used to update spaces. */
class VEAPI_API FApiUpdateSpaceRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateSpaceMetadata RequestMetadata;
	FApiSpaceMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Space batch request delegate. */
typedef TDelegate<void(const FApiSpaceBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnSpaceBatchRequestCompleted;
/** Space entity request delegate. */
typedef TDelegate<void(const FApiSpaceMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnSpaceRequestCompleted;

