// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2OpenSeaSubsystem.h"
#include "VeApi2.h"

FName FApi2OpenSeaSubsystem::Name = TEXT("Api2OpenSeaSubsystem");


bool FApi2OpenSeaBatchRequest::Process() {
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *(Api::GetApi2RootUrl() / TEXT("nft") / TEXT("assets")), *RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2OpenSeaBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2OpenSeaRequest::Process() {
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *(Api::GetApi2RootUrl() / TEXT("nft")), *RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower)));

	return FApi2Request::Process();
}

bool FApi2OpenSeaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

void FApi2OpenSeaSubsystem::Initialize() {}

void FApi2OpenSeaSubsystem::Shutdown() {}

void FApi2OpenSeaSubsystem::GetAssets(const IApiBatchQueryRequestMetadata& InMetadata, TSharedRef<FOnOpenSeaBatchRequestCompleted> InCallback) const {
	const auto Request = MakeShared<FApi2OpenSeaBatchRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}

void FApi2OpenSeaSubsystem::Get(const FGuid& InMetadata, TSharedRef<FOnOpenSeaRequestCompleted> InCallback) const {
	const auto Request = MakeShared<FApi2OpenSeaRequest>();

	Request->RequestMetadata.Id = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
