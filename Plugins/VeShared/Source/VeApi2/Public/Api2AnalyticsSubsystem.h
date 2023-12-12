// Authors: Khusan.Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Api2Request.h"
#include "ApiAnalyticsMetadata.h"
#include "ApiEntityMetadata.h"
#include "VeShared.h"

/** Analytics request delegate. */
//typedef TDelegate<void(const FApiAnalyticsMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnAnalyticsRequestCompleted2;

class VEAPI2_API FApi2AnalyticsRequestMetadata final : public FApiAnalyticsMetadata {
public:
	FApi2AnalyticsRequestMetadata();

	FString Platform = FString();
	FString Deployment = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

typedef TDelegate<void(const FApi2AnalyticsRequestMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnAnalyticsRequestCompleted2;

class VEAPI2_API FApi2AnalyticsBatchRequestMetadata final : public IApi2BatchRequestMetadata {
public:
	FApi2AnalyticsBatchRequestMetadata();

	int32 Offset = 0;
	int32 Limit = 20;

	FGuid AppId = {};
	FGuid ContextEntityId = {};
	FString ContextEntityType = {};
	FGuid UserId = {};
	FString Platform = {};
	FString Deployment = {};
	FString Configuration = {};
	FString Event = {};

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

class VEAPI2_API FApi2AnalyticsMetadata final : public IApiMetadata {
public:
	FApi2AnalyticsMetadata();

	/**
	 * @brief ID of the event.
	 */
	FGuid Id = {};

	/**
	 * @brief Timestamp of the event.
	 */
	FDateTime Timestamp = {};

	/**
	 * @brief ID of the application.
	 */
	FGuid AppId = {};

	/**
	 * @brief ID of the entity that is the context of the event.
	 */
	FGuid ContextEntityId = {};

	/**
	 * @brief Type of the entity that is the context of the event (world, user, etc.).
	 */
	FString ContextEntityType = {};

	/**
	 * @brief ID of the user that produced the event. 
	 */
	FGuid UserId = {};

	/**
	 * @brief Platform of the app that produced the event.
	 */
	FString Platform = {};

	/**
	 * @brief Deployment of the app that produced the event.
	 */
	FString Deployment = {};

	/**
	 * @brief Configuration of the app that produced the event.
	 */
	FString Configuration = {};

	/**
	 * @brief Event name.
	 */
	FString Event = {};

	/**
	 * @brief Event payload (usually a JSON object).
	 */
	FString Payload = {};

	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};

typedef TApi2BatchMetadata<FApi2AnalyticsMetadata> FApi2AnalyticsBatchResponseMetadata;

/** Analytics batch request delegate. */
typedef TDelegate<void(const FApi2AnalyticsBatchResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnAnalyticsBatchRequestCompleted2;

class VEAPI2_API FApi2AnalyticsBatchRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2AnalyticsBatchRequestMetadata RequestMetadata;
	FApi2AnalyticsBatchResponseMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/**
 * Request for the Analytics get route
 */
class VEAPI2_API FApi2AnalyticsRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2AnalyticsRequestMetadata RequestMetadata;
	FApi2AnalyticsRequestMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

// typedef TDelegate<void(const FApiAnalyticsBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnAnalyticsBatchRequestCompleted;;

class VEAPI2_API FApi2AnalyticsSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	void IndexEvents(FApi2AnalyticsBatchRequestMetadata RequestMetadata, TSharedRef<FOnAnalyticsBatchRequestCompleted2> InCallback) const;
	void ReportEvent(FApi2AnalyticsRequestMetadata RequestMetadata, TSharedRef<FOnAnalyticsRequestCompleted2> InCallback) const;

};
