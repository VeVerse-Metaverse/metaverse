// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiServerRequest.h"

bool FApiServerBatchRequest::Process() {
	return TApiBatchRequest<FApiServerMetadata>::Process();
}

bool FApiServerBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

#if 0
bool FApiGetServerRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetServersUrl(), *RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens)));

	return FApiRequest::Process();
}

bool FApiGetServerRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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
#endif

#if 1
bool FApiGetMatchingServerRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/match/%s"), *Api::GetServersUrl(), *RequestSpaceId.ToString(EGuidFormats::DigitsWithHyphens)));

	return FApiRequest::Process();
}

bool FApiGetMatchingServerRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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
#endif

bool FApiUserServerBatchRequest::Process() {
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *Api::GetServersUrl(), *RequestMetadata.ToUrlParams()));
	return TApiBatchRequest<FApiServerMetadata>::Process();
}

bool FApiUserServerBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiRegisterServerRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(Api::GetServersUrl());
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiRegisterServerRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiUpdateServerRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetServersUrl(), *RequestMetadata.Id.ToString()));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiUpdateServerRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

#if 0
bool FApiUnregisterServerRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(Api::GetServersUrl() / RequestServerId.ToString(EGuidFormats::DigitsWithHyphens));
	return FApiRequest::Process();
}

bool FApiUnregisterServerRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	const bool bOk = CheckResponse(Response);
	return bOk;
	FApiRequest::OnComplete(Request, Response, bSuccessful);
}
#endif

#if 0
bool FApiServerHeartbeatRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(
		Api::GetServersUrl() / TEXT("heartbeat") / RequestServerId.ToString(EGuidFormats::DigitsWithHyphens) + FString::Printf(
			TEXT("?status=%s&=details=%s"), *RequestStatus, *RequestDetails));
	return FApiRequest::Process();
}

bool FApiServerHeartbeatRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	const bool bOk = CheckResponse(Response);
	return bOk;
}
#endif

bool FApiServerPlayerConnectRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(Api::GetServersUrl() / RequestServerId.ToString(EGuidFormats::DigitsWithHyphens) / TEXT("connect") / RequestUserId.ToString(EGuidFormats::DigitsWithHyphens));
	return FApiRequest::Process();
}

bool FApiServerPlayerConnectRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	const bool bOk = CheckResponse(Response);
	return bOk;
}

bool FApiServerPlayerDisconnectRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(Api::GetServersUrl() / RequestServerId.ToString(EGuidFormats::DigitsWithHyphens) / TEXT("connect") / RequestUserId.ToString(EGuidFormats::DigitsWithHyphens));
	return FApiRequest::Process();
}

bool FApiServerPlayerDisconnectRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	const bool bOk = CheckResponse(Response);
	return bOk;
}
