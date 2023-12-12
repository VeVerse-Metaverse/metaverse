// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.



#pragma once
#include "Api2Request.h"
#include "ApiOpenSeaMetadata.h"
#include "VeShared.h"

typedef TDelegate<void(const FApiOpenSeaAssetBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnOpenSeaBatchRequestCompleted;;
typedef TDelegate<void(const FApiOpenSeaAssetMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnOpenSeaRequestCompleted;

class VEAPI2_API FApi2OpenSeaBatchRequest final : public TApi2BatchRequest<FApiOpenSeaAssetMetadata> {
public:
	virtual bool Process() override;

	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiOpenSeaAssetBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2OpenSeaRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiOpenSeaAssetMetadata RequestMetadata;
	FApiOpenSeaAssetMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2OpenSeaSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	void GetAssets(const IApiBatchQueryRequestMetadata& InMetadata, TSharedRef<FOnOpenSeaBatchRequestCompleted> InCallback) const;
	void Get(const FGuid& InMetadata, TSharedRef<FOnOpenSeaRequestCompleted> InCallback) const;

};
