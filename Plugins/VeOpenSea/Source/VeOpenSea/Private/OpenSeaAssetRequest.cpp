// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "OpenSeaAssetRequest.h"
#include "OpenSeaCommon.h"
#include "OpenSeaConfig.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "VeShared.h"

bool FOpenSeaRequest::Process() {
#if UE_BUILD_SHIPPING
	HttpRequest->SetHeader(TEXT("X-API-KEY"), OpenSea::Key);
#else
	HttpRequest->SetHeader(TEXT("X-API-KEY"), OpenSea::Key);
#endif
	return FHttpRequest::Process();
}

/** https://docs.opensea.io/reference/getting-assets */
bool FOpenSeaAssetBatchRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *OpenSea::AssetsUrl, *RequestMetadata.ToUrlParams()));
	return FOpenSeaRequest::Process();
}

bool FOpenSeaAssetBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse(Response)) {
		const FString ResponseBodyJsonString = Response->GetContentAsString();
		const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseBodyJsonString);
		TSharedPtr<FJsonObject> JsonObject;
		if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid()) {
			LogWarningF("failed to deserialize json: %s", *ResponseBodyJsonString);
		} else {
			if (!JsonObject->HasTypedField<EJson::Array>(Api::Fields::Assets)) {
				LogWarningF("json object does not contain required field");
			} else {
				const auto JsonPayload = JsonObject->GetArrayField(Api::Fields::Assets);
				if (JsonPayload.Num() == 0) {
					LogWarningF("failed to get json payload: %s", *ResponseBodyJsonString);
				} else {
					if (ResponseMetadata.FromJson(JsonPayload)) {
						bOk = true;
					}
				}
			}
		}
	}

	return bOk;
}

/** https://docs.opensea.io/reference/retrieving-a-single-asset */
bool FOpenSeaAssetGetRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s/"), *OpenSea::AssetUrl, *ContractAddress, *TokenId));
	return FOpenSeaRequest::Process();
}

bool FOpenSeaAssetGetRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse(Response)) {
		const FString ResponseBodyJsonString = Response->GetContentAsString();
		const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseBodyJsonString);
		TSharedPtr<FJsonObject> JsonObject;
		if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid()) {
			LogWarningF("failed to deserialize json: %s", *ResponseBodyJsonString);
		} else {
			if (ResponseMetadata.FromJson(JsonObject)) {
				bOk = true;
			}
		}
	}

	return bOk;
}
