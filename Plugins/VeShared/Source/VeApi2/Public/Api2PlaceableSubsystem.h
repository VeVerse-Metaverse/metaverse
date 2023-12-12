// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "Api2Request.h"
#include "ApiEntityMetadata.h"
#include "ApiPlaceableRequest.h"
#include "VeShared.h"

/** Placeable batch request delegate. */
typedef TDelegate<void(const FApiPlaceableBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnPlaceableBatchRequestCompleted2;

/** Placeable request delegate. */
typedef TDelegate<void(const FApiPlaceableMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnPlaceableRequestCompleted2;

typedef TDelegate<void(const FApiPlaceableClassBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnPlaceableClassBatchRequestCompleted2;



class VEAPI2_API FApi2PlaceableBatchRequestMetadata final : public IApiBatchRequestMetadata {
public:
	FApi2PlaceableBatchRequestMetadata();

	FGuid WorldId = FGuid{};

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

/** Base interface for batched metadata requests with string query. */
class VEAPI2_API FApi2PlaceableClassBatchQueryRequestMetadata : public IApiBatchQueryRequestMetadata {
public:
	FString Category = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

/** Used to request space placeables in batches (with pagination). */
class VEAPI2_API FApi2GetPlaceableBatchClassRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2PlaceableClassBatchQueryRequestMetadata RequestMetadata;
	FApiPlaceableClassBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2PlaceableBatchRequest final : public TApi2BatchRequest<FApiPlaceableMetadata> {
public:
	virtual bool Process() override;

	FApi2PlaceableBatchRequestMetadata RequestMetadata;
	FApiPlaceableBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/**
 * Request for the Placeable get route
 */
class VEAPI2_API FApi2PlaceableRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid PlaceableId = FGuid{};
	FApiPlaceableMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to update space placeable. */
class VEAPI2_API FApi2UpdateWorldPlaceableRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid Id;
	FApiPlaceableMetadata RequestMetadata;
	FApiPlaceableMetadata ResponseMetadata;
	EApiResponseCode ResponseCode = EApiResponseCode::Unknown;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2UpdatePlaceableTransformRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUpdatePlaceableTransformMetadata RequestMetadata;
	FApiPlaceableMetadata ResponseMetadata;
	EApiResponseCode ResponseCode = EApiResponseCode::Unknown;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2UpdatePlaceableEntityRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestPlaceableId;
	FGuid RequestEntityId;
	FApiPlaceableMetadata ResponseMetadata;
	EApiResponseCode ResponseCode = EApiResponseCode::Unknown;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to get space placeable. */
class VEAPI2_API FApi2DeletePlaceableRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid Id;
	FApiSpaceMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

// typedef TDelegate<void(const FApiPlaceableBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnPlaceableBatchRequestCompleted;;

class VEAPI2_API FApi2PlaceableSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	void Index(const FApi2PlaceableBatchRequestMetadata& InMetadata, TSharedRef<FOnPlaceableBatchRequestCompleted2> InCallback) const;

	void Get(const FGuid& Id, TSharedRef<FOnPlaceableRequestCompleted2> InCallback) const;

	void GetPlaceableClassBatch(const FApi2PlaceableClassBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnPlaceableClassBatchRequestCompleted2> InCallback);

	/** Create or update a placeable. */
	void UpdatePlaceable(const FGuid& InSpaceId, const FApiPlaceableMetadata& InPlaceableMetadata, TSharedPtr<FOnPlaceableRequestCompleted2> InCallback);

	/** Update placeable transform. */
	void UpdatePlaceableTransform(const FApiUpdatePlaceableTransformMetadata& InPlaceableMetadata, TSharedPtr<FOnPlaceableRequestCompleted2> InCallback);

	/** Update placeable entity. */
	void UpdatePlaceableEntity(const FGuid& InPlaceableId, const FGuid& InEntityId, TSharedRef<FOnPlaceableRequestCompleted2> InCallback);

	/** Delete placeable from the space. */
	void DeletePlaceable(const FGuid& InPlaceableId, TSharedPtr<FOnGenericRequestCompleted2> InCallback);

};
