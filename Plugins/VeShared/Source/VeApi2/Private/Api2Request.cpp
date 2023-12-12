// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2Request.h"

#include "VeApi2.h"
#include "VeShared.h"
#include "Api2AuthSubsystem.h"

FApi2Request::FApi2Request() { }

FApi2Request::~FApi2Request() { }

void FApi2Request::SetApiKey() const {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		const FString Key = AuthSubsystem->GetUserApiKey();
		if (!Key.IsEmpty()) {
			HttpRequest->SetHeader(Api::Headers::Key, Key);
		}
	}
}

bool FApi2Request::Process() {
	// Whitelabel client ID.
	HttpRequest->SetHeader(TEXT("X-Ve-Id"), "");
	// Whitelabel client key/secret. 
	HttpRequest->SetHeader(TEXT("X-Ve-Secret"), "");

	// Client authorization
	if (HttpRequest->GetHeader(TEXT("Authorization")).IsEmpty()) {
		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
		if (AuthSubsystem) {
			const auto Token = AuthSubsystem->GetSessionToken();
			if (!Token.IsEmpty()) {
				HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Token));
			}
		}
	}

	if (HttpRequest->GetHeader(TEXT("Accept-Encoding")).IsEmpty()) {
		HttpRequest->SetHeader(TEXT("Accept-Encoding"), TEXT("application/json"));
	}

	if (HttpRequest->GetHeader(TEXT("Content-Type")).IsEmpty()) {
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	}

	return FHttpRequest::Process();
}

// bool FApi2Request::OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, const bool bSuccessful) {
// 	return FHttpRequest::OnComplete(InRequest, InResponse, bSuccessful);
// }

bool FApi2Request::CheckResponse() {
	auto& Response = GetResponse();
	if (!Response) {
		return false;
	}

	const auto ContentTypeString = Response->GetContentType();

	if (!EHttpResponseCodes::IsOk(GetResponseCode())) {
		const auto ResponseContent = Response->GetContentAsString();

		if (ContentTypeString == TEXT("application/json")) {
			TSharedPtr<FJsonObject> JsonPayload;
			const auto JsonReaderRef = TJsonReaderFactory<>::Create(ResponseContent);
			if (!FJsonSerializer::Deserialize(JsonReaderRef, JsonPayload) || !JsonPayload) {
				ErrorString = FString::Printf(TEXT("failed to deserialize json: %s"), *ResponseContent);
				return false;
			}
			ErrorString = JsonPayload->GetStringField(TEXT("message"));
		} else if (Response->GetResponseCode() >= 500) {
			ErrorString = FString::Printf(TEXT("%d %s"), Response->GetResponseCode(), TEXT("Server Error"));
		} else {
			ErrorString = ResponseContent;
		}

		return false;
	}

	return true;
}

bool FApi2Request::GetResponseStringPayload(const FHttpResponsePtr Response, FString& OutStringPayload) {
	if (!Response) {
		return false;
	}

	const auto ResponseContent = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	const auto JsonReaderRef = TJsonReaderFactory<>::Create(ResponseContent);
	if (!FJsonSerializer::Deserialize(JsonReaderRef, JsonObject) || !JsonObject) {
		VeLogError("failed to deserialize json: %s", *ResponseContent);
		return false;
	}

	OutStringPayload = JsonObject->GetStringField(TEXT("data"));
	if (OutStringPayload.IsEmpty()) {
		return false;
	}

	return true;
}

bool FApi2Request::GetResponseJsonObjectPayload(const FHttpResponsePtr Response, TSharedPtr<FJsonObject>& OutJsonPayload) {
	if (!Response) {
		return false;
	}

	const auto ResponseContent = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	const auto JsonReaderRef = TJsonReaderFactory<>::Create(ResponseContent);
	if (!FJsonSerializer::Deserialize(JsonReaderRef, JsonObject) || !JsonObject) {
		VeLogError("failed to deserialize json: %s", *ResponseContent);
		return false;
	}

	OutJsonPayload = JsonObject->GetObjectField(TEXT("data"));
	if (!OutJsonPayload) {
		return false;
	}

	return true;
}

bool FApi2Request::GetResponseJsonArrayPayload(const FHttpResponsePtr Response, TArray<TSharedPtr<FJsonValue>>& OutJsonPayload) {
	if (!Response) {
		return false;
	}

	const auto ResponseContent = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	const auto JsonReaderRef = TJsonReaderFactory<>::Create(ResponseContent);
	if (!FJsonSerializer::Deserialize(JsonReaderRef, JsonObject) || !JsonObject) {
		VeLogError("failed to deserialize json: %s", *ResponseContent);
		return false;
	}

	OutJsonPayload = JsonObject->GetArrayField(TEXT("data"));
	return true;
}

bool FApi2Request::SerializeJsonPayload(const TSharedPtr<FJsonObject>& JsonObject, TArray<uint8>& OutBytes) {
	if (!JsonObject) {
		return false;
	}

	FString RequestContent;
	const auto JsonWriterRef = TCondensedJsonWriterFactory::Create(&RequestContent);

	if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriterRef)) {
		return false;
	}

	const auto StringConverter = FTCHARToUTF8(*RequestContent);
	const auto* RequestContentUtf = const_cast<ANSICHAR*>(StringConverter.Get());
	const auto RequestContentUtfLen = FCStringAnsi::Strlen(RequestContentUtf);

	OutBytes.SetNumUninitialized(RequestContentUtfLen);
	for (int32 i = 0; i < RequestContentUtfLen; i++) {
		OutBytes[i] = RequestContentUtf[i];
	}

	return true;
}

void FApi2StringRequest::SetRequestBody(const FString& InBody) {
	RequestBody = InBody;
}

FString FApi2StringRequest::GetResponseBody() const {
	return ResponseBody;
}

void FApi2StringRequest::SetUrl(const FString& InUrl) const {
	HttpRequest->SetURL(InUrl);
}

void FApi2StringRequest::SetVerb(const FString& InVerb) const {
	HttpRequest->SetVerb(InVerb);
}

bool FApi2StringRequest::Process() {
	HttpRequest->SetContentAsString(RequestBody);

	return FApi2Request::Process();
}

bool FApi2StringRequest::OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) {
	if (CheckResponse()) {
		ResponseBody = InResponse->GetContentAsString();
	}

	return false;
}

EApi2ResponseCode FApi2Request::GetApiResponseCode() const {
	if (!GetResponse().IsValid()) {
		return EApi2ResponseCode::Unknown;
	}

	int32 ResponseCode = GetResponseCode();

	if (ResponseCode >= 200 && ResponseCode < 300) {
		return EApi2ResponseCode::Ok;
	}

	switch (ResponseCode) {
	case 400:
	case 403:
	case 404:
		return static_cast<EApi2ResponseCode>(ResponseCode);
	default:
		return EApi2ResponseCode::Unknown;
	}
}

bool FApi2DeleteEntityRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(Api::GetEntitiesUrl() / Id.ToString(EGuidFormats::Digits));

	return FApi2Request::Process();
}

bool FApi2DeleteEntityRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	return CheckResponse();
}
