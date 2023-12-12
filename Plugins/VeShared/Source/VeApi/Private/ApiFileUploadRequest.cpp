// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiFileUploadRequest.h"

#include "VeShared.h"

bool FApiFileUploadRequest::Process() {
	HttpRequest->SetTimeout(60.0f * 60.0f * 2.0f);
	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(Api::GetEntitiesUrl() / RequestMetadata.EntityId.ToString(EGuidFormats::DigitsWithHyphensLower) / Api::Files / RequestMetadata.FileType);

	const FString Boundary = FString::Printf(TEXT("---------------------------%s"), TEXT("ve-boundary"));
	HttpRequest->SetHeader(Api::Headers::ContentType, FString::Printf(TEXT("%s; boundary =%s"), Api::ContentTypes::MultipartFormData, *Boundary));
	HttpRequest->SetHeader(Api::Headers::Accept, Api::ContentTypes::ApplicationJson);

	TArray<uint8> RequestContent;
	TArray<uint8> FileContent;

	const FString BoundaryBegin = "\r\n--" + Boundary + "\r\n";

#pragma region File
	if (RequestMetadata.FilePath.StartsWith("file:///")) {
		constexpr auto PrefixSize = 8;
		RequestMetadata.FilePath.RightChopInline(PrefixSize);
	}

	if (!FPaths::FileExists(RequestMetadata.FilePath)) {
		LogErrorF("file does not exist at %s", *RequestMetadata.FilePath);

		if (OnRequestComplete.IsBound()) {
			OnRequestComplete.Execute(SharedThis(this), false);
		}
		return false;
	}

	if (FFileHelper::LoadFileToArray(FileContent, *RequestMetadata.FilePath)) {
		if (FileContent.Num() <= 0) {
			LogErrorF("failed to load file from: %s", *RequestMetadata.FilePath);
			if (OnRequestComplete.IsBound()) {
				OnRequestComplete.Execute(SharedThis(this), false);
			}
			return false;
		}

		// Append start boundary.
		RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*BoundaryBegin)), BoundaryBegin.Len());

		// Append file header.
		FString FileHeader = "Content-Disposition: form-data;name=\"file\";";
		FileHeader.Append("filename=\"" + FPaths::GetCleanFilename(RequestMetadata.FilePath) + "\"\r\n");
		FileHeader.Append("Content-Type: \"binary/octet-stream\" \r\n\r\n");
		RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*FileHeader)), FileHeader.Len());

		// Append file content.
		RequestContent.Append(FileContent);
	} else {
		if (OnRequestComplete.IsBound()) {
			OnRequestComplete.Execute(SharedThis(this), false);
		}
		return false;
	}
#pragma endregion

	// Append end boundary.
	const FString BoundaryEnd = "\r\n--" + Boundary + "--\r\n";
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*BoundaryEnd)), BoundaryEnd.Len());

	HttpRequest->SetContent(RequestContent);
	return FApiRequest::Process();
}

bool FApiFileUploadRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}

bool FApiMemoryFileUploadRequest::Process() {
	HttpRequest->SetTimeout(60.0f * 60.0f * 2.0f);
	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(Api::GetEntitiesUrl() / RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens) / Api::Files / RequestMetadata.FileType);

	const FString Boundary = FString::Printf(TEXT("---------------------------%s"), TEXT("ve-boundary"));
	HttpRequest->SetHeader(Api::Headers::ContentType, FString::Printf(TEXT("%s; boundary =%s"), Api::ContentTypes::MultipartFormData, *Boundary));
	HttpRequest->SetHeader(Api::Headers::Accept, Api::ContentTypes::ApplicationJson);

	TArray<uint8> RequestContent;

	const FString BoundaryBegin = "\r\n--" + Boundary + "\r\n";

#pragma region File

	if (RequestMetadata.FileBytes.Num() <= 0) {
		LogErrorF("memory file invalid");
		if (OnRequestComplete.IsBound()) {
			OnRequestComplete.Execute(SharedThis(this), false);
		}
		return false;
	}

	// Append start boundary.
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*BoundaryBegin)), BoundaryBegin.Len());

	// Append file header.
	FString FileHeader = "Content-Disposition: form-data;name=\"file\";";
	FileHeader.Append("filename=\"" + RequestMetadata.FileName + "\"\r\n");
	FileHeader.Append("Content-Type: \"binary/octet-stream\" \r\n\r\n");
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*FileHeader)), FileHeader.Len());

	// Append file content.
	RequestContent.Append(RequestMetadata.FileBytes);

#pragma endregion

	// Append end boundary.
	const FString BoundaryEnd = "\r\n--" + Boundary + "--\r\n";
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*BoundaryEnd)), BoundaryEnd.Len());

	HttpRequest->SetContent(RequestContent);
	return FApiRequest::Process();
}

bool FApiMemoryFileUploadRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}

bool FApiEntityFileLinkRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(
		Api::GetEntitiesUrl() / *RequestMetadata.EntityId.ToString(EGuidFormats::DigitsWithHyphensLower) / Api::Files / RequestMetadata.FileType + FString::Printf(
			TEXT("?file=%s&mime=%s"), *RequestMetadata.File, *RequestMetadata.MimeType));

	return FApiRequest::Process();
}

bool FApiEntityFileLinkRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	API_ENTITY_REQUEST_ON_COMPLETE_BODY();
}
