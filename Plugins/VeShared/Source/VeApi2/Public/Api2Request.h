// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2EntityMetadata.h"
#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"
#include "HttpRequest.h"

enum class EApi2ResponseCode : uint32 {
	Unknown = 0,
	Ok = 200,
	BadRequest = 400,
	Forbidden = 403,
	NotFound = 404,
	Failed = static_cast<uint32>(-1)
};

/** Generic request delegate. */
typedef TDelegate<void(const EApi2ResponseCode InResponseCode, const FString& InError)> FOnGenericRequestCompleted2;
typedef TDelegate<void(const bool bInOk, EApi2ResponseCode InResponseCode, const FString& InError)> FOnOkRequestCompleted2;


class VEAPI2_API FApi2Request : public FHttpRequest {
public:
	FApi2Request();

	virtual bool Process() override;
	// virtual bool OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) override;
	EApi2ResponseCode GetApiResponseCode() const;

protected:
	virtual ~FApi2Request() override;

	/** Old version authorization. */
	void SetApiKey() const;
	
	virtual bool CheckResponse() override;

	static bool GetResponseStringPayload(const FHttpResponsePtr Response, FString& OutStringPayload);
	static bool GetResponseJsonObjectPayload(const FHttpResponsePtr Response, TSharedPtr<FJsonObject>& OutJsonPayload);
	static bool GetResponseJsonArrayPayload(const FHttpResponsePtr Response, TArray<TSharedPtr<FJsonValue>>& OutJsonPayload);
	static bool SerializeJsonPayload(const TSharedPtr<FJsonObject>& JsonObject, TArray<uint8>& OutBytes);

};

class VEAPI2_API FApi2StringRequest : public FApi2Request {
	FString RequestBody;
	FString ResponseBody;

public:
	void SetRequestBody(const FString& InBody);
	FString GetResponseBody() const;
	void SetUrl(const FString& InUrl) const;
	void SetVerb(const FString& InVerb) const;

	virtual bool Process() override;
	virtual bool OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) override;
};

typedef TDelegate<void(const FString& InString, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnStringRequestCompleted2;

template <typename T>
class VEAPI2_API TApi2BatchRequest : public FApi2Request {
public:
	virtual bool Process() override {
		HttpRequest->SetVerb(Api::HttpMethods::Get);
		return FApi2Request::Process();
	}

	IApi2BatchRequestMetadata RequestMetadata;
	TApi2BatchMetadata<T> ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override {
		bool bOk = false;

		if (CheckResponse()) {
			TSharedPtr<FJsonObject> JsonPayload;
			if (GetResponseJsonObjectPayload(GetResponse(), JsonPayload)) {
				if (ResponseMetadata.FromJson(JsonPayload)) {
					bOk = true;
				}
			}
		}

		return bOk;
	}
};

/** Used to delete entities. */
class VEAPI2_API FApi2DeleteEntityRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid Id;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) override;
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
