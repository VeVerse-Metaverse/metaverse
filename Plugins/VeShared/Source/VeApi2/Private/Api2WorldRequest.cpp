// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2WorldRequest.h"

#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"


FApiWorldIndexRequestMetadata::FApiWorldIndexRequestMetadata() {
	Platform = FVeApi2Module::GetPlatform();
	Deployment = FVeApi2Module::GetDeployment();
}

TSharedPtr<FJsonObject> FApiWorldIndexRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchQueryRequestMetadata::ToJson();
	if (!Platform.IsEmpty()) {
		JsonObject->SetStringField(TEXT("platform"), Platform);
	}
	if (!Deployment.IsEmpty()) {
		JsonObject->SetStringField(TEXT("deployment"), Deployment);
	}
	return JsonObject;
}

FString FApiWorldIndexRequestMetadata::ToUrlParams() {
	const FString Base = IApiBatchQueryRequestMetadata::ToUrlParams();

	TArray<FString> Params;
	Params.Emplace(Base);

	if (PackageId.IsValid()) {
		Params.Emplace(TEXT("packageId=") + PackageId.ToString(EGuidFormats::DigitsWithHyphensLower));
	}
	if (!Platform.IsEmpty()) {
		Params.Emplace(TEXT("platform=") + FGenericPlatformHttp::UrlEncode(Platform));
	}
	if (!Deployment.IsEmpty()) {
		Params.Emplace(TEXT("deployment=") + FGenericPlatformHttp::UrlEncode(Deployment));
	}

	return FString::Join(Params, TEXT("&"));
}

bool FApi2WorldIndexRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/worlds?%s"), *Api::GetApi2RootUrl(), *RequestMetadata.ToUrlParams()));
	return FApi2Request::Process();
}

bool FApi2WorldIndexRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

FApi2WorldRequestMetadata::FApi2WorldRequestMetadata() {
	Platform = FVeApi2Module::GetPlatform();
	Deployment = FVeApi2Module::GetDeployment();
}

TSharedPtr<FJsonObject> FApi2WorldRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	if (!Platform.IsEmpty()) {
		JsonObject->SetStringField(TEXT("platform"), Platform);
	}
	if (!Deployment.IsEmpty()) {
		JsonObject->SetStringField(TEXT("deployment"), Deployment);
	}
	return JsonObject;
}

FString FApi2WorldRequestMetadata::ToUrlParams() {
	const FString Base = IApiMetadata::ToUrlParams();

	TArray<FString> Params;
	Params.Emplace(Base);

	// if (OwnerId.IsValid()) {
	// 	Params.Emplace(TEXT("owner-id=") + OwnerId.ToString(EGuidFormats::DigitsWithHyphensLower));
	// }
	if (!Platform.IsEmpty()) {
		Params.Emplace(TEXT("platform=") + FGenericPlatformHttp::UrlEncode(Platform));
	}
	if (!Deployment.IsEmpty()) {
		Params.Emplace(TEXT("deployment=") + FGenericPlatformHttp::UrlEncode(Deployment));
	}

	return FString::Join(Params, TEXT("&"));
}

bool FApi2WorldIndexRequestV2::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/v2"), *Api::GetWorldsUrl()));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2WorldIndexRequestV2::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2GetWorldRequestV2::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/v2"), *Api::GetWorldsUrl(),*WorldId.ToString(EGuidFormats::DigitsWithHyphensLower)));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2GetWorldRequestV2::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2WorldRequest::Process() {

	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/worlds/%s?%s"), *Api::GetApi2RootUrl(), *WorldId.ToString(EGuidFormats::DigitsWithHyphensLower), *WorldRequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2WorldRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2CreateWorldRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/worlds"), *Api::GetApi2RootUrl()));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2CreateWorldRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApi2UpdateWorldRequest::Process() {
	if (!RequestMetadata.Id.IsValid()) {
		return false;
	}

	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetWorldUrl2() / RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UpdateWorldRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApi2DeleteWorldRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/worlds/%s"), *Api::GetApi2RootUrl(), *RequestMetadataId.ToString(EGuidFormats::DigitsWithHyphensLower)));

	return FApi2Request::Process();
}

bool FApi2DeleteWorldRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApi2WorldPortalsBatchRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	// todo: replace URL from APIv1 to APIv2
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s?%s"),
		*Api::GetSpacesUrl(),
		*WorldId.ToString(EGuidFormats::DigitsWithHyphensLower),
		*Api::Portals,
		*QueryMetadata.ToUrlParams()));

	SetApiKey();
	return FApi2Request::Process();
}

bool FApi2WorldPortalsBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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
