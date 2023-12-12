// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "Api2Request.h"
#include "Api2WorldMetadata.h"
#include "Api2PortalMetadata.h"

typedef TDelegate<void(const FApiSpaceMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError)> FOnWorldRequestCompleted2;
typedef TDelegate<void(const FApi2WorldBatchMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError)> FOnWorldBatchRequestCompleted2;
/**
 * Metadata for the space index request
 */
class VEAPI2_API FApiWorldIndexRequestMetadata final : public IApiBatchQueryRequestMetadata {
public:
	FApiWorldIndexRequestMetadata();

	FGuid PackageId;
	FString Platform;
	FString Deployment;

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

/**
 * Metadata for the space index request
 */
class VEAPI2_API FApi2WorldIndexRequest final : public TApi2BatchRequest<FApiSpaceMetadata> {
public:
	virtual bool Process() override;

	FApiWorldIndexRequestMetadata RequestMetadata;
	FApi2WorldBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;

};

class VEAPI2_API FApi2WorldRequestMetadata final : public IApiMetadata {
public:
	FApi2WorldRequestMetadata();

	FString Platform = FString();
	FString Deployment = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

/**
 * Request for the space index route
 */
class VEAPI2_API FApi2WorldIndexRequestV2 final : public TApi2BatchRequest<FApiSpaceMetadata> {
public:
	virtual bool Process() override;

	FApi2BatchSortRequestMetadata RequestMetadata;
	FApi2WorldBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2GetWorldRequestV2 final : public FApi2Request {
public:
	virtual bool Process() override;
	FGuid WorldId;
	FApi2GetRequestMetadata RequestMetadata;
	FApiSpaceMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/**
 * Request for the space index route
 */
class VEAPI2_API FApi2WorldRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid WorldId = FGuid{};
	FApi2WorldRequestMetadata WorldRequestMetadata;
	FApiSpaceMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create spaces. */
class VEAPI2_API FApi2CreateWorldRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUpdateSpaceMetadata RequestMetadata;
	FApiSpaceMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create spaces. */
class VEAPI2_API FApi2UpdateWorldRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUpdateSpaceMetadata RequestMetadata;
	FApiSpaceMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create spaces. */
class VEAPI2_API FApi2DeleteWorldRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestMetadataId;
	FApiSpaceMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2WorldPortalsBatchRequest : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid WorldId;
	IApiBatchQueryRequestMetadata QueryMetadata;
	FApi2PortalBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};