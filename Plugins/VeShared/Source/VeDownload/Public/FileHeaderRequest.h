// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiRequest.h"

struct FManagedTexture;

struct VEDOWNLOAD_API FFileHeaderResult {
	explicit FFileHeaderResult(const FString& InUrl = TEXT(""), const TMap<FString, FString>& InHeaders = {});

	/** Url used to load the texture. */
	FString Url;

	/** Request headers. */
	TMap<FString, FString> Headers = {};

	/** Get header by name */
	FString GetHeader(const FString& HeaderName) const;

	/** Get content type */
	FString GetContentMimeType() const;

	/** Get content length */
	int32 GetContentLength() const;
};

DECLARE_DELEGATE_ThreeParams(FOnFileHeaderRequestComplete, const FFileHeaderResult& /* InResult */, const bool /*bInSuccessful*/, const FString& /*InError*/)

class VEDOWNLOAD_API FFileHeaderRequest final : public FHttpRequest {
public:
	virtual bool Process() override;

	/** Url to download from. */
	FString Url;

	/** Response metadata. */
	FFileHeaderResult ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};
