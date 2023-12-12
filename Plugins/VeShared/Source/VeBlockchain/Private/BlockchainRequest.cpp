// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Public/BlockchainRequest.h"

#include "ApiCommon.h"
#include "BlockchainCommon.h"
#include "Interfaces/IHttpResponse.h"
#include "VeShared.h"

static int64 RequestId = -1;

bool FBlockchainTokenMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	const bool bOk = IApiMetadata::FromJson(InJsonObject);
	Name = GetJsonStringField(InJsonObject, Api::Fields::Name);
	Description = GetJsonStringField(InJsonObject, Api::Fields::Description);
	Image = GetJsonStringField(InJsonObject, Api::Fields::Image);
	return bOk;
}

bool FBlockchainRequest::Process() {
	HttpRequest->SetHeader(Api::Headers::MetaverseId, "bd-metaverse");
	HttpRequest->SetHeader(Api::Headers::MetaverseKey, "D65700A3-01C8-416A-A99A-FC886D53E759");
	HttpRequest->SetHeader(Api::Headers::RequestId, FString::Printf(TEXT("%lld"), ++RequestId));

	if (HttpRequest->GetHeader(Api::Headers::ContentType).IsEmpty()) {
		HttpRequest->SetHeader(Api::Headers::ContentType, Api::ContentTypes::ApplicationJson);
	}

	return  FHttpRequest::Process();
}

bool FBlockchainRequest::CheckResponse(const FHttpResponsePtr Response) {
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

		LogWarningF("bad response: %d at %s: %s", GetResponseCode(), *Response->GetURL(), *ErrorString);

		return false;
	}

	return true;
}

bool FBlockchainRequest::GetResponseJsonObjectPayload(const FHttpResponsePtr Response, TSharedPtr<FJsonObject>& OutJsonPayload) const {
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

bool FBlockchainRequest::GetResponseJsonArrayPayload(const FHttpResponsePtr Response, TArray<TSharedPtr<FJsonValue>>& OutJsonPayload) const {
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

bool FBlockchainRequest::SerializeJsonPayload(const TSharedPtr<FJsonObject>& JsonObject, TArray<uint8>& OutBytes) const {
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

bool FBlockchainOwnerOfRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(Blockchain::GetRootUrl() / TEXT("web3") / TEXT("ownerOf") / FString::Printf(TEXT("%d"), RequestTokenId));

	return FBlockchainRequest::Process();
}

bool FBlockchainOwnerOfRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse(Response)) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (JsonPayload->TryGetStringField(Api::Fields::Owner, ResponseOwnerAddress)) {
				bOk = true;
			} else if (JsonPayload->TryGetStringField(Api::Fields::Error, Error)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

bool FBlockchainTokenUriRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(Blockchain::GetRootUrl() / TEXT("web3") / TEXT("tokenUri") / FString::Printf(TEXT("%d"), RequestTokenId));

	return FBlockchainRequest::Process();
}

bool FBlockchainTokenUriRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse(Response)) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			bOk = ResponseMetadata.FromJson(JsonPayload);
		} else if (JsonPayload->TryGetStringField(Api::Fields::Error, Error)) {
			bOk = true;
		}
	}

	return bOk;
}
