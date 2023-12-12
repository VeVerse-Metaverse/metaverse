// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "ApiPersonaRequest.h"
#include "VeShared.h"
#include "ApiCommon.h"

bool FApiGetPersonaRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(Api::GetUsersUrl() / Api::Personas / RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens));

	return FApiRequest::Process();
}

bool FApiGetPersonaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiGetPersonaBatchRequest::Process() {

	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(Api::GetUsersUrl() / RequestUserId.ToString(EGuidFormats::DigitsWithHyphens) / Api::Personas + "?" + RequestMetadata.ToUrlParams());

	return FApiRequest::Process();
}

bool FApiGetPersonaBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiCreatePersonaRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(Api::GetUserMeUrl() / Api::Personas);
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiCreatePersonaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiDefaultPersonaRequest::Process() {

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(Api::GetUserMeUrl() / Api::Personas / Api::Default + FString::Printf(TEXT("?%s="), Api::Fields::Id) + PersonaId.ToString(EGuidFormats::DigitsWithHyphens));

	return FApiRequest::Process();
}

bool FApiDefaultPersonaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiUpdatePersonaRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetUserMeUrl() / Api::Personas / RequestMetadata.PersonaId.ToString(EGuidFormats::DigitsWithHyphens));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiUpdatePersonaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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

bool FApiDeletePersonaRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetUserMeUrl() / Api::Personas / RequestMetadata.PersonaId.ToString(EGuidFormats::DigitsWithHyphens));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApiRequest::Process();
}

bool FApiDeletePersonaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
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
