// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2PersonaSubsystem.h"

#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FName FApi2PersonaSubsystem::Name = TEXT("Api2PersonaSubsystem");

bool FApi2GetPersonaBatchRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(Api::GetUsersUrl2() / RequestUserId.ToString(EGuidFormats::DigitsWithHyphens) / Api::Personas + "?" + RequestMetadata.ToUrlParams());

	return FApi2Request::Process();
}

bool FApi2GetPersonaBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2GetPersonaRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(Api::GetUsersUrl2() / Api::Personas / RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens));

	return FApi2Request::Process();
}

bool FApi2GetPersonaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

bool FApi2CreatePersonaRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(Api::GetUserMeUrl2() / Api::Personas);
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2CreatePersonaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

bool FApi2DefaultPersonaRequest::Process() {
	
	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(Api::GetUserMeUrl2() / Api::Personas / Api::Default + FString::Printf(TEXT("?%s="), Api::Fields::Id) + PersonaId.ToString(EGuidFormats::DigitsWithHyphens));

	return FApi2Request::Process();
}

bool FApi2DefaultPersonaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

bool FApi2UpdatePersonaRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetUserMeUrl2() / Api::Personas / RequestMetadata.PersonaId.ToString(EGuidFormats::DigitsWithHyphens));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UpdatePersonaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

bool FApi2DeletePersonaRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(Api::GetUserMeUrl2() / Api::Personas / RequestMetadata.PersonaId.ToString(EGuidFormats::DigitsWithHyphens));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2DeletePersonaRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

void FApi2PersonaSubsystem::Initialize() {}
void FApi2PersonaSubsystem::Shutdown() {}

void FApi2PersonaSubsystem::GetPersonas(const FGuid& InUserId, const IApiBatchQueryRequestMetadata InMetadata, TSharedPtr<FOnPersonaBatchRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2GetPersonaBatchRequest>();
	Request->RequestUserId = InUserId;
	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PersonaSubsystem::GetPersona(const FGuid& InId, TSharedRef<FOnPersonaRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2GetPersonaRequest>();
	Request->RequestEntityId = InId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PersonaSubsystem::PostPersonas(const FApiCreatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2CreatePersonaRequest>();
	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PersonaSubsystem::PostDefaultPersonas(const FGuid& InPersonaId, TSharedPtr<FOnPersonaRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2DefaultPersonaRequest>();
	Request->PersonaId = InPersonaId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
void FApi2PersonaSubsystem::PatchPersonas(const FApiUpdatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2UpdatePersonaRequest>();
	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
void FApi2PersonaSubsystem::DeletePersonas(const FApiUpdatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2DeletePersonaRequest>();
	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
