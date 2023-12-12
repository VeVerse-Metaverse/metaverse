// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiRequest.h"

#include "ApiAuthSubsystem.h"
#include "ApiCommon.h"
#include "ApiSubsystem.h"
#include "Interfaces/IHttpResponse.h"
#include "VeApi.h"
#include "VeShared.h"

#if !UE_EDITOR
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#endif

FApiRequest::FApiRequest() {}

FApiRequest::~FApiRequest() {}

static int64 RequestId = -1;

bool FApiRequest::Process() {
	HttpRequest->SetHeader(Api::Headers::MetaverseId, "bd-metaverse");
	HttpRequest->SetHeader(Api::Headers::MetaverseKey, "6e500749-0833-4b02-b3f4-cebc7e6b227b");
	HttpRequest->SetHeader(Api::Headers::RequestId, FString::Printf(TEXT("%lld"), ++RequestId));

	if (HttpRequest->GetHeader(Api::Headers::Key).IsEmpty()) {
		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api, Auth);
		if (AuthSubsystem) {
			const FString Key = AuthSubsystem->GetUserApiKey();
			if (!Key.IsEmpty()) {
				HttpRequest->SetHeader(Api::Headers::Key, Key);
			}
		}
	}

	if (HttpRequest->GetHeader(Api::Headers::ContentType).IsEmpty()) {
		HttpRequest->SetHeader(Api::Headers::ContentType, Api::ContentTypes::ApplicationJson);
	}

	return FHttpRequest::Process();
}

// bool FApiRequest::OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) {
// 	return FHttpRequest::OnComplete(InRequest, InResponse, bSuccessful);
// }

bool FApiRequest::CheckResponse(const FHttpResponsePtr Response) {
	if (!Response.IsValid()) {
		return false;
	}

	if (!EHttpResponseCodes::IsOk(Response->GetResponseCode())) {
		const FString ResponseString = Response->GetContentAsString();
		const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseString);
		TSharedPtr<FJsonObject> JsonPayload;
		if (!FJsonSerializer::Deserialize(JsonReader, JsonPayload) || !JsonPayload.IsValid()) {
			ErrorString = FString::Printf(TEXT("failed to deserialize json: %s"), *ResponseString);
			return false;
		}

		ErrorString = JsonPayload->GetStringField(Api::Fields::Detail);

		LogWarningF("bad response: %d at %s: %s", Response->GetResponseCode(), *Response->GetURL(), *ErrorString);

		return false;
	}

	return true;
}

bool FApiRequest::GetResponseJsonObjectPayload(const FHttpResponsePtr Response, TSharedPtr<FJsonObject>& OutJsonPayload) const {
	const FString ResponseBodyJsonString = Response->GetContentAsString();
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseBodyJsonString);
	TSharedPtr<FJsonObject> JsonObject;
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid()) {
		LogWarningF("failed to deserialize json: %s", *ResponseBodyJsonString);
		return false;
	}

	OutJsonPayload = JsonObject->GetObjectField(Api::Fields::Payload);
	if (!OutJsonPayload.IsValid()) {
		LogWarningF("failed to get json payload: %s", *ResponseBodyJsonString);
		return false;
	}

	return true;
}

bool FApiRequest::GetResponseJsonArrayPayload(const FHttpResponsePtr Response, TArray<TSharedPtr<FJsonValue>>& OutJsonPayload) const {
	const FString ResponseBodyJsonString = Response->GetContentAsString();
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseBodyJsonString);
	TSharedPtr<FJsonObject> JsonObject;
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid()) {
		LogWarningF("failed to deserialize json: %s", *ResponseBodyJsonString);
		return false;
	}

	OutJsonPayload = JsonObject->GetArrayField(Api::Fields::Payload);
	return true;
}

bool FApiRequest::SerializeJsonPayload(const TSharedPtr<FJsonObject>& JsonObject, TArray<uint8>& OutBytes) const {
	FString RequestBodyJsonString;
	const TCondensedJsonWriterRef JsonWriter = TCondensedJsonWriterFactory::Create(&RequestBodyJsonString);

	if (!JsonObject.IsValid()) {
		return false;
	}

	if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter)) {
		return false;
	}

	const FTCHARToUTF8 StringConverter = FTCHARToUTF8(*RequestBodyJsonString);
	const char* Utf8RequestBodyJsonString = const_cast<ANSICHAR*>(StringConverter.Get());
	const int32 Utf8RequestBodyJsonStringLen = FCStringAnsi::Strlen(Utf8RequestBodyJsonString);

	OutBytes.SetNumUninitialized(Utf8RequestBodyJsonStringLen);
	for (int32 I = 0; I < Utf8RequestBodyJsonStringLen; I++) {
		OutBytes[I] = Utf8RequestBodyJsonString[I];
	}

	return true;
}

EApiResponseCode FApiRequest::GetApiResponseCode(const FHttpResponsePtr& Response) const {
	int32 StatusCode = Response->GetResponseCode();
	
	if (StatusCode >= 200 && StatusCode < 300) {
		return EApiResponseCode::Ok;
	}

	switch (StatusCode) {
	case 400:
	case 403:
	case 404:
		return static_cast<EApiResponseCode>(StatusCode);
	default:
		return EApiResponseCode::Unknown;
	}
}

bool FApiDeleteEntityRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(Api::GetEntitiesUrl() / Id.ToString(EGuidFormats::Digits));

	return FApiRequest::Process();
}

bool FApiDeleteEntityRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse(Response)) {
		bOk = true;
	}

	return bOk;
}
