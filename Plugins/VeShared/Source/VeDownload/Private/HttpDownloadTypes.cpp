// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "HttpDownloadTypes.h"

#include "VeShared.h"
#include "VeDownload.h"

#undef VE_LOG_MODULE
#define VE_LOG_MODULE HttpDownloadSubsystem


FHttpDownloadMetadata::FHttpDownloadMetadata(const TMap<FString, FString>& Headers) {
	LoadFromHeaders(Headers);
}

void FHttpDownloadMetadata::Empty() {
	ContentMetadata = FHttpDownloadContentMetadata();
	DownloadedSize = 0;
	AcceptRanges = false;
}

bool FHttpDownloadMetadata::Serialize(FArchive& Ar) {
	if (!FMetadataFile::Serialize(Ar)) {
		return false;
	}
	
	Ar << MetadataVersion;
	if (MetadataVersion == 1) {
		Ar << ContentMetadata.ContentLength;
		Ar << ContentMetadata.LastModified;
		Ar << ContentMetadata.ETag;
		Ar << DownloadedSize;
		Ar << AcceptRanges;
		return true;
	}

	return false;
}

bool FHttpDownloadMetadata::IsSourceChanged(const FHttpDownloadContentMetadata& InContentInfo) const {
	if (ContentMetadata.ContentLength != InContentInfo.ContentLength) {
		return true;
	}

	if (!ContentMetadata.ETag.IsEmpty() && ContentMetadata.ETag == InContentInfo.ETag) {
		return false;
	}

	if (ContentMetadata.LastModified != FDateTime() && ContentMetadata.LastModified == InContentInfo.LastModified) {
		return false;
	}

	return true;
}

void FHttpDownloadMetadata::LoadFromHeaders(const TMap<FString, FString>& Headers) {
	ContentMetadata.ContentLength = LoadFromHeaders_ContentLength(Headers);
	ContentMetadata.ETag = LoadFromHeaders_Etag(Headers);
	ContentMetadata.LastModified = LoadFromHeaders_LastModified(Headers);
	AcceptRanges = LoadFromHeaders_AcceptRanges(Headers);
	DownloadedSize = 0;
}

int64 FHttpDownloadMetadata::LoadFromHeaders_ContentLength(const TMap<FString, FString>& Headers) {
	// Content-Range
	if (auto* HeaderValue = Headers.Find(TEXT("Content-Range"))) {
		FString ContentLength;
		HeaderValue->Split("/", nullptr, &ContentLength);
		if (ContentLength != TEXT("*")) {
			if (const int64 Result = FCString::Atoi64(*ContentLength)) {
				return Result;
			}
		}
	}

	// Content-Length
	if (auto* HeaderValue = Headers.Find(TEXT("Content-Length"))) {
		if (const int64 Result = FCString::Atoi64(**HeaderValue)) {
			return Result;
		}
	}

	return 0;
}

FString FHttpDownloadMetadata::LoadFromHeaders_Etag(const TMap<FString, FString>& Headers) {
	if (auto* HeaderValue = Headers.Find(TEXT("ETag"))) {
		return *HeaderValue;
	}
	return FString();
}

bool FHttpDownloadMetadata::LoadFromHeaders_AcceptRanges(const TMap<FString, FString>& Headers) {
	if (auto* HeaderValue = Headers.Find(TEXT("Accept-Ranges"))) {
		if (HeaderValue->ToLower() == TEXT("bytes")) {
			return true;
		}
	}
	return false;
}

FDateTime FHttpDownloadMetadata::LoadFromHeaders_LastModified(const TMap<FString, FString>& Headers) {
	if (auto* HeaderValue = Headers.Find(TEXT("Last-Modified"))) {
		FDateTime OutDateTime;
		if (FDateTime::ParseHttpDate(*HeaderValue, OutDateTime)) {
			return OutDateTime;
		}
	}
	return FDateTime();
}
