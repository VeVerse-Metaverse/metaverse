// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

//#include "CoreMinimal.h"
#include "MetadataFile.h"
#include "HttpDownloadTypes.generated.h"


struct FHttpDownloadContentMetadata {
	int64 ContentLength = 0;
	FDateTime LastModified;
	FString ETag;

	bool operator==(const FHttpDownloadContentMetadata&& rhs) const {
		return ContentLength == rhs.ContentLength
			&& LastModified == rhs.LastModified
			&& ETag == rhs.ETag;
	}

	bool IsEmpty() const {
		return operator==(FHttpDownloadContentMetadata());
	}
};


class VEDOWNLOAD_API FHttpDownloadMetadata : public FMetadataFile {
public:
	FHttpDownloadMetadata() = default;
	FHttpDownloadMetadata(const TMap<FString, FString>& Headers);
	virtual ~FHttpDownloadMetadata() = default;

	virtual void Empty() override;
	virtual bool Serialize(FArchive& Ar) override;

	int64 GetContentLength() const { return ContentMetadata.ContentLength; }
	const FDateTime& GetLastModified() const { return ContentMetadata.LastModified; }
	const FString& GetEtag() const { return ContentMetadata.ETag; }
	bool GetAcceptRanges() const { return AcceptRanges; }

	const FHttpDownloadContentMetadata& GetContentMetadata() const { return ContentMetadata; }
	void SetContentMetadata(const FHttpDownloadContentMetadata& InContentInfo) { ContentMetadata = InContentInfo; }

	int64 GetDownloadedSize() const { return DownloadedSize; }
	void SetDownloadedSize(int64 Size) { DownloadedSize = Size; }

	bool IsSourceChanged(const FHttpDownloadContentMetadata& InContentInfo) const;

	void LoadFromHeaders(const TMap<FString, FString>& Headers);

protected:
	virtual EMetadataFileType GetMetadataFileType() const override { return EMetadataFileType::HttpDownload; }

private:
	static int64 LoadFromHeaders_ContentLength(const TMap<FString, FString>& Headers);
	static FString LoadFromHeaders_Etag(const TMap<FString, FString>& Headers);
	static bool LoadFromHeaders_AcceptRanges(const TMap<FString, FString>& Headers);
	static FDateTime LoadFromHeaders_LastModified(const TMap<FString, FString>& Headers);

	uint8 MetadataVersion = 1;

	FHttpDownloadContentMetadata ContentMetadata;
	int64 DownloadedSize = 0;
	bool AcceptRanges = false;
};


struct FHttpDownloadItemOptions {

	FHttpDownloadContentMetadata ContentMetadata;

	/** Path where downloaded file will be placed. */
	FString Filename;

	/** Download priority, more is higher, -1 is reserved for top priority tasks. */
	int32 Priority = 0;
};


struct FHttpDownloadTaskOptions {

	FHttpDownloadContentMetadata ContentMetadata;

	/** Path where downloaded file will be placed. */
	FString Filename;

	/** Delete cache files before downloading. */
	bool DeleteCache = false;	
};


USTRUCT(BlueprintType)
struct FHttpDownloadProgressMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int64 ContentLength = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int64 DownloadedSize = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Progress = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 BytesPerSec = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Time = 0.0f;
};

UENUM(BlueprintType)
enum class EHttpDownloadCode : uint8 {
	Unknown = 0,
	Processing,
	Completed,
	Error,
	Cancelled,
};

USTRUCT(BlueprintType)
struct FHttpDownloadResultMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EHttpDownloadCode Code = EHttpDownloadCode::Unknown;

	FHttpDownloadContentMetadata ContentMetadata;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Filename;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Time = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Error;
};
