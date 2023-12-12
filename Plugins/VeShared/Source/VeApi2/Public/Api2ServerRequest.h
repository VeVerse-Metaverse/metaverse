// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2Request.h"
#include "Api2ServerMetadata.h"
#include "Api2WorldMetadata.h"

#if 0
/** Used to request a single server metadata by its id. */
class VEAPI2_API FApi2GetServerRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiServerMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};
#endif

/** Used to create servers. */
class VEAPI2_API FApi2UnregisterServerRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestServerId;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to report player connections to servers. */
class VEAPI2_API FApi2ServerHeartbeatRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid ServerId;
	FApi2RequestServerHeartbeatMetadata RequestMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to request a scheduled world. */
class VEAPI2_API FApi2GetScheduledWorldRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiSpaceMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to request a single server metadata by its id. */
class VEAPI2_API FApi2GetMatchingServerRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2RequestServerMatchMetadata RequestMetadata;
	FApi2ServerMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to report that server is online and players can connect. */
class VEAPI2_API FApi2ServerPlayerConnectRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid ServerId;
	FApi2RequestPlayerConnectMetadata RequestMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to report player disconnections from servers. */
class VEAPI2_API FApi2ServerPlayerDisconnectRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid ServerId;
	FGuid PlayerId;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


/** Server batch request delegate. */
// todo: FApiServerBatchMetadata -> FApi2ServerBatchMetadata
typedef TDelegate<void(const FApiServerBatchMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError)> FOnServerBatchRequestCompleted2;
/** Server entity request delegate. */
typedef TDelegate<void(const FApi2ServerMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError)> FOnServerRequestCompleted2;
