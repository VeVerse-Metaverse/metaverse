// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2PlaceableSubsystem.h"

#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FName FApi2PlaceableSubsystem::Name = TEXT("Api2PlaceableSubsystem");

FApi2PlaceableBatchRequestMetadata::FApi2PlaceableBatchRequestMetadata() { }

TSharedPtr<FJsonObject> FApi2PlaceableBatchRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchRequestMetadata::ToJson();
	if (WorldId.IsValid()) {
		JsonObject->SetStringField(TEXT("spaceId"), WorldId.ToString(EGuidFormats::DigitsWithHyphensLower));
	}
	return JsonObject;
}

TSharedPtr<FJsonObject> FApi2PlaceableClassBatchQueryRequestMetadata::ToJson() {
	return IApiBatchQueryRequestMetadata::ToJson();
}

FString FApi2PlaceableClassBatchQueryRequestMetadata::ToUrlParams() {
	return IApiBatchQueryRequestMetadata::ToUrlParams();
}

bool FApi2GetPlaceableBatchClassRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *Api::GetPlaceableClassesUrl2(), *RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2GetPlaceableBatchClassRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2PlaceableBatchRequest::Process() {
	HttpRequest->SetVerb(TEXT("GET"));
	// todo: replace Spaces to Worlds
	HttpRequest->SetURL(FString::Printf(TEXT("%s/placeables?%s"), *(Api::GetApi2RootUrl() / TEXT("spaces") / RequestMetadata.WorldId.ToString(EGuidFormats::DigitsWithHyphensLower)), *RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2PlaceableBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}


void FApi2PlaceableSubsystem::Shutdown() {}


bool FApi2PlaceableRequest::Process() {
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetApi2RootUrl(), TEXT("objects"), *PlaceableId.ToString(EGuidFormats::DigitsWithHyphensLower)));

	return FApi2Request::Process();
}

bool FApi2PlaceableRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2UpdateWorldPlaceableRequest::Process() {
	if (!Id.IsValid()) {
		return false;
	}

	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(Api::GetWorldUrl2() / Id.ToString(EGuidFormats::DigitsWithHyphens) / Api::Placeables);
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UpdateWorldPlaceableRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2UpdatePlaceableTransformRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetWorldUrl2() / Api::Placeables / RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens) / Api::Transform);
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UpdatePlaceableTransformRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2UpdatePlaceableEntityRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetSpacesUrl() / Api::Placeables / RequestPlaceableId.ToString(EGuidFormats::DigitsWithHyphensLower) / Api::Entity / RequestEntityId.ToString(EGuidFormats::DigitsWithHyphensLower));

	return FApi2Request::Process();
}

bool FApi2UpdatePlaceableEntityRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2DeletePlaceableRequest::Process() {
	if (!Id.IsValid()) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(Api::GetSpacesUrl() / Api::Placeables / Id.ToString(EGuidFormats::DigitsWithHyphens));

	return FApi2Request::Process();
}

bool FApi2DeletePlaceableRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		bOk = true;
	}

	return bOk;
}

void FApi2PlaceableSubsystem::Initialize() {}

void FApi2PlaceableSubsystem::Index(const FApi2PlaceableBatchRequestMetadata& InMetadata, TSharedRef<FOnPlaceableBatchRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2PlaceableBatchRequest>();
	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PlaceableSubsystem::Get(const FGuid& Id, TSharedRef<FOnPlaceableRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2PlaceableRequest>();
	Request->PlaceableId = Id;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PlaceableSubsystem::GetPlaceableClassBatch(const FApi2PlaceableClassBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnPlaceableClassBatchRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2GetPlaceableBatchClassRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}

void FApi2PlaceableSubsystem::UpdatePlaceable(const FGuid& InSpaceId, const FApiPlaceableMetadata& InPlaceableMetadata, TSharedPtr<FOnPlaceableRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2UpdateWorldPlaceableRequest>();
	Request->RequestMetadata = InPlaceableMetadata;
	Request->Id = InSpaceId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
void FApi2PlaceableSubsystem::UpdatePlaceableTransform(const FApiUpdatePlaceableTransformMetadata& InPlaceableMetadata, TSharedPtr<FOnPlaceableRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2UpdatePlaceableTransformRequest>();
	Request->RequestMetadata = InPlaceableMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
void FApi2PlaceableSubsystem::UpdatePlaceableEntity(const FGuid& InPlaceableId, const FGuid& InEntityId, TSharedRef<FOnPlaceableRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2UpdatePlaceableEntityRequest>();
	Request->RequestPlaceableId = InPlaceableId;
	Request->RequestEntityId = InEntityId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
void FApi2PlaceableSubsystem::DeletePlaceable(const FGuid& InPlaceableId, TSharedPtr<FOnGenericRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2DeletePlaceableRequest>();
	Request->Id = InPlaceableId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, TEXT("not processed"));
	}
}
