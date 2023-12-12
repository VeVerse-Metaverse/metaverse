// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "ApiPropertyMetadata.h"
#include "ApiRequest.h"

class VEAPI_API FApiPropertyBatchRequest : public TApiBatchRequest<FApiPropertyMetadata> {
public:
	virtual bool Process() override;

	FGuid EntityId;

	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiPropertyBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

// class VEAPI_API FApiPropertyRequest : public FApiRequest {
// public:
// 	virtual bool Process() override;
//
// 	FGuid RequestEntityId = FGuid();
// 	FApiPropertyMetadata ResponseMetadata;
//
// protected:
// 	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
// };

class VEAPI_API FApiCreatePropertyRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid EntityId;

	FApiUpdatePropertyMetadata RequestMetadata;
	FApiPropertyMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUpdatePropertyRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid EntityId;

	FApiUpdatePropertyMetadata RequestMetadata;
	FApiPropertyMetadata ResponseMetadata;

	// FApiUpdatePropertyRequest() = default;
	
protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiDeletePropertyRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid EntityId;
	FString PropertyName;

	FApiUpdatePropertyMetadata RequestMetadata;
	FApiPropertyMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


/** Mod batch request delegate. */
typedef TDelegate<void(const FApiPropertyBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError)> FOnPropertyBatchRequestCompleted;
/** Mod entity request delegate. */
typedef TDelegate<void(const FApiPropertyMetadata& InMetadata, const bool bInSuccessful, const FString& InError)> FOnPropertyRequestCompleted;
