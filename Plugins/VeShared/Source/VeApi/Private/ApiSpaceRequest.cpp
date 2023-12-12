// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiSpaceRequest.h"

#include "ApiCommon.h"

// bool FApiSpaceBatchRequest::Process() {
// 	HttpRequest->SetVerb(Api::HttpMethods::Get);
// 	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *Api::GetSpacesUrl(), *RequestMetadata.ToUrlParams()));
//
// 	return FApiRequest::Process();
// }
//
// bool FApiSpaceBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiUserSpaceBatchRequest::Process() {
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s?%s"),
										*Api::GetUsersUrl(),
										*RequestMetadata.UserId.ToString(EGuidFormats::DigitsWithHyphens),
										*Api::Spaces,
										*RequestMetadata.ToUrlParams()));
	return TApiBatchRequest<FApiSpaceMetadata/*, UApiObjectMetadataObject*/>::Process();
}

bool FApiUserSpaceBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiMetaverseSpacesBatchRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s?%s"),
		*Api::GetMetaversesUrl(),
		*MetaverseId.ToString(EGuidFormats::DigitsWithHyphensLower),
		*Api::Spaces,
		*QueryMetadata.ToUrlParams()));
	
	return FApiRequest::Process();
}

bool FApiMetaverseSpacesBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse(Response)) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				return true;
			}
		}
	}
	return false;
}

bool FApiGetSpaceRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetSpacesUrl(), *RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens)));

	return FApiRequest::Process();
}

bool FApiGetSpaceRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

// bool FApiCreateSpaceRequest::Process() {
// 	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();
//
// 	TArray<uint8> RequestBodyBinary;
// 	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
// 		return false;
// 	}
//
// 	HttpRequest->SetVerb(Api::HttpMethods::Post);
// 	HttpRequest->SetURL(Api::GetSpacesUrl());
// 	HttpRequest->SetContent(RequestBodyBinary);
//
// 	return FApiRequest::Process();
// }
//
// bool FApiCreateSpaceRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
// 	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
// }

bool FApiUpdateSpaceRequest::Process() {
	if (!RequestMetadata.Id.IsValid()) {
		return false;
	}

	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetSpacesUrl() / RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiUpdateSpaceRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}
