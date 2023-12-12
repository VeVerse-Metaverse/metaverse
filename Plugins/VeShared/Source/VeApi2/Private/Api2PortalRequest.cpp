// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2PortalRequest.h"

#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"


FApiPortalIndexRequestMetadata::FApiPortalIndexRequestMetadata() {
	Platform = FVeApi2Module::GetPlatform();
	Deployment = FVeApi2Module::GetDeployment();
}

TSharedPtr<FJsonObject> FApiPortalIndexRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchQueryRequestMetadata::ToJson();
	if (!Platform.IsEmpty()) {
		JsonObject->SetStringField(TEXT("platform"), Platform);
	}
	if (!Deployment.IsEmpty()) {
		JsonObject->SetStringField(TEXT("deployment"), Deployment);
	}
	return JsonObject;
}

FString FApiPortalIndexRequestMetadata::ToUrlParams() {
	const FString Base = IApiBatchQueryRequestMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s&%s=%s"), *Base, TEXT("platform"), *FGenericPlatformHttp::UrlEncode(Platform), TEXT("deployment"), *FGenericPlatformHttp::UrlEncode(Deployment));
}

FApiPortalRequestMetadata::FApiPortalRequestMetadata() {
	Platform = FVeApi2Module::GetPlatform();
	Deployment = FVeApi2Module::GetDeployment();
}

TSharedPtr<FJsonObject> FApiPortalRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(TEXT("id"), Id.ToString());
	if (!Platform.IsEmpty()) {
		JsonObject->SetStringField(TEXT("platform"), Platform);
	}
	if (!Deployment.IsEmpty()) {
		JsonObject->SetStringField(TEXT("deployment"), Deployment);
	}
	return JsonObject;
}

FString FApiPortalRequestMetadata::ToUrlParams() {
	const FString Base = IApiMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s&%s=%s"), *Base, TEXT("platform"), *FGenericPlatformHttp::UrlEncode(Platform), TEXT("deployment"), *FGenericPlatformHttp::UrlEncode(Deployment));
}

bool FApi2PortalBatchRequest::Process() {
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s?%s"), *Api::GetApi2RootUrl(), TEXT("portals"), *RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2PortalBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2PortalRequest::Process() {
	if (!RequestMetadata.Id.IsValid()) {
		VeLogErrorFunc("invalid id");
		return false;
	}

	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s?%s"), *Api::GetApi2RootUrl(), TEXT("portals"), *RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower), *RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2PortalRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2CreatePortalRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetApi2RootUrl(), TEXT("portals")));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2CreatePortalRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			if (ResponseMetadata.FromJson(JsonObject)) {
				return true;
			}
		}
	}
	return false;;
}
