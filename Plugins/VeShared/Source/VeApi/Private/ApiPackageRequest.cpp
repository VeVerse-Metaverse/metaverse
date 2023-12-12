// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPackageRequest.h"

#include "ApiCommon.h"

// bool FApiMetaverseBatchRequest::Process() {
// 	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *Api::GetMetaversesUrl(), *RequestMetadata.ToUrlParams()));
// 	return TApiBatchRequest<FApiMetaverseMetadata>::Process();
// }
//
// bool FApiMetaverseBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiGetPackageRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetMetaversesUrl(), *RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens)));
	
	return FApiRequest::Process();
}

bool FApiGetPackageRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiUserPackageBatchRequest::Process() {
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s?%s"), *Api::GetUsersUrl(), *RequestMetadata.UserId.ToString(EGuidFormats::DigitsWithHyphens), *Api::Metaverses, *RequestMetadata.ToUrlParams()));
	return TApiBatchRequest<FApiPackageMetadata/*, UApiModMetadataObject*/>::Process();
}

bool FApiUserPackageBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

// bool FApiCreateMetaverseRequest::Process() {
// 	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();
//
// 	TArray<uint8> RequestBodyBinary;
// 	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
// 		return false;
// 	}
//
// 	HttpRequest->SetVerb(Api::HttpMethods::Post);
// 	HttpRequest->SetURL(Api::GetMetaversesUrl());
// 	HttpRequest->SetContent(RequestBodyBinary);
//
// 	return FApiRequest::Process();
// }
//
// bool FApiCreateMetaverseRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
// 	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
// }

bool FApiUpdatePackageRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetMetaversesUrl() / RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiUpdatePackageRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}
