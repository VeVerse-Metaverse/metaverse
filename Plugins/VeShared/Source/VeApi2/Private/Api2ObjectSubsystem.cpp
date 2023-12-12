// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2ObjectSubsystem.h"
#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FName FApi2ObjectSubsystem::Name = TEXT("Api2ObjectSubsystem");

void FApi2ObjectSubsystem::Initialize() {
}

void FApi2ObjectSubsystem::Shutdown() {}

FApiObjectIndexRequestMetadata::FApiObjectIndexRequestMetadata() {
	Platform = FVeApi2Module::GetPlatform();
	Deployment = FVeApi2Module::GetDeployment();
}

TSharedPtr<FJsonObject> FApiObjectIndexRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchQueryRequestMetadata::ToJson();
	if (!Platform.IsEmpty()) {
		JsonObject->SetStringField(TEXT("platform"), Platform);
	}
	if (!Deployment.IsEmpty()) {
		JsonObject->SetStringField(TEXT("deployment"), Deployment);
	}
	return JsonObject;
}

FString FApiObjectIndexRequestMetadata::ToUrlParams() {
	const FString Base = IApiBatchQueryRequestMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s&%s=%s"), *Base, TEXT("platform"), *FGenericPlatformHttp::UrlEncode(Platform), TEXT("deployment"), *FGenericPlatformHttp::UrlEncode(Deployment));
}

FApi2ObjectRequestMetadata::FApi2ObjectRequestMetadata() {
	Platform = FVeApi2Module::GetPlatform();
	Deployment = FVeApi2Module::GetDeployment();
}

TSharedPtr<FJsonObject> FApi2ObjectRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	if (!Platform.IsEmpty()) {
		JsonObject->SetStringField(TEXT("platform"), Platform);
	}
	if (!Deployment.IsEmpty()) {
		JsonObject->SetStringField(TEXT("deployment"), Deployment);
	}
	return JsonObject;
}

FString FApi2ObjectRequestMetadata::ToUrlParams() {
	const FString Base = IApiMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s&%s=%s"), *Base, TEXT("platform"), *FGenericPlatformHttp::UrlEncode(Platform), TEXT("deployment"), *FGenericPlatformHttp::UrlEncode(Deployment));
}

bool FApi2ObjectIndexRequest::Process() {
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *Api::GetArtObjectUrl(), *RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2ObjectIndexRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2ObjectRequest::Process() {

	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s?%s"), *Api::GetArtObjectUrl(), *ObjectId.ToString(EGuidFormats::DigitsWithHyphensLower), *ObjectRequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2ObjectRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2CreateObjectRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(Api::GetArtObjectUrl());
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2CreateObjectRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApi2UpdateObjectRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetArtObjectUrl() / RequestMetadata.Id.ToString(EGuidFormats::Digits));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UpdateObjectRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApi2DeleteObjectRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(Api::GetEntitiesUrl() / RequestMetadataId.ToString(EGuidFormats::DigitsWithHyphensLower));

	return FApi2Request::Process();
}

bool FApi2DeleteObjectRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

void FApi2ObjectSubsystem::Index(const IApiBatchQueryRequestMetadata& InMetadata, TSharedRef<FOnObjectBatchRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2ObjectIndexRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2ObjectSubsystem::Get(const FGuid& Id, TSharedRef<FOnObjectRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2ObjectRequest>();

	Request->ObjectId = Id;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2ObjectSubsystem::Create(const FApiUpdateObjectMetadata& InMetadata, TSharedRef<FOnObjectRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2CreateObjectRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2ObjectSubsystem::Update(const FApiUpdateObjectMetadata& InMetadata, TSharedRef<FOnObjectRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UpdateObjectRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2ObjectSubsystem::Delete(const FGuid& InId, TSharedRef<FOnObjectRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2DeleteObjectRequest>();

	Request->RequestMetadataId = InId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
