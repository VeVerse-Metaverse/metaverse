// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Download1Metadata.generated.h"

UENUM(BlueprintType)
enum class EDownload1Mode : uint8 {
	/** Overwrites the existing file. */
	Overwrite = 0,
	/** Tries to resume download if it is possible, requires range header to be enabled. */
	Resume
};

UENUM(BlueprintType)
enum class EDownload1Code : uint8 {
	Unknown = 0,
	Processing,
	Completed,
	Error,
	Cancelled,
};

USTRUCT(BlueprintType)
struct FDownload1RequestMetadata {
	GENERATED_BODY()

	/** URL from where the file will be downloaded. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Url = {};

	/** Length of the content at the server. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int64 ContentLength = 0;

	/** Path where downloaded file will be placed. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Path = {};

	/** Download priority, more is higher, -1 is reserved for top priority tasks. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 Priority = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EDownload1Mode Mode = EDownload1Mode::Overwrite;

	/** Should the download system use range download. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bUseRange = true;

	/** Should the download system use pre-signed URL (for private LE7EL cloud storage files). */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bUsePreSignedUrl = false;

	/** Entity ID for the pre-signed request. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGuid EntityId = {};

	/** File ID for the pre-signed request. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGuid FileId = {};

	/** Pre-signed URL (used to download private LE7EL cloud storage files). */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString PreSignedUrl;
};

USTRUCT(BlueprintType)
struct FDownload1ProgressMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FDownload1RequestMetadata RequestMetadata;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int64 ContentLength = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Progress = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EDownload1Code DownloadCode = EDownload1Code::Unknown;
};

USTRUCT(BlueprintType)
struct FDownload1ResultMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FDownload1RequestMetadata RequestMetadata;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Error;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Time = -1.0f;

	/** Total downloaded content length. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int64 ContentLength = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EDownload1Code DownloadCode = EDownload1Code::Unknown;
};

USTRUCT(BlueprintType)
struct FDownload1LinkMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Url;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Error;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bSuccessful = false;
};

USTRUCT(BlueprintType)
struct FDownload1HeaderMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Url;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString ETag;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int64 Size = 0;
};

typedef TDelegate<void(const FDownload1ProgressMetadata&)> FOnDownload1ProgressCallback;
typedef TDelegate<void(const FDownload1ResultMetadata&)> FOnDownload1CompleteCallback;
typedef TMulticastDelegate<void(const FDownload1RequestMetadata&)> FOnDownload1Request;
typedef TMulticastDelegate<void(const FDownload1ProgressMetadata&)> FOnDownload1Progress;
typedef TMulticastDelegate<void(const FDownload1ResultMetadata&)> FOnDownload1Complete;
