// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiCommon.h"
#include "ApiEntityMetadata.h"
#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"
#include "ICallbackInterface.h"
#include "HttpRequest.h"


enum class EApiResponseCode : uint32 {
	Unknown = 0,
	Ok = 200,
	BadRequest = 400,
	Forbidden = 403,
	NotFound = 404,
	Failed = static_cast<uint32>(-1)
};

/** Generic request delegate. */
typedef TDelegate<void(const bool bInSuccessful, const FString& InError)> FOnGenericRequestCompleted;
/** Generic id request delegate. */
typedef TDelegate<void(const FGuid& InId, const bool bInSuccessful, const FString& InError)> FOnGenericIdRequestCompleted;
/** Yes-no request delegate. */
typedef TDelegate<void(const bool bInOk, const bool bInSuccessful, const FString& InError)> FOnOkRequestCompleted;


class VEAPI_API FApiRequest : public FHttpRequest {
public:
	FApiRequest();

	virtual bool Process() override;
	// virtual bool OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) override;

protected:
	virtual ~FApiRequest() override;

	virtual bool CheckResponse(const FHttpResponsePtr Response) override;
	bool GetResponseJsonObjectPayload(const FHttpResponsePtr Response, TSharedPtr<FJsonObject>& OutJsonPayload) const;
	bool GetResponseJsonArrayPayload(const FHttpResponsePtr Response, TArray<TSharedPtr<FJsonValue>>& OutJsonPayload) const;
	bool SerializeJsonPayload(const TSharedPtr<FJsonObject>& JsonObject, TArray<uint8>& OutBytes) const;

	EApiResponseCode GetApiResponseCode(const FHttpResponsePtr& Response) const;
};

template <typename T/*, typename U*/>
class VEAPI_API TApiBatchRequest : public FApiRequest {
public:
	virtual bool Process() override {
		HttpRequest->SetVerb(Api::HttpMethods::Get);
		return FApiRequest::Process();
	}

	IApiBatchRequestMetadata RequestMetadata;
	TApiBatchMetadata<T/*, U*/> ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override {
		bool bOk = false;

		if (CheckResponse(Response)) {
			TSharedPtr<FJsonObject> JsonPayload;
			if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
				if (ResponseMetadata.FromJson(JsonPayload)) {
					bOk = true;
				}
			}
		}

		return bOk;
	}
};

/** Used to delete entities. */
class VEAPI_API FApiDeleteEntityRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid Id;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

#define API_ENTITY_REQUEST_ON_COMPLETE_BODY() \
if (CheckResponse(Response)) { \
	TSharedPtr<FJsonObject> JsonPayload; \
	if (GetResponseJsonObjectPayload(Response, JsonPayload)) { \
		if (ResponseMetadata.FromJson(JsonPayload)) { \
			return true; \
		} \
	} \
} \
return false;
