// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2Request.h"
#include "Api2PortalMetadata.h"


class VEAPI2_API FApiPortalIndexRequestMetadata final : public IApiBatchQueryRequestMetadata {
public:
	FApiPortalIndexRequestMetadata();

	FString Platform = FString();
	FString Deployment = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

class VEAPI2_API FApiPortalRequestMetadata final : public IApiMetadata {
public:
	FApiPortalRequestMetadata();

	FGuid Id = FGuid();
	FString Platform = FString();
	FString Deployment = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};


class VEAPI2_API FApi2PortalBatchRequest final : public TApi2BatchRequest<FApiPortalMetadata> {
public:
	virtual bool Process() override;

	FApiPortalIndexRequestMetadata RequestMetadata;
	FApi2PortalBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2PortalRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiPortalRequestMetadata RequestMetadata;
	FApiPortalMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2CreatePortalRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUpdatePortalMetadata RequestMetadata;
	FApiPortalMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


typedef TDelegate<void(const FApiPortalMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError)> FOnPortalRequestCompleted2;
typedef TDelegate<void(const FApi2PortalBatchMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError)> FOnPortalBatchRequestCompleted2;
