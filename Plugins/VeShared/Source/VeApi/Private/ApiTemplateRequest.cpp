// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "ApiTemplateRequest.h"
#include "ApiCommon.h"


bool FApiTemplateBatchRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *Api::Template::GetTemplatesUrl(), *RequestMetadata.ToUrlParams()));

	return FApiRequest::Process();
}

bool FApiTemplateBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiGetTemplateRequest::Process() {
	
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::Template::GetTemplatesUrl(), *RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens)));
	
	return FApiRequest::Process();
}

bool FApiGetTemplateRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	return FApiRequest::OnComplete(Request, Response, bSuccessful);
}

bool FApiCreateTemplateRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(Api::Template::GetTemplatesUrl());
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiCreateTemplateRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}

bool FApiUpdateTemplateRequest::Process() {
	if (!RequestMetadata.TemplateId.IsValid()) {
		return false;
	}

	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::Template::GetTemplatesUrl() / RequestMetadata.TemplateId.ToString(EGuidFormats::DigitsWithHyphens));
	HttpRequest->SetContent(RequestBodyBinary);
	
	return FApiRequest::Process();
}

bool FApiUpdateTemplateRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}



