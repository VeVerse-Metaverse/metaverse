// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "Api2ServerRequest.h"

#if 0
bool FApi2GetServerRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	// todo: replace URL from APIv1 to APIv2
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetServersUrl(), *RequestEntityId.ToString(EGuidFormats::DigitsWithHyphensLower)));

	SetApiKey();
	return FApi2Request::Process();
}

bool FApi2GetServerRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			if (ResponseMetadata.FromJson(JsonObject)) {
				return true;
			}
		}
	}
	return false;
}
#endif

bool FApi2UnregisterServerRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	// todo: replace URL from APIv1 to APIv2
	HttpRequest->SetURL(Api::GetServersUrl() / RequestServerId.ToString(EGuidFormats::DigitsWithHyphensLower));

	SetApiKey();
	return FApi2Request::Process();
}

bool FApi2UnregisterServerRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	return CheckResponse();
}

bool FApi2ServerHeartbeatRequest::Process() {
#if 0 // APIv2
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/status"), *Api::GetGameServersUrl2(), *ServerId.ToString(EGuidFormats::DigitsWithHyphensLower)));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
#else // APIv1
	// todo: replace from APIv1 to APIv2
	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(
		Api::GetServersUrl() / TEXT("heartbeat") / ServerId.ToString(EGuidFormats::DigitsWithHyphensLower) + FString::Printf(
			TEXT("?status=%s"), *Api2ServerStatusToString(RequestMetadata.Status)));
	SetApiKey();
	return FApi2Request::Process();
#endif
}

bool FApi2ServerHeartbeatRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	return CheckResponse();
}

bool FApi2GetScheduledWorldRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);

#if PLATFORM_WINDOWS
	const FString PlatformName = "Win64";
#elif PLATFORM_LINUX
	const FString PlatformName = "Linux";
#else
	const FString PlatformName = "Unknown";
#endif

	// todo: replace URL from APIv1 to APIv2
	HttpRequest->SetURL(FString::Printf(TEXT("%s/scheduled?platform=%s"), *Api::GetServersUrl(), *PlatformName));

	SetApiKey();
	return FApi2Request::Process();
}

bool FApi2GetScheduledWorldRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			if (ResponseMetadata.FromJson(JsonObject)) {
				return true;
			}
		}
	}
	return false;
}

bool FApi2GetMatchingServerRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/match"), *Api::GetGameServersUrl2()));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2GetMatchingServerRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			if (ResponseMetadata.FromJson(JsonObject)) {
				return true;
			}
		}
	}
	return false;
}

bool FApi2ServerPlayerConnectRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/players"), *Api::GetGameServersUrl2(), *ServerId.ToString(EGuidFormats::DigitsWithHyphensLower)));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2ServerPlayerConnectRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	return CheckResponse();
}

bool FApi2ServerPlayerDisconnectRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/players/%s"), *Api::GetGameServersUrl2(), *ServerId.ToString(EGuidFormats::DigitsWithHyphensLower), *PlayerId.ToString(EGuidFormats::DigitsWithHyphensLower)));

	return FApi2Request::Process();
}

bool FApi2ServerPlayerDisconnectRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	return CheckResponse();
}
