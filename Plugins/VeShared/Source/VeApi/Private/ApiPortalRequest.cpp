// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPortalRequest.h"

// bool FApiPortalBatchRequest::Process() {
// 	
// 	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *Api::GetPortalsUrl(), *RequestMetadata.ToUrlParams()));
// 	return TApiBatchRequest<FApiPortalMetadata/*, UApiPortalMetadataObject*/>::Process();
// }
//
// bool FApiPortalBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
// 	bool bOk = false;
//
// 	if (CheckResponse(Response)) {
// 		TSharedPtr<FJsonObject> JsonPayload;
// 		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
// 			if (ResponseMetadata.FromJson(JsonPayload)) {
// 				bOk = true;
// 			}
// 		}
// 	}
//
// 	return bOk;
// }

// bool FApiGetPortalRequest::Process() {
// 	HttpRequest->SetVerb(Api::HttpMethods::Get);
// 	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetPortalsUrl(), *RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens)));
//
// 	return FApiRequest::Process();
// }
//
// bool FApiGetPortalRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
// 	bool bOk = false;
//
// 	if (CheckResponse(Response)) {
// 		TSharedPtr<FJsonObject> JsonPayload;
// 		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
// 			if (ResponseMetadata.FromJson(JsonPayload)) {
// 				bOk = true;
// 			}
// 		}
// 	}
//
// 	return bOk;
// }

// bool FApiCreatePortalRequest::Process() {
// 	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();
//
// 	TArray<uint8> RequestBodyBinary;
// 	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
// 		return false;
// 	}
//
// 	HttpRequest->SetVerb(Api::HttpMethods::Post);
// 	HttpRequest->SetURL(Api::GetPortalsUrl());
// 	HttpRequest->SetContent(RequestBodyBinary);
//
// 	return FApiRequest::Process();
// }
//
// bool FApiCreatePortalRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
// 	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
// }

bool FApiUpdatePortalRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetPortalsUrl() / RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiUpdatePortalRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}
