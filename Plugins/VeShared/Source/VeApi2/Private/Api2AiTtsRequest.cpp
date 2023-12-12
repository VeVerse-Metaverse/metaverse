// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2AiTtsRequest.h"

FApi2AiTtsRequestMetadata::FApi2AiTtsRequestMetadata() {}

TSharedPtr<FJsonObject> FApi2AiTtsRequestMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Engine, Engine);
	JsonObject->SetStringField(Api::Fields::LanguageCode, LanguageCode);
	JsonObject->SetStringField(Api::Fields::OutputFormat, OutputFormat);
	JsonObject->SetStringField(Api::Fields::SampleRate, SampleRate);
	JsonObject->SetStringField(Api::Fields::Text, Text);
	JsonObject->SetStringField(Api::Fields::TextType, TextType);
	JsonObject->SetStringField(Api::Fields::VoiceId, VoiceId);
	return JsonObject;
}

FApi2AiTtsResponseMetadata::FApi2AiTtsResponseMetadata() {}

bool FApi2AiTtsRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s/ai/tts"), *Api::GetApi2RootUrl()));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2AiTtsRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		if (FString Payload; GetResponseStringPayload(Response, Payload)) {
			ResponseMetadata.Url = Payload;
			return true;
		}
	}
	return false;
}

bool FApi2AiStreamTtsRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s/ai/tts/stream"), *Api::GetApi2RootUrl()));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2AiStreamTtsRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TArray<uint8> Payload;
		Payload.SetNumUninitialized(Response->GetContentLength());
		Payload.Append(Response.Get()->GetContent());
		// Save to file
		FGuid Guid = FGuid::NewGuid();
		FString FilePath = FString::Printf(TEXT("%saudio/%s.mp3"), *FPaths::ProjectSavedDir(), *Guid.ToString());
		if (!FFileHelper::SaveArrayToFile(Payload, *FilePath)) {
			return false;
		}
		ResponseMetadata.Url = FilePath;
		return true;
	}
	return false;
}
