// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2AnalyticsSubsystem.h"

#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FName FApi2AnalyticsSubsystem::Name = TEXT("Api2AnalyticsSubsystem");

FApi2AnalyticsRequestMetadata::FApi2AnalyticsRequestMetadata() {}

TSharedPtr<FJsonObject> FApi2AnalyticsRequestMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::AnalyticsType::AppId, AppId.ToString(EGuidFormats::DigitsWithHyphensLower));
	JsonObject->SetStringField(Api::AnalyticsType::ContextEntityId, ContextEntityId.ToString(EGuidFormats::DigitsWithHyphensLower));
	JsonObject->SetStringField(Api::AnalyticsType::ContextEntityType, ContextEntityType);
	JsonObject->SetStringField(Api::Fields::UserId, UserId.ToString(EGuidFormats::DigitsWithHyphensLower));
	JsonObject->SetStringField(Api::Fields::Platform, FVeApi2Module::GetPlatform());
	JsonObject->SetStringField(Api::Fields::Deployment, FVeApi2Module::GetDeployment());
	JsonObject->SetStringField(Api::Fields::Configuration, Configuration);
	JsonObject->SetStringField(Api::AnalyticsType::Event, Event);


	const auto JsonReader = TJsonReaderFactory<>::Create(Payload);
	TSharedPtr<FJsonObject> JsonPayloadObject;
	if (FJsonSerializer::Deserialize(JsonReader, JsonPayloadObject) && JsonPayloadObject.IsValid()) {
		JsonObject->SetObjectField(Api::Fields::Payload, JsonPayloadObject);
	}
	return JsonObject;
}

FString FApi2AnalyticsRequestMetadata::ToUrlParams() {
	const FString Base = IApiMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s&%s=%s"), *Base, TEXT("platform"), *FGenericPlatformHttp::UrlEncode(Platform),TEXT("deployment"), *FGenericPlatformHttp::UrlEncode(Deployment));
}

FApi2AnalyticsBatchRequestMetadata::FApi2AnalyticsBatchRequestMetadata() {}

TSharedPtr<FJsonObject> FApi2AnalyticsBatchRequestMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApi2BatchRequestMetadata::ToJson();
	JsonObject->SetStringField(Api::AnalyticsType::AppId, AppId.ToString(EGuidFormats::DigitsWithHyphensLower));
	JsonObject->SetStringField(Api::AnalyticsType::ContextEntityId, ContextEntityId.ToString(EGuidFormats::DigitsWithHyphensLower));
	JsonObject->SetStringField(Api::AnalyticsType::ContextEntityType, ContextEntityType);
	JsonObject->SetStringField(Api::Fields::UserId, UserId.ToString(EGuidFormats::DigitsWithHyphensLower));
	JsonObject->SetStringField(Api::Fields::Platform, FVeApi2Module::GetPlatform());
	JsonObject->SetStringField(Api::Fields::Deployment, FVeApi2Module::GetDeployment());
	JsonObject->SetStringField(Api::Fields::Configuration, Configuration);
	JsonObject->SetStringField(Api::AnalyticsType::Event, Event);
	return JsonObject;
}

FString FApi2AnalyticsBatchRequestMetadata::ToUrlParams() {
	const FString Base = IApi2BatchRequestMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s"), *Base,
		TEXT("contextEntityId"), *FGenericPlatformHttp::UrlEncode(ContextEntityId.ToString(EGuidFormats::DigitsWithHyphensLower)),
		TEXT("contextEntityType"), *FGenericPlatformHttp::UrlEncode(ContextEntityType),
		TEXT("userId"), *FGenericPlatformHttp::UrlEncode(UserId.ToString(EGuidFormats::DigitsWithHyphensLower)),
		TEXT("platform"), *FGenericPlatformHttp::UrlEncode(Platform),
		TEXT("deployment"), *FGenericPlatformHttp::UrlEncode(Deployment),
		TEXT("configuration"), *FGenericPlatformHttp::UrlEncode(Configuration),
		TEXT("event"), *FGenericPlatformHttp::UrlEncode(Event));
}

FApi2AnalyticsMetadata::FApi2AnalyticsMetadata() {}

bool FApi2AnalyticsMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	const bool bOk = IApiMetadata::FromJson(InJsonObject);

	Id = GetJsonGuidField(InJsonObject, Api::Fields::Id);
	Timestamp = GetJsonDateTimeField(InJsonObject, Api::Fields::Timestamp);
	AppId = GetJsonGuidField(InJsonObject, Api::AnalyticsType::AppId);
	ContextEntityId = GetJsonGuidField(InJsonObject, Api::AnalyticsType::ContextEntityId);
	ContextEntityType = GetJsonStringField(InJsonObject, Api::AnalyticsType::ContextEntityType);
	UserId = GetJsonGuidField(InJsonObject, Api::Fields::UserId);
	Platform = GetJsonStringField(InJsonObject, Api::Fields::Platform);
	Deployment = GetJsonStringField(InJsonObject, Api::Fields::Deployment);
	Configuration = GetJsonStringField(InJsonObject, Api::Fields::Configuration);
	Event = GetJsonStringField(InJsonObject, Api::AnalyticsType::Event);
	Payload = GetJsonStringField(InJsonObject, Api::Fields::Payload);

	return bOk;
}

bool FApi2AnalyticsBatchRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *Api::GetAnalyticsUrl(), *RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2AnalyticsBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2AnalyticsRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(FString::Printf(TEXT("%s"), *Api::GetAnalyticsUrl()));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}


bool FApi2AnalyticsRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

void FApi2AnalyticsSubsystem::Initialize() {}
void FApi2AnalyticsSubsystem::Shutdown() {}

void FApi2AnalyticsSubsystem::IndexEvents(FApi2AnalyticsBatchRequestMetadata RequestMetadata, TSharedRef<FOnAnalyticsBatchRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2AnalyticsBatchRequest>();
	Request->RequestMetadata = RequestMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2AnalyticsSubsystem::ReportEvent(FApi2AnalyticsRequestMetadata RequestMetadata, TSharedRef<FOnAnalyticsRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2AnalyticsRequest>();
	Request->RequestMetadata = RequestMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
