// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiEventRequest.h"

#include "ApiCommon.h"

bool FApiEventBatchRequest::Process() {
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *Api::GetEventsUrl(), *RequestMetadata.ToUrlParams()));
	return TApiBatchRequest<FApiEventMetadata>::Process();
}

bool FApiEventBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiGetEventRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetEventsUrl(), *RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens)));
	
	return FApiRequest::Process();
}

bool FApiGetEventRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiUserEventBatchRequest::Process() {
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s?%s"), *Api::GetUsersUrl(), *RequestMetadata.UserId.ToString(EGuidFormats::DigitsWithHyphens), *Api::Events, *RequestMetadata.ToUrlParams()));
	return TApiBatchRequest<FApiEventMetadata>::Process();
}

bool FApiUserEventBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiCreateEventRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(Api::GetEventsUrl());
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiCreateEventRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}

bool FApiUpdateEventRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetEventsUrl() / RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiUpdateEventRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}
