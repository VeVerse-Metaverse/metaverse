// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "FileHeaderRequest.h"

#include "VeShared.h"

FFileHeaderResult::FFileHeaderResult(const FString& InUrl, const TMap<FString, FString>& InHeaders): Url(InUrl), Headers(InHeaders) {
}

FString FFileHeaderResult::GetHeader(const FString& HeaderName) const {
	FString Result;
	if (const FString* Header = Headers.Find(HeaderName)) {
		Result = *Header;
	}
	return Result;
}

FString FFileHeaderResult::GetContentMimeType() const {
	return GetHeader(TEXT("Content-Type"));
}

int32 FFileHeaderResult::GetContentLength() const {
	const FString ContentRangeHeader = GetHeader(TEXT("Content-Length"));
	FString ContentLength;
	ContentRangeHeader.Split(TEXT("/"), nullptr, &ContentLength);
	return static_cast<int32>(FCString::Atoi64(*ContentLength));
}

bool FFileHeaderRequest::Process() {
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb("HEAD");

	return FHttpRequest::Process();
}

bool FFileHeaderRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse(Response)) {
		TArray<FString> ResponseHeaders = Response->GetAllHeaders();

		ResponseMetadata = FFileHeaderResult{Url, {}};

		for (const FString& ItHeader : ResponseHeaders) {
			FString Left, Right;
			ItHeader.Split(TEXT(": "), &Left, &Right);
			ResponseMetadata.Headers.Add(Left, Right);
		}

		bOk = true;
	}

	return bOk;
}
