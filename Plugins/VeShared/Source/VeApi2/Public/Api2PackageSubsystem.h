// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Api2Request.h"
#include "ApiEntityMetadata.h"
#include "ApiPackageMetadata.h"
#include "Api2PackageMetadata.h"
#include "VeShared.h"

class VEAPI2_API FApiPackageIndexRequestMetadata final : public IApiBatchQueryRequestMetadata {
public:
	FApiPackageIndexRequestMetadata();

	FString Platform = FString();
	FString Deployment = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

class VEAPI2_API FApi2PackageBatchRequest final : public TApi2BatchRequest<FApiPackageMetadata> {
public:
	virtual bool Process() override;

	FApiPackageIndexRequestMetadata RequestMetadata;
	FApiPackageBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create mods. */
class VEAPI2_API FApi2CreatePackageRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUpdatePackageMetadata RequestMetadata;
	FApiPackageMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2GetPackageRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiPackageMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2MapsPackageRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestPackageId = FGuid();
	FApi2MapArrayMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


/** Used to update mods. */
class VEAPI2_API FApi2UpdatePackageRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUpdatePackageMetadata RequestMetadata;
	FApiPackageMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


typedef TDelegate<void(const FApiPackageBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnPackageBatchRequestCompleted2;
typedef TDelegate<void(const FApiPackageMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnPackageRequestCompleted2;
typedef TDelegate<void(const FApi2MapArrayMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnMapBatchRequestCompleted2;


class VEAPI2_API FApi2PackageSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/**
	 * Requests the package metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	void Index(const FApiPackageIndexRequestMetadata& InMetadata, TSharedRef<FOnPackageBatchRequestCompleted2> InCallback) const;

	/**
	 * Requests the package metadata by the id.
	 * @param InPackageId Package ID.
	 * @param InCallback Complete callback delegate.
	 */
	void Get(const FGuid& InPackageId, TSharedRef<FOnPackageRequestCompleted2> InCallback) const;

	/**
	 * Requests the package create.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void Create(const FApiUpdatePackageMetadata& InMetadata, TSharedRef<FOnPackageRequestCompleted2> InCallback) const;

	/**
	 * Requests the package update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void Update(const FApiUpdatePackageMetadata& InMetadata, TSharedRef<FOnPackageRequestCompleted2> InCallback) const;

	/**
	 * Requests the package delete.
	 * @param InPackageId Entity ID to be deleted.
	 * @param InCallback Complete callback delegate.
	 */
	void Delete(const FGuid& InPackageId, TSharedRef<FOnGenericRequestCompleted2> InCallback) const;

	/**
	 * Requests the map metadata batch.
	 * @param InPackageId Package ID.
	 * @param InCallback Complete callback delegate.
	 */
	void GetMaps(const FGuid& InPackageId, TSharedRef<FOnMapBatchRequestCompleted2> InCallback) const;
};
