// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiRequest.h"
#include "ApiEventMetadata.h"

/** Used to request mods in batches (with pagination). */
class VEAPI_API FApiEventBatchRequest final : public TApiBatchRequest<FApiEventMetadata/*, UApiModMetadataObject*/> {
public:
	virtual bool Process() override;

	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiEventBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiGetEventRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiEventMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


/** Used to request mods in batches (with pagination). */
class VEAPI_API FApiUserEventBatchRequest final : public TApiBatchRequest<FApiEventMetadata/*, UApiModMetadataObject*/> {
public:
	virtual bool Process() override;

	IApiUserBatchRequestMetadata RequestMetadata;
	FApiEventBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create mods. */
class VEAPI_API FApiCreateEventRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateEventMetadata RequestMetadata;
	FApiEventMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to update mods. */
class VEAPI_API FApiUpdateEventRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateEventMetadata RequestMetadata;
	FApiEventMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Mod batch request delegate. */
typedef TDelegate<void(const FApiEventBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnEventBatchRequestCompleted;
/** Mod entity request delegate. */
typedef TDelegate<void(const FApiEventMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnEventRequestCompleted;
