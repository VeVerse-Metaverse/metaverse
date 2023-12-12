// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPropertyRequest.h"


bool FApiPropertyBatchRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(Api::GetEntitiesUrl() / EntityId.ToString(EGuidFormats::Digits) / Api::Properties);
	return TApiBatchRequest<FApiPropertyMetadata>::Process();
}

bool FApiPropertyBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

// bool FApiPropertyRequest::Process() {
// 	HttpRequest->SetVerb(Api::HttpMethods::Get);
// 	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::PortalsUrl, *RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens)));
//
// 	return FApiRequest::Process();
// }
//
// bool FApiPropertyRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
// 	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
// }

bool FApiCreatePropertyRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(Api::GetEntitiesUrl() / EntityId.ToString(EGuidFormats::Digits) / Api::Properties);
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiCreatePropertyRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}

bool FApiUpdatePropertyRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(Api::GetEntitiesUrl() / EntityId.ToString(EGuidFormats::Digits) / Api::Properties);
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiUpdatePropertyRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}

bool FApiDeletePropertyRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(Api::GetEntitiesUrl() / EntityId.ToString(EGuidFormats::Digits) / Api::Properties / PropertyName);

	return FApiRequest::Process();
}

bool FApiDeletePropertyRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse(Response)) {
		return true;
	}
	return false;
}
