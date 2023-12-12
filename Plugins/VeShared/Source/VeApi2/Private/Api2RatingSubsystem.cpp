// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2RatingSubsystem.h"
#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FName FApi2RatingSubsystem::Name = TEXT("Api2RatingSubsystem");

bool FApi2UpdateDislikeRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetEntitiesUrl2(), *RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower), TEXT("dislike")));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UpdateDislikeRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2UpdateUnlikeRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetEntitiesUrl2(), *RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower), TEXT("unlike")));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UpdateUnlikeRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

void FApi2RatingSubsystem::Initialize() {
}

void FApi2RatingSubsystem::Shutdown() {}

bool FApi2UpdateLikeRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetEntitiesUrl2(), *RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower), TEXT("like")));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UpdateLikeRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

void FApi2RatingSubsystem::UpdateLike(const FGuid& Id, TSharedRef<FOnRatingRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UpdateLikeRequest>();

	Request->RequestMetadata.Id = Id;


	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		auto _ = InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2RatingSubsystem::UpdateDisLike(const FGuid& Id, TSharedRef<FOnRatingRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UpdateDislikeRequest>();

	Request->RequestMetadata.Id = Id;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		auto _ = InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2RatingSubsystem::UpdateUnlike(const FGuid& Id, TSharedRef<FOnRatingRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UpdateUnlikeRequest>();

	Request->RequestMetadata.Id = Id;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		auto _ = InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
