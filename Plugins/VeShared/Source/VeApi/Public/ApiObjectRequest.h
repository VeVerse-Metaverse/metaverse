// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiRequest.h"
#include "ApiObjectMetadata.h"
#include "ApiObjectMetadataObject.h"

/** Used to request objects in batches (with pagination). */
class VEAPI_API FApiObjectBatchRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiObjectBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to request objects in batches (with pagination). */
class VEAPI_API FApiUserObjectBatchRequest final : public TApiBatchRequest<FApiObjectMetadata/*, UApiObjectMetadataObject*/> {
public:
	virtual bool Process() override;

	IApiUserBatchRequestMetadata RequestMetadata;
	FApiObjectBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiGetObjectRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiObjectMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create objects. */
class VEAPI_API FApiCreateObjectRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateObjectMetadata RequestMetadata;
	FApiObjectMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to update objects. */
class VEAPI_API FApiUpdateObjectRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateObjectMetadata RequestMetadata;
	FApiObjectMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Object batch request delegate. */
typedef TDelegate<void(const FApiObjectBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnObjectBatchRequestCompleted;
/** Object entity request delegate. */
typedef TDelegate<void(const FApiObjectMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnObjectRequestCompleted;
