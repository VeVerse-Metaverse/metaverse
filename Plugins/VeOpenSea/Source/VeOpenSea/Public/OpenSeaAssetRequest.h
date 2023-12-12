// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiRequest.h"
#include "OpenSeaAssetMetadata.h"

class VEOPENSEA_API FOpenSeaRequest : public FHttpRequest {
public:
	virtual bool Process() override;
};

/** Used to request objects in batches (with pagination). */
class VEOPENSEA_API FOpenSeaAssetBatchRequest final : public FOpenSeaRequest {
public:
	virtual bool Process() override;

	FOpenSeaAssetBatchRequestMetadata RequestMetadata;
	FOpenSeaAssetBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEOPENSEA_API FOpenSeaAssetGetRequest final : public FOpenSeaRequest {
public:
	virtual bool Process() override;

	/** FOpenSeaAssetMetadata.TokenId */
	FString TokenId{};

	/** FOpenSeaAssetMetadata.AssetContract.Address */
	FString ContractAddress{};
	
	FOpenSeaAssetMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Object batch request delegate. */
typedef TDelegate<void(const FOpenSeaAssetBatchMetadata& /*InMetadata*/, bool /*bInSuccessful*/, const FString& /*InError*/)> FOnOpenSeaAssetBatchRequestCompleted;
/** Object entity request delegate. */
typedef TDelegate<void(const FOpenSeaAssetMetadata& /*InMetadata*/, bool /*bInSuccessful*/, const FString& /*InError*/)> FOnOpenSeaAssetRequestCompleted;
