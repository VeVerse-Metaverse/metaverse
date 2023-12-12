// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "Api2Request.h"
#include "ApiEntityMetadata.h"
#include "ApiObjectMetadata.h"
#include "VeShared.h"

/**
 * Metadata for the Object index request
 */
class VEAPI2_API FApiObjectIndexRequestMetadata final : public IApiBatchQueryRequestMetadata {
public:
	FApiObjectIndexRequestMetadata();

	FString Platform = FString();
	FString Deployment = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

class VEAPI2_API FApi2ObjectRequestMetadata final : public IApiMetadata {
public:
	FApi2ObjectRequestMetadata();

	FString Platform = FString();
	FString Deployment = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

/**
 * Request for the Object index route
 */
class VEAPI2_API FApi2ObjectIndexRequest final : public TApi2BatchRequest<FApiObjectMetadata> {
public:
	virtual bool Process() override;

	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiObjectBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/**
 * Request for the Object index route
 */
class VEAPI2_API FApi2ObjectRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid ObjectId = FGuid{};
	FApi2ObjectRequestMetadata ObjectRequestMetadata;
	FApiObjectMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create Objects. */
class VEAPI2_API FApi2CreateObjectRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUpdateObjectMetadata RequestMetadata;
	FApiObjectMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create Objects. */
class VEAPI2_API FApi2UpdateObjectRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUpdateObjectMetadata RequestMetadata;
	FApiObjectMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create Objects. */
class VEAPI2_API FApi2DeleteObjectRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestMetadataId;
	FApiObjectMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


typedef TDelegate<void(const FApiObjectBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnObjectBatchRequestCompleted2;
typedef TDelegate<void(const FApiObjectMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnObjectRequestCompleted2;


class VEAPI2_API FApi2ObjectSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/**
	 * Requests the Object metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	void Index(const IApiBatchQueryRequestMetadata& InMetadata, TSharedRef<FOnObjectBatchRequestCompleted2> InCallback) const;

	/**
	 * Requests the Object metadata by the id.
	 * @param Id Object ID.
	 * @param InCallback Complete callback delegate.
	 */
	void Get(const FGuid& Id, TSharedRef<FOnObjectRequestCompleted2> InCallback) const;

	/**
	 * Requests the Object create.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void Create(const FApiUpdateObjectMetadata& InMetadata, TSharedRef<FOnObjectRequestCompleted2> InCallback) const;

	void Update(const FApiUpdateObjectMetadata& InMetadata, TSharedRef<FOnObjectRequestCompleted2> InCallback) const;

	void Delete(const FGuid& InId, TSharedRef<FOnObjectRequestCompleted2> InCallback) const;

};

typedef TSharedPtr<FApi2ObjectSubsystem> FApi2ObjectSubsystemPtr;
