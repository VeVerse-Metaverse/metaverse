// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2SharedAccessSubsystem.h"


FName FApi2SharedAccessSubsystem::Name = TEXT("Api2SharedAccessSubsystem");

FApi2SharedAccessBatchRequestMetadata::FApi2SharedAccessBatchRequestMetadata() {}

TSharedPtr<FJsonObject> FApi2SharedAccessBatchRequestMetadata::ToJson() {
	return IApiBatchQueryRequestMetadata::ToJson();
}

bool FApi2SharedAccessRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetEntitiesUrl2(), *RequestMetadata.EntityId.ToString(EGuidFormats::DigitsWithHyphensLower), TEXT("access")));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2SharedAccessRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2SharedPublicRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetEntitiesUrl2(), *RequestMetadata.EntityId.ToString(EGuidFormats::DigitsWithHyphensLower), TEXT("public")));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2SharedPublicRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2SharedAccessIndexRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();
	
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s?%s"), *Api::GetEntitiesUrl2(), *RequestMetadata.EntityId.ToString(EGuidFormats::DigitsWithHyphensLower), TEXT("access"), *RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2SharedAccessIndexRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

void FApi2SharedAccessSubsystem::Initialize() {}

void FApi2SharedAccessSubsystem::Shutdown() {}

void FApi2SharedAccessSubsystem::UpdateSharedAccess(const FApi2SharedAccessMetadata& InMetadata, TSharedRef<FOnSharedAccessRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2SharedAccessRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		auto _ = InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2SharedAccessSubsystem::UpdatePublicAccess(const FApi2SharedAccessMetadata& InMetadata, TSharedRef<FOnSharedAccessRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2SharedPublicRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		auto _ = InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2SharedAccessSubsystem::Index(const FApi2SharedAccessBatchRequestMetadata& InMetadata, TSharedRef<FOnSharedAccessBatchRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2SharedAccessIndexRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
	InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
});

	if (!Request->Process()) {
		auto _ = InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
