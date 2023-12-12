// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPlaceableRequest.h"
#include "ApiCommon.h"

bool FApiUpdateSpacePlaceableRequest::Process() {
	if (!Id.IsValid()) {
		return false;
	}

	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(Api::GetSpacesUrl() / Id.ToString(EGuidFormats::DigitsWithHyphens) / Api::Placeables);
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiUpdateSpacePlaceableRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	ResponseCode = GetApiResponseCode(Response);

	if (CheckResponse(Response)) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			ResponsePlaceableMetadata.FromJson(JsonPayload);
			bOk = true;
		}
	}

	return bOk;
}

bool FApiPlaceableBatchRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(Api::GetSpacesUrl() / SpaceId.ToString(EGuidFormats::DigitsWithHyphens) / Api::Placeables + "?" + RequestMetadata.ToUrlParams());

	return FApiRequest::Process();
}

bool FApiPlaceableBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiGetPlaceableRequest::Process() {
	if (!RequestEntityId.IsValid()) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(Api::GetSpacesUrl() / Api::Placeables / RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens));

	return FApiRequest::Process();
}

bool FApiGetPlaceableRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	ResponseCode = GetApiResponseCode(Response);

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

bool FApiUpdatePlaceableTransformRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetSpacesUrl() / Api::Placeables / RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens) / Api::Transform);
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiUpdatePlaceableTransformRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	ResponseCode = GetApiResponseCode(Response);

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

bool FApiUpdatePlaceableEntityRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetSpacesUrl() / Api::Placeables / RequestPlaceableId.ToString(EGuidFormats::DigitsWithHyphensLower) / Api::Entity / RequestEntityId.ToString(EGuidFormats::DigitsWithHyphensLower));

	return FApiRequest::Process();
}

bool FApiUpdatePlaceableEntityRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	ResponseCode = GetApiResponseCode(Response);

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

bool FApiDeletePlaceableRequest::Process() {
	if (!Id.IsValid()) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(Api::GetSpacesUrl() / Api::Placeables / Id.ToString(EGuidFormats::DigitsWithHyphens));

	return FApiRequest::Process();
}

bool FApiDeletePlaceableRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse(Response)) {
		bOk = true;
	}

	return bOk;
}

bool FApiGetPlaceableBatchClassRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *Api::GetPlaceableClassesUrl(), *RequestMetadata.ToUrlParams()));
	return FApiRequest::Process();
}

bool FApiGetPlaceableBatchClassRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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
