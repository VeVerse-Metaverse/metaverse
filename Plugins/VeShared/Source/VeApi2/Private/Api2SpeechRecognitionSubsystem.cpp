// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2SpeechRecognitionSubsystem.h"
#include "VeApi2.h"

FName FApi2SpeechRecognitionSubsystem::Name = TEXT("Api2SpeechToTextSubsystem");

FApi2SpeechRecognitionRequestMetadata::FApi2SpeechRecognitionRequestMetadata() {}

TSharedPtr<FJsonObject> FApi2SpeechRecognitionRequestMetadata::ToJson() {
	return IApiMetadata::ToJson();
}

FApi2SpeechRecognitionResponseMetadata::FApi2SpeechRecognitionResponseMetadata() {}

bool FApi2SpeechRecognitionRequest::Process() {
	HttpRequest->SetTimeout(60.0f * 5.0f);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s/ai/stt"), *Api::GetApi2RootUrl()));

	const FString Boundary = TEXT("----VeApi2SpeechRecognitionRequestBoundary");
	HttpRequest->SetHeader(TEXT("Content-Type"), FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));

	TArray<uint8> RequestContent;
	TArray<uint8> FileContent;

	const FString BoundaryBegin = "\r\n--" + Boundary + "\r\n";

	if (RequestMetadata.FilePath.IsEmpty()) {
		VeLogErrorFunc("File path is empty");
		return false;
	}

	if (RequestMetadata.FilePath.StartsWith(TEXT("file:///"))) {
		constexpr auto PrefixLen = 8;
		RequestMetadata.FilePath.RightChopInline(PrefixLen);
	}

	if (!FPaths::FileExists(RequestMetadata.FilePath)) {
		VeLogErrorFunc("File not found: %s", *RequestMetadata.FilePath);
		return false;
	}

	if (!FFileHelper::LoadFileToArray(FileContent, *RequestMetadata.FilePath)) {
		return false;
	}

	if (FileContent.Num() <= 0) {
		VeLogErrorFunc("File is empty: %s", *RequestMetadata.FilePath);
		return false;
	}

	// Add boundary.
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*BoundaryBegin)), BoundaryBegin.Len());

	// Add file header.
	const FString FileHeader = FString::Printf(TEXT("Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\nContent-Type: \"audio/wav\"\r\n\r\n"),
		*FPaths::GetCleanFilename(RequestMetadata.FilePath));
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*FileHeader)), FileHeader.Len());

	// Add file content.
	RequestContent.Append(FileContent);

	const FString BoundaryEnd = "\r\n--" + Boundary + "--\r\n";
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*BoundaryEnd)), BoundaryEnd.Len());

	HttpRequest->SetContent(RequestContent);

	return FApi2Request::Process();
}

bool FApi2SpeechRecognitionRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		if (GetResponseStringPayload(Response, ResponseMetadata.Result)) {
			return true;
		}
	}

	return false;
}

void FApi2SpeechRecognitionSubsystem::Initialize() {}

void FApi2SpeechRecognitionSubsystem::Shutdown() {}

void FApi2SpeechRecognitionSubsystem::Recognize(const FApi2SpeechRecognitionRequestMetadata& InMetadata, const TSharedRef<FOnSpeechRecognitionRequestCompleted2> InCallback) const {
	// Create request object.
	const auto Request = MakeShared<FApi2SpeechRecognitionRequest>();

	// Set request metadata.
	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool /*bInSuccessful*/) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
