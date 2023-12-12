// Author: Egor A. Pristavka & Khusan T. Yadgarov

#pragma once
#include "ApiPlaceableMetadata.h"
#include "ApiRequest.h"
#include "ApiSpaceMetadata.h"
#include "ApiSpaceMetadataObject.h"


/** Used to update space placeable. */
class VEAPI_API FApiUpdateSpacePlaceableRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid Id;
	FApiPlaceableMetadata RequestMetadata;
	FApiPlaceableMetadata ResponsePlaceableMetadata;
	EApiResponseCode ResponseCode = EApiResponseCode::Unknown;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


/** Used to request space placeables in batches (with pagination). */
class VEAPI_API FApiPlaceableBatchRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid SpaceId;
	IApiBatchRequestMetadata RequestMetadata;
	FApiPlaceableBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to request space placeables in batches (with pagination). */
class VEAPI_API FApiGetPlaceableRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestEntityId;
	FApiPlaceableMetadata ResponseMetadata;
	EApiResponseCode ResponseCode = EApiResponseCode::Unknown;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUpdatePlaceableTransformRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdatePlaceableTransformMetadata RequestMetadata;
	FApiPlaceableMetadata ResponseMetadata;
	EApiResponseCode ResponseCode = EApiResponseCode::Unknown;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUpdatePlaceableEntityRequest final : public FApiRequest {
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
class VEAPI_API FApiDeletePlaceableRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid Id;
	FApiSpaceMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to request space placeables in batches (with pagination). */
class VEAPI_API FApiGetPlaceableBatchClassRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiPlaceableClassBatchQueryRequestMetadata RequestMetadata;
	FApiPlaceableClassBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Placeable batch request delegate. */
typedef TDelegate<void(const FApiPlaceableBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnPlaceableBatchRequestCompleted;
/** Placeable request delegate. */
typedef TDelegate<void(const FApiPlaceableMetadata& /*InMetadata*/, const EApiResponseCode /*InResponseCode*/, const FString& /*InError*/)> FOnPlaceableRequestCompleted;
/** PlaceableClass request delegate. */
typedef TDelegate<void(const FApiPlaceableClassMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnPlaceableClassRequestCompleted;

typedef TDelegate<void(const FApiPlaceableClassBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnPlaceableClassBatchRequestCompleted;
