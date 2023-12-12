// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiRequest.h"
#include "ApiServerMetadata.h"

/** Used to request servers in batches (with pagination). */
class VEAPI_API FApiServerBatchRequest final : public TApiBatchRequest<FApiServerMetadata/*, UApiServerMetadataObject*/> {
public:
	virtual bool Process() override;

	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiServerBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

#if 0
/** Used to request a single server metadata by its id. */
class VEAPI_API FApiGetServerRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiServerMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};
#endif

#if 1
/** Used to request a single server metadata by its id. */
class VEAPI_API FApiGetMatchingServerRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestSpaceId = FGuid();
	FApiServerMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};
#endif

/** Used to request servers in batches (with pagination). */
class VEAPI_API FApiUserServerBatchRequest final : public TApiBatchRequest<FApiServerMetadata/*, UApiServerMetadataObject*/> {
public:
	virtual bool Process() override;

	IApiUserBatchRequestMetadata RequestMetadata;
	FApiServerBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create servers. */
class VEAPI_API FApiRegisterServerRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateServerMetadata RequestMetadata;
	FApiServerMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create servers. */
class VEAPI_API FApiUpdateServerRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateServerMetadata RequestMetadata;
	FApiServerMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

#if 0
/** Used to create servers. */
class VEAPI_API FApiUnregisterServerRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestServerId;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};
#endif

#if 0
/** Used to report player connections to servers. */
class VEAPI_API FApiServerHeartbeatRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestServerId;
	FString RequestStatus;
	FString RequestDetails;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};
#endif

/** Used to report that server is online and players can connect. */
class VEAPI_API FApiServerPlayerConnectRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestServerId;
	FGuid RequestUserId;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to report player disconnections from servers. */
class VEAPI_API FApiServerPlayerDisconnectRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestServerId;
	FGuid RequestUserId;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Server batch request delegate. */
typedef TDelegate<void(const FApiServerBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnServerBatchRequestCompleted;
/** Server entity request delegate. */
typedef TDelegate<void(const FApiServerMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnServerRequestCompleted;
