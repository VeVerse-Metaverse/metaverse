// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiRequest.h"
#include "ApiPackageMetadata.h"

/** Used to request mods in batches (with pagination). */
// class VEAPI_API FApiMetaverseBatchRequest final : public TApiBatchRequest<FApiMetaverseMetadata/*, UApiModMetadataObject*/> {
// public:
// 	virtual bool Process() override;
// 	
// 	IApiBatchQueryRequestMetadata RequestMetadata;
// 	FApiMetaverseBatchMetadata ResponseMetadata;
//
// protected:
// 	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
// };

class VEAPI_API FApiGetPackageRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiPackageMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


/** Used to request mods in batches (with pagination). */
class VEAPI_API FApiUserPackageBatchRequest final : public TApiBatchRequest<FApiPackageMetadata/*, UApiModMetadataObject*/> {
public:
	virtual bool Process() override;

	IApiUserBatchRequestMetadata RequestMetadata;
	FApiPackageBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create mods. */
// class VEAPI_API FApiCreateMetaverseRequest final : public FApiRequest {
// public:
// 	virtual bool Process() override;
//
// 	FApiUpdateMetaverseMetadata RequestMetadata;
// 	FApiMetaverseMetadata ResponseMetadata;
//
// protected:
// 	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
// };

/** Used to update mods. */
class VEAPI_API FApiUpdatePackageRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdatePackageMetadata RequestMetadata;
	FApiPackageMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Mod batch request delegate. */
typedef TDelegate<void(const FApiPackageBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnPackageBatchRequestCompleted;
/** Mod entity request delegate. */
typedef TDelegate<void(const FApiPackageMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnPackageRequestCompleted;
